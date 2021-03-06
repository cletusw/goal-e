//objectTrackingTutorial.cpp

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <highgui.h>
#include <cv.h>

using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars

//object 0
int H_MIN0 = 0;
int H_MAX0 = 256;
int S_MIN0 = 0;
int S_MAX0 = 256;
int V_MIN0 = 0;
int V_MAX0 = 256;
int ERODE_MIN0 = 0;
int ERODE_MAX0 = 10;
int DILATE_MIN0 = 0;
int DILATE_MAX0 = 50;

//object 1
int H_MIN1 = 0;
int H_MAX1 = 256;
int S_MIN1 = 0;
int S_MAX1 = 256;
int V_MIN1 = 0;
int V_MAX1 = 256;
int ERODE_MIN1 = 0;
int ERODE_MAX1 = 10;
int DILATE_MIN1 = 0;
int DILATE_MAX1 = 50;


//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName20 = "Thresholded Image 0";
const string windowName21 = "Thresholded Image 1";
const string windowName3 = "After Morphological Operations 0";
const string windowName4 = "After Morphological Operations 1";
const string trackbarWindowName0 = "Trackbars 0";
const string trackbarWindowName1 = "Trackbars 1";
void on_trackbar0( int, void* )
{//This function gets called whenever a
	// trackbar position is changed

}


void on_trackbar1( int, void* )
{//This function gets called whenever a
	// trackbar position is changed



}
string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}
void createTrackbars0(){
	//create window for trackbars


    namedWindow(trackbarWindowName0,0);
	//create memory to store trackbar name on window
	char TrackbarName0[50];
	sprintf( TrackbarName0, "H_MIN0", H_MIN0);
	sprintf( TrackbarName0, "H_MAX0", H_MAX0);
	sprintf( TrackbarName0, "S_MIN0", S_MIN0);
	sprintf( TrackbarName0, "S_MAX0", S_MAX0);
	sprintf( TrackbarName0, "V_MIN0", V_MIN0);
	sprintf( TrackbarName0, "V_MAX0", V_MAX0);
	sprintf( TrackbarName0, "ERODE_MIN0", ERODE_MIN0);
	sprintf( TrackbarName0, "ERODE_MAX0", ERODE_MAX0);
	sprintf( TrackbarName0, "DILATE_MIN0", ERODE_MIN0);
	sprintf( TrackbarName0, "DILATE_MAX0", ERODE_MAX0);
	
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", trackbarWindowName0, &H_MIN0, H_MAX0, on_trackbar0 );
    createTrackbar( "H_MAX", trackbarWindowName0, &H_MAX0, H_MAX0, on_trackbar0 );
    createTrackbar( "S_MIN", trackbarWindowName0, &S_MIN0, S_MAX0, on_trackbar0 );
    createTrackbar( "S_MAX", trackbarWindowName0, &S_MAX0, S_MAX0, on_trackbar0 );
    createTrackbar( "V_MIN", trackbarWindowName0, &V_MIN0, V_MAX0, on_trackbar0 );
    createTrackbar( "V_MAX", trackbarWindowName0, &V_MAX0, V_MAX0, on_trackbar0 );
    createTrackbar( "Errode", trackbarWindowName0, &ERODE_MIN0, ERODE_MAX0, on_trackbar0);
    createTrackbar( "Errode", trackbarWindowName0, &ERODE_MAX0, ERODE_MAX0, on_trackbar0);
    createTrackbar( "Dilate", trackbarWindowName0, &DILATE_MIN0, DILATE_MAX0, on_trackbar0);
    createTrackbar( "Dilate", trackbarWindowName0, &DILATE_MAX0, DILATE_MAX0, on_trackbar0);

}


