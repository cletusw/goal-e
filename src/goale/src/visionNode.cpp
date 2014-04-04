// Vision Node for Robot Soccer 2014
// Author: Alex Wilson & Brandon Williams
// In colaboration with other members of the Vision Team

#include <stdio.h>
#include <time.h>
#include <vector>
#include <exception>

#include "cv.h"
#include "highgui.h"

#include <DepthSense.hxx>

#include "VisionObject.h"

#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

using namespace DepthSense;
using namespace std;
using namespace cv;

// Function Prototypes 
void configureDepthNode();
void rosSetup();
void configureColorNode();
void configureNode(Node node);
void onNodeConnected(Device device, Device::NodeAddedData data);
void onNodeDisconnected(Device device, Device::NodeRemovedData data);
void onDeviceConnected(Context context, Context::DeviceAddedData data);
void onDeviceDisconnected(Context context, Context::DeviceRemovedData data);


//VisionObject OrangePillar;
//VisionObject Ball;


// Global Flags and Variables
bool trackObjects = true;
bool useMorphOps = true;
float depth =0;
int x=0, y=0;
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
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
void on_trackbar0( int, void* )
{//This function gets called whenever a
	// trackbar position is changed

}

IplImage 
  *g_depthImage=NULL, 
  *g_videoImage=NULL; // initialized in main, used in CBs
CvSize 
  g_szDepth=cvSize(320,240), // QQVGA
  g_szVideo=cvSize(640,480); //VGA

bool g_saveImageFlag=false, g_saveDepthFlag=false;

Context g_context;
DepthNode g_dnode;
ColorNode g_cnode;
AudioNode g_anode;

uint32_t g_aFrames = 0;
uint32_t g_cFrames = 0;
uint32_t g_dFrames = 0;

clock_t g_fTime;

bool g_bDeviceFound = false;

ProjectionHelper* g_pProjHelper = NULL;
StereoCameraParameters g_scp;

string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

ros::Publisher chatter_pub;

void rosSetup(int argc, char* argv[]){
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);
}

void drawObject(int x, int y,Mat &frame)
{
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
	Scalar green(0,255,0);

	circle(frame,Point(x,y),20,green,2);
    if(y-25>0)
    	line(frame,Point(x,y),Point(x,y-25),green,2);
    else
		line(frame,Point(x,y),Point(x,0),green,2);

    if(y+25<FRAME_HEIGHT)
    	line(frame,Point(x,y),Point(x,y+25),green,2);
    else
		line(frame,Point(x,y),Point(x,FRAME_HEIGHT),green,2);

    if(x-25>0)
    	line(frame,Point(x,y),Point(x-25,y),green,2);
    else
		line(frame,Point(x,y),Point(0,y),green,2);

    if(x+25<FRAME_WIDTH)
    	line(frame,Point(x,y),Point(x+25,y),green,2);
    else
		line(frame,Point(x,y),Point(FRAME_WIDTH,y),green,2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,green,2);

}

void morphOps(Mat &thresh)
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

// Function used for tracking Multiple Objects
void trackMultiFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
	Mat temp;
	threshold.copyTo(temp);

	// These two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );

	// Use moments method to find our filtered object
	double refArea[2] = {0, 0};
	int objectIndex[2] = {-1,-1};
	bool objectFound[2] = {false, false};
	int tempX[2];
	int tempY[2];
	if (hierarchy.size() > 0)
   	{
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects < MAX_NUM_OBJECTS)
		{
			// find largest two blobs
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			{
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				if(area > MIN_OBJECT_AREA && area < MAX_OBJECT_AREA)
				{
					if(area > refArea[0])
					{
						// Check to see if we need to move the 0 postion to
						// postion 1
						if(refArea[0] > refArea[1])
						{
							refArea[1] = refArea[0];
							objectIndex[1] = objectIndex[0];
						}
						refArea[0] = area;
						objectIndex[0] = index;
					}
					else if(area > refArea[1])
					{
						refArea[1] = area;
						objectIndex[1] = index;
					}
				}
			}

			for(int i = 0; i <= 1; i++)
			{
				if(objectIndex[i] > -1)
				{
					Moments moment = moments((cv::Mat)contours[objectIndex[i]]);
					double area = moment.m00;

					tempX[i] = moment.m10/area;
					tempY[i] = moment.m01/area;
					objectFound[i] = true;

					putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
					//draw object location on screen
					drawObject(tempX[i],tempY[i],cameraFeed);
				}
			}
			printf("hello %d %d \n", tempX[0], tempY[0]);
			x = tempX[0];
			y = tempY[0];

			/*
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			{
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea[index])
				{
					tempX[index] = moment.m10/area;
					tempY[index] = moment.m01/area;
					objectFound[index] = true;
					refArea[index] = area;
				}
				else
				{
					objectFound[index] = false;
				}


			//let user know you found an object
			if(objectFound[index] == true)
			{
				putText(cameraFeed,"Tracking Object",point,2,1,green,2);
				//draw object location on screen
				//drawObject(tempX[index],tempY[index],cameraFeed);
				int idx = 0;
   				 for( ; idx >= 0; idx = hierarchy[idx][0] )
   				 {
       			 Scalar color( 0, 0, 255 );
       			 drawContours( cameraFeed, contours, idx, color, 1, 8, hierarchy );
   				 }
			}

	findXAngle		}
			*/
		}
		else
		{	
			putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,255,0),2);
		}
	}
}


