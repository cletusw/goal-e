// VisionObject.cpp

#include "VisionObject.h"

// Private Globals

// default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
// max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
// minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;

static float anglePerXPixel = 63.2 / FRAME_WIDTH;
static float anglePerYPixel = 49.3 / FRAME_HEIGHT;

// Public Globals
VisionObject Ball;


// Functions

void InitializeVisionObjects()
{
	InitializeBall();

}

void InitializeBall()
{
	Ball.found = false;
	Ball.x = -1;
	Ball.y = -1;
	float angle = -1;
	float depth = -1;	
}

float findXAngle(int x)
{
	float angle;
	
	int distFromCenter = -(x - 320);

	if(distFromCenter > 0)
	{
		angle = anglePerXPixel * distFromCenter;
	}
	else
	{
		angle = anglePerXPixel * (distFromCenter - 1);
	}

	return angle;
}

float findYAngle(int y)
{
	float angle;
	
	int distFromCenter = -(y - 240);

	if(distFromCenter > 0)
	{
		angle = anglePerYPixel * distFromCenter;
	}
	else
	{
		angle = anglePerYPixel * (distFromCenter - 1);
	}

	return angle;
}

void trackBall(Mat &HSV)
{
	Mat threshold;
	
	// Filter the image based on the Color of the Ball
	inRange(HSV, MinHSV(&Yellow), MaxHSV(&Yellow), threshold);

	// Morph the threshold image
	morphOperations(threshold);


	//imshow("Ball",threshold);
	// Track the image
	trackSingleObject(Ball.x, Ball.y, Ball.found, threshold);
}

// track a single Object
void trackSingleObject(int &x, int &y, bool &objectFound, Mat threshold)
{
	// these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// find contours of filtered image using openCV findContours function
	findContours(threshold, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	// use moments method to find our filtered object
	double refArea = 0;
	objectFound = false;
	if (hierarchy.size() > 0)
   	{
		int numObjects = hierarchy.size();

        // if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS)
		{
			for (int index = 0; index >= 0; index = hierarchy[index][0])
		   	{
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
				{
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}
				else 
				{
					objectFound = false;
				}
			}			
		}
		else
		{
			objectFound = false;
		}
	}
}


Scalar MinHSV(const Color *c)
{
	return Scalar(c->H_MIN, c->S_MIN, c->V_MIN);
}

Scalar MaxHSV(const Color *c)
{
	return Scalar(c->H_MAX, c->S_MAX, c->V_MAX);
}


void morphOperations(Mat &thresh)
{
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(6,6));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}