void createTrackbars1(){
	//create window for trackbars


    namedWindow(trackbarWindowName1,0);
	//create memory to store trackbar name on window
	char TrackbarName1[50];
	sprintf( TrackbarName1, "H_MIN1", H_MIN1);
	sprintf( TrackbarName1, "H_MAX1", H_MAX1);
	sprintf( TrackbarName1, "S_MIN1", S_MIN1);
	sprintf( TrackbarName1, "S_MAX1", S_MAX1);
	sprintf( TrackbarName1, "V_MIN1", V_MIN1);
	sprintf( TrackbarName1, "V_MAX1", V_MAX1);
	sprintf( TrackbarName1, "ERODE_MIN1", ERODE_MIN1);
	sprintf( TrackbarName1, "ERODE_MAX1", ERODE_MAX1);
	sprintf( TrackbarName1, "DILATE_MIN1", ERODE_MIN1);
	sprintf( TrackbarName1, "DILATE_MAX1", ERODE_MAX1);
	
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", trackbarWindowName1, &H_MIN1, H_MAX1, on_trackbar1);
    createTrackbar( "H_MAX", trackbarWindowName1, &H_MAX1, H_MAX1, on_trackbar1 );
    createTrackbar( "S_MIN", trackbarWindowName1, &S_MIN1, S_MAX1, on_trackbar1 );
    createTrackbar( "S_MAX", trackbarWindowName1, &S_MAX1, S_MAX1, on_trackbar1 );
    createTrackbar( "V_MIN", trackbarWindowName1, &V_MIN1, V_MAX1, on_trackbar1 );
    createTrackbar( "V_MAX", trackbarWindowName1, &V_MAX1, V_MAX1, on_trackbar1 );
    createTrackbar( "Errode", trackbarWindowName1, &ERODE_MIN1, ERODE_MAX1, on_trackbar1);
    createTrackbar( "Errode", trackbarWindowName1, &ERODE_MAX1, ERODE_MAX1, on_trackbar1);
    createTrackbar( "Dilate", trackbarWindowName1, &DILATE_MIN1, DILATE_MAX1, on_trackbar1);
    createTrackbar( "Dilate", trackbarWindowName1, &DILATE_MAX1, DILATE_MAX1, on_trackbar1);
}



void drawObject(int x, int y,Mat &frame){

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}


void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(ERODE_MAX0,ERODE_MAX0));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(DILATE_MAX0,DILATE_MAX0));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	


}
void trackFilteredObject0(int &x, int &y, Mat threshold, Mat &cameraFeed){

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}// 165 185 161 220 0 106
}


void trackFilteredObject1(int &x, int &y, Mat threshold, Mat &cameraFeed){

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}// 165 185 161 220 0 106
}



int main(int argc, char* argv[])
{
	//some boolean variables for different functionality within this
	//program
    bool trackObjects = true;
    bool useMorphOps = true;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold0;
	Mat threshold1;
	//x and y values for the location of the object
	int x0=0, y0=0;
	int x1=0, y1=0;
	//create slider bars for HSV filtering
	createTrackbars0();
	createTrackbars1();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1){
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		//filter HSV image between values and store filtered image to
		//threshold matrix
		inRange(HSV,Scalar(H_MIN0,S_MIN0,V_MIN0),Scalar(H_MAX0,S_MAX0,V_MAX0),threshold0);
		inRange(HSV,Scalar(H_MIN1,S_MIN1,V_MIN1),Scalar(H_MAX1,S_MAX1,V_MAX1),threshold1);
		//perform morphological operations on thresholded image to eliminate noise
		//and emphasize the filtered object(s)
		if(useMorphOps)
		morphOps(threshold0);
		morphOps(threshold1);
		//pass in thresholded frame to our object tracking function
		//this function will return the x and y coordinates of the
		//filtered object
		if(trackObjects)
			trackFilteredObject0(x0,y0,threshold0,cameraFeed);
		if(trackObjects)
			trackFilteredObject1(x1,y1,threshold1,cameraFeed);

		//show frames 
		imshow(windowName20,threshold0);
		imshow(windowName21,threshold1);
		imshow(windowName,cameraFeed);
		imshow(windowName1,HSV);
		

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}






	return 0;
}