/*----------------------------------------------------------------------------*/
// New color sample event handler
/* Comments from SoftKinetic

From data you can get

::DepthSense::Pointer< uint8_t > colorMap
The color map. If captureConfiguration::compression is
DepthSense::COMPRESSION_TYPE_MJPEG, the output format is BGR, otherwise
the output format is YUY2.
 */
void onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data)
{
  //printf("C#%u: %d\n",g_cFrames,data.colorMap.size());
    
    int32_t w, h;
    //matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;

    FrameFormat_toResolution(data.captureConfiguration.frameFormat,&w,&h);

    for(int i = 0; i < data.colorMap.size(); i++)
	{
    	g_videoImage->imageData[i] = data.colorMap[i];
    }

    Mat image(g_videoImage, false);

	// Blur the image
	for (int i = 1; i < 10; i += 2)
    {
		//GaussianBlur( image, image, Size( i, i ), 0, 0 );
		blur( image, image, Size(i, i), Point(-1,-1) );
	}
	// Convert to HSV
    cvtColor(image,HSV,COLOR_BGR2HSV);

	//filter HSV image between values and store filtered image to
	//threshold matrix
	// Track Orange
	//inRange(HSV,Scalar(Orange.H_MIN,Orange.S_MIN,Orange.V_MIN),Scalar(Orange.H_MAX,Orange.S_MAX,Orange.V_MAX),threshold);
	//inRange(HSV,OrangePillar.MinHSV(),OrangePillar.MaxHSV(),threshold);
	//inRange(HSV,Ball.MinHSV(),Ball.MaxHSV(),threshold);
	//perform morphological operations on thresholded image to eliminate noise
	//and emphasize the filtered object(s)
	//if(useMorphOps)
	//	morphOps(threshold);
	//pass in thresholded frame to our object tracking function
	//this function will return the x and y coordinates of the
	//filtered object
	//if(trackObjects)
	//	trackMultiFilteredObject(x,y,threshold,image);
	
	float angle = findXAngle(Ball.x);
	printf("Ball x: %d\tx angle to Ball: %f\n",Ball.x, angle);
	printf("Ball y: %d\ty angle to Ball: %f\n",Ball.y, findYAngle(Ball.y));
	x = Ball.x;
	y = Ball.y;
	std_msgs::String msg;
	std::stringstream ss;
	ss << "ball angle " << angle;
	msg.data = ss.str();

	chatter_pub.publish(msg);

	ros::spinOnce();
	
	// Track the Ball
	trackBall(HSV);
	if(Ball.found)
	{
		drawObject(Ball.x,Ball.y,image);
	}
	

	//show frames 
	//imshow(windowName2,threshold);
	//imshow(windowName,image);
	//imshow(windowName1,HSV);
    //cvFlip(g_videoImage, NULL, 1);
    g_cFrames++;
}

/*----------------------------------------------------------------------------*/
// New depth sample event handler

/* From SoftKinetic

::DepthSense::Pointer< int16_t > depthMap
The depth map in fixed point format. This map represents the cartesian depth of each
pixel, expressed in millimeters. Valid values lies in the range [0 - 31999]. Saturated
pixels are given the special value 32002.
::DepthSense::Pointer< float > depthMapFloatingPoint
The depth map in floating point format. This map represents the cartesian depth of
each pixel, expressed in meters. Saturated pixels are given the special value -2.0.
*/

void onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data)
{
  //printf("Z#%u: %d\n",g_dFrames,data.vertices.size());

    int32_t w, h;
    FrameFormat_toResolution(data.captureConfiguration.frameFormat,&w,&h);

    int count=0; // DS data index
    //printf("\n\n\nwidth: %d  height: %d", w, h);
    if (data.depthMapFloatingPoint!=0)// just in case !
	{
		for (int i=0; i<h; i++)
		{
		  for (int j=0; j<w; j++)
		  {
			  // some arbitrary scaling to make this visible
			  //if(i == y/4 && j == x/4)
			  //printf("depth:%f\n",data.depthMapFloatingPoint[count]);
			  float val = data.depthMapFloatingPoint[count++];
			  //if (!g_saveImageFlag && !g_saveDepthFlag) val*=150;
			  if (val<0)
			  {
					val=255; // catch the saturated points
			  }
		  	  cvSet2D(g_depthImage,i,j,cvScalar(val));
			}
		}	
	}
	printf("x = %d %d", y, x);
	int index = ((y*320) + x)/4;
	depth = data.depthMapFloatingPoint[index];

    //for(int i=0; i < 20; i++)
    //	g_depthImage->imageData[index+i] = 150;
    /*for(int i = 0; i < w; i++)
    g_depthImage->imageData[y/2*320+i] = 150;
    for(int i = 0; i < h; i++)
    g_depthImage->imageData[(i)*320+x/2] = 150;
    */
	
    printf("depth:%f\n", depth);
    g_dFrames++;

    /*
    // Quit the main loop after 200 depth frames received
    if (g_dFrames == 20) {
      printf("Quitting main loop after MAX frames\n");
        g_context.quit();
    }
    */

    /* OpenCV display - this will slow stuff down, should be in thread*/

//    cvShowImage("Video",g_videoImage);
//    cvShowImage("Depth",g_depthImage);

    if (g_saveImageFlag || g_saveDepthFlag) { // save a timestamped image pair; synched by depth image time
      char filename[100];
      g_fTime = clock();
      sprintf(filename,"df%d.%d.jpg",(int)(g_fTime/CLOCKS_PER_SEC), (int)(g_fTime%CLOCKS_PER_SEC));
      cvSaveImage(filename,g_depthImage);
      sprintf(filename,"vf%d.%d.jpg",(int)(g_fTime/CLOCKS_PER_SEC), (int)(g_fTime%CLOCKS_PER_SEC));
      if (g_saveImageFlag) 
    cvSaveImage(filename,g_videoImage);
    }

    // Allow OpenCV to shut down the program
    char key = cvWaitKey(10);

    if (key==27) {
      printf("Quitting main loop from OpenCV\n");
        g_context.quit();
    } else 
      if (key=='W') g_saveImageFlag = !g_saveImageFlag;
      else if (key=='w') g_saveDepthFlag = !g_saveDepthFlag;
}

int main(int argc, char* argv[])
{
	// Perform device initilization
    g_context = Context::create("localhost");
    g_context.deviceAddedEvent().connect(&onDeviceConnected);
    g_context.deviceRemovedEvent().connect(&onDeviceDisconnected);

    // Get the list of currently connected devices
    vector<Device> da = g_context.getDevices();
	printf("devices: %d\n",da.size()); 
    // We are only interested in the first device
    if (da.size() >= 1)
    {
        g_bDeviceFound = true;

        da[0].nodeAddedEvent().connect(&onNodeConnected);
        da[0].nodeRemovedEvent().connect(&onNodeDisconnected);

        vector<Node> na = da[0].getNodes();
        
        printf("Found %u nodes\n",na.size());
        
        for (int n = 0; n < (int)na.size();n++)
            configureNode(na[n]);
    }

    /* Some OpenCV init; make windows and buffers to display the data */


    // VGA format color image
    g_videoImage=cvCreateImage(g_szVideo,IPL_DEPTH_8U,3);
    if (g_videoImage==NULL) 
      { printf("Unable to create video image buffer\n"); exit(0); }

    // QVGA format depth image
    g_depthImage=cvCreateImage(g_szDepth,IPL_DEPTH_8U,1);
    if (g_depthImage==NULL)
      { printf("Unable to create depth image buffer\n"); exit(0);}
    

	//OrangePillar.Initialize(&Orange);
	//Ball.Initialize(&Yellow);
	InitializeVisionObjects();

	rosSetup(argc, argv);
    g_context.startNodes();

    g_context.run();

    g_context.stopNodes();

    if (g_cnode.isSet()) g_context.unregisterNode(g_cnode);
    if (g_dnode.isSet()) g_context.unregisterNode(g_dnode);
    if (g_anode.isSet()) g_context.unregisterNode(g_anode);

    if (g_pProjHelper)
        delete g_pProjHelper;

    return 0;
}


/*----------------------------------------------------------------------------*/
void configureDepthNode()
{
    g_dnode.newSampleReceivedEvent().connect(&onNewDepthSample);

    DepthNode::Configuration config = g_dnode.getConfiguration();
    config.frameFormat = FRAME_FORMAT_QVGA;
    config.framerate = 25;
    config.mode = DepthNode::CAMERA_MODE_CLOSE_MODE;
    config.saturation = true;
    
    //    g_dnode.setEnableVertices(true);
    g_dnode.setEnableDepthMapFloatingPoint(true);



    try 
    {
        g_context.requestControl(g_dnode,0);

        g_dnode.setConfiguration(config);
    }
    catch (ArgumentException& e)
    {
        printf("DEPTH Argument Exception: %s\n",e.what());
    }
    catch (UnauthorizedAccessException& e)
    {
        printf("DEPTH Unauthorized Access Exception: %s\n",e.what());
    }
    catch (IOException& e)
    {
        printf("DEPTH IO Exception: %s\n",e.what());
    }
    catch (InvalidOperationException& e)
    {
        printf("DEPTH Invalid Operation Exception: %s\n",e.what());
    }
    catch (ConfigurationException& e)
    {
        printf("DEPTH Configuration Exception: %s\n",e.what());
    }
    catch (StreamingException& e)
    {
        printf("DEPTH Streaming Exception: %s\n",e.what());
    }
    catch (TimeoutException&)
    {
        printf("DEPTH TimeoutException\n");
    }

}

/*----------------------------------------------------------------------------*/
void configureColorNode()
{
    // connect new color sample handler
    g_cnode.newSampleReceivedEvent().connect(&onNewColorSample);

    ColorNode::Configuration config = g_cnode.getConfiguration();

    
    config.frameFormat = FRAME_FORMAT_VGA;
    config.compression = COMPRESSION_TYPE_MJPEG;
    
    //config.powerLineFrequency = POWER_LINE_FREQUENCY_50HZ;
    //config.framerate = 25;
    
    g_cnode.setEnableColorMap(true);


    try 
    {
        g_context.requestControl(g_cnode,0);

        g_cnode.setConfiguration(config);
    }
    catch (ArgumentException& e)
    {
        printf("COLOR Argument Exception: %s\n",e.what());
    }
    catch (UnauthorizedAccessException& e)
    {
        printf("COLOR Unauthorized Access Exception: %s\n",e.what());
    }
    catch (IOException& e)
    {
        printf("COLOR IO Exception: %s\n",e.what());
    }
    catch (InvalidOperationException& e)
    {
        printf("COLOR Invalid Operation Exception: %s\n",e.what());
    }
    catch (ConfigurationException& e)
    {
        printf("COLOR Configuration Exception: %s\n",e.what());
    }
    catch (StreamingException& e)
    {
        printf("COLOR Streaming Exception: %s\n",e.what());
    }
    catch (TimeoutException&)
    {
        printf("COLOR TimeoutException\n");
    }
}

/*----------------------------------------------------------------------------*/
void configureNode(Node node)
{
    if ((node.is<DepthNode>())&&(!g_dnode.isSet()))
    {
        g_dnode = node.as<DepthNode>();
        configureDepthNode();
        g_context.registerNode(node);
    }

    if ((node.is<ColorNode>())&&(!g_cnode.isSet()))
    {
        g_cnode = node.as<ColorNode>();
        configureColorNode();
        g_context.registerNode(node);
    }

}

/*----------------------------------------------------------------------------*/
void onNodeConnected(Device device, Device::NodeAddedData data)
{
    configureNode(data.node);
}

/*----------------------------------------------------------------------------*/
void onNodeDisconnected(Device device, Device::NodeRemovedData data)
{
    if (data.node.is<AudioNode>() && (data.node.as<AudioNode>() == g_anode))
        g_anode.unset();
    if (data.node.is<ColorNode>() && (data.node.as<ColorNode>() == g_cnode))
        g_cnode.unset();
    if (data.node.is<DepthNode>() && (data.node.as<DepthNode>() == g_dnode))
        g_dnode.unset();
    printf("Node disconnected\n");
}

/*----------------------------------------------------------------------------*/
void onDeviceConnected(Context context, Context::DeviceAddedData data)
{
    if (!g_bDeviceFound)
    {
        data.device.nodeAddedEvent().connect(&onNodeConnected);
        data.device.nodeRemovedEvent().connect(&onNodeDisconnected);
        g_bDeviceFound = true;
    }
}

/*----------------------------------------------------------------------------*/
void onDeviceDisconnected(Context context, Context::DeviceRemovedData data)
{
    g_bDeviceFound = false;
    printf("Device disconnected\n");
}

/*----------------------------------------------------------------------------*/

