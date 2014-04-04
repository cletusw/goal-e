 ////////////////////////////////////////////////////////////////////////////////
// SoftKinetic DepthSense SDK
//
// COPYRIGHT AND CONFIDENTIALITY NOTICE - SOFTKINETIC CONFIDENTIAL
// INFORMATION
//
// All rights reserved to SOFTKINETIC SENSORS NV (a
// company incorporated and existing under the laws of Belgium, with
// its principal place of business at Boulevard de la Plainelaan 15,
// 1050 Brussels (Belgium), registered with the Crossroads bank for
// enterprises under company number 0811 341 454 - "Softkinetic
// Sensors").
//
// The source code of the SoftKinetic DepthSense Camera Drivers is
// proprietary and confidential information of Softkinetic Sensors NV.
//
// For any question about terms and conditions, please contact:
// info@softkinetic.com Copyright (c) 2002-2012 Softkinetic Sensors NV
////////////////////////////////////////////////////////////////////////////////


// Some OpenCV mods added below for viewing and saving - Damian Lyons, dlyons@fordham.edu

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <stdio.h>
#include <time.h>
#include <math.h>

#include <vector>
#include <exception>

#include "cv.h"
#include "highgui.h"
#include "parameters.h"
#include <DepthSense.hxx>
#include "communicator.cpp"
#include </usr/include/python2.7/Python.h>

#define PI 3.141592653589793

using namespace DepthSense;
using namespace std;
using namespace cv;



// Open CV vars
//initial min and max HSV filter values.
//these will be changed using trackbars
 bool trackObjects = true;
 bool objectConfirmed = false;
 bool processingImage = false;
 int foundCount = 0;
    bool useMorphOps = true;
    float depth = 0;
    float areaAverage = 0;
    int sampleNum = 1;
    int x=0, y=0;
    float baseArea = 340.621765;
/*int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int RGB_SAMPLE = 25;
int HSV_SAMPLE = 25;
int ERODE_NUM = 25;
int DIALATE_NUM = 25;
int HOMOG_MEDIAN = 1;
int BLUR_SAMPLE = 25;
*/
//can't  segregate purple paper from red noodle
//teal paper and ceiling can't segregate

int H_MIN = 19;
int H_MAX = 37;
int S_MIN = 64;
int S_MAX = 110;
int V_MIN = 91;
int V_MAX = 256;
int RGB_SAMPLE = 5;
int HSV_SAMPLE = 5;
int ERODE_NUM = 1;
int DIALATE_NUM = 2;
int HOMOG_MEDIAN = 1;
int BLUR_SAMPLE = 25;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
static float anglePerPixel = 63.2 / FRAME_WIDTH;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 5*5;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";


IplImage 
  *g_depthImage=NULL, 
  *g_videoImage=NULL; // initialized in main, used in CBs
CvSize 
  g_szDepth=cvSize(320,240), // QQVGA
  g_szVideo=cvSize(640,480); //VGA

bool g_saveImageFlag=false, g_saveDepthFlag=false;
bool firstTime = true;
double firstTimeDiameter = 0;

Context g_context;
ColorNode g_cnode;

uint32_t g_cFrames = 0;

clock_t g_fTime;

bool g_bDeviceFound = false;

ProjectionHelper* g_pProjHelper = NULL;
StereoCameraParameters g_scp;


void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed





}

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

float findAngle(int x)
{
	float angle;
	
	int distFromCenter = -(x - 320);

	if(distFromCenter > 0)
	{
		angle = anglePerPixel * distFromCenter;
	}
	else
	{
		angle = anglePerPixel * (distFromCenter - 1);
	}

	return angle;
}

void morphOpsRGB(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
    cvtColor(thresh,thresh,CV_BGR2YCrCb);

    vector<Mat> channels;
    split(thresh,channels);

    equalizeHist(channels[0], channels[0]);

    Mat result;
    merge(channels,thresh);

    cvtColor(thresh,thresh,CV_YCrCb2BGR);
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(RGB_SAMPLE,RGB_SAMPLE));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(RGB_SAMPLE,RGB_SAMPLE));
	medianBlur( thresh, thresh, 11 );
	for(int i=DIALATE_NUM; i>0; i--)
	{
		dilate(thresh,thresh,dilateElement);
	}
	for(int i=ERODE_NUM; i>0; i--)
	{
		erode(thresh,thresh,erodeElement);
	}
		


}
void morphOpsHSV(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
    //equalizeHist( thresh, thresh );
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(HSV_SAMPLE,HSV_SAMPLE));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(HSV_SAMPLE,HSV_SAMPLE));
	for(int i=ERODE_NUM; i>0; i--)
	{
		erode(thresh,thresh,erodeElement);
	}
	for(int i=DIALATE_NUM; i>0; i--)
	{
		dilate(thresh,thresh,dilateElement);
	}

}
void runMotors(int speed1, int speed2, int speed3){

        motor_command command1;
        command1.action = 1;
        command1.board_address = 0x80;//C
        command1.motor_num = 0;
        command1.speed = speed1;
        motor_command command2;
        command2.action = 1;
        command2.board_address = 0x80;//B
        command2.motor_num = 1;
        command2.speed = speed2;
        motor_command command3;
        command3.action = 1;
        command3.board_address = 0x81;//A
        command3.motor_num = 0;
        command3.speed = speed3;
        
        run_python(&command1);
        run_python(&command2);
        run_python(&command3);
}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){

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
				if(moment.m01/area < 240)
				{
					continue;
				}

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
					//printf("area:%f\n", area);
					if((/*!firstTime && */sampleNum % 10 == 0) || (firstTime && sampleNum % 500 == 0)){
						areaAverage += area;
						sampleNum++;
						areaAverage /= sampleNum;
						if(firstTime){
							firstTimeDiameter = areaAverage;
							firstTime = false;
						}
						if(!firstTime){
							float ratio = sqrt(areaAverage) / sqrt(baseArea);
							depth = 5 / ratio;
							//printf("Difference:%f\n", difference);
							//DIALATE_NUM = 3-((int)depth/3);
							//printf("depth:%f\n", depth);
							//printf("average Area:%f\n", areaAverage);
							//printf("angle:%f\n", findAngle(x));
							//printf("firstTime:%f\n", firstTimeDiameter);
							printf("rooty:%f\n", sqrt(y));
						}
						areaAverage = 0;
						sampleNum = 1;
					}
					else {
						areaAverage += area;
						sampleNum++;
					}
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				if(!objectConfirmed && foundCount < 20){
				    if (objectFound){
                        foundCount++;
                        if(foundCount >= 5){
                            objectConfirmed = true;
                            foundCount = 0;
                        }
                    }
				}
				else if(objectConfirmed){
				    if (!objectFound){
                        foundCount++;
                        if(foundCount > 5){
                            objectConfirmed = false;
                            //foundCount = 0;
                        }
                    }
                    else
                        foundCount = 0;
                     float angle = findAngle(x);
                    if(abs(angle)*PI/180 > PI/60)
					{
                        if (angle > 0)
						{
                            printf("turn left\n");
                            runMotors(-60000,90000,-30000);
						}
                        else
						{
                            printf("turn right\n");
                           runMotors(60000,-90000,30000);
						}
					}
                    else
					{
                        printf("move forward!\n");
                        runMotors(-60000,0,30000);
				    } 
				}
				            //draw object location on screen
			}

		}
	}// 165 185 161 220 0 106
}
/*void goToBall(){
    //look for ball
    int foundCount = 0;
    while(!objectFound && foundCount < 20){
        if (objectFound)
            foundCount++;
        //runMotors(5000,5000,5000);
    }
    printf("Ball found!\n");
    foundCount = 0;
    while (objectFound || foundCount < 20){
        if (!objectFound)
            foundCount++;
        else
            foundCount = 0;
        float angle = findAngle(x);
        if(abs(angle) > PI/90)
            if (angle > 0)
                printf("turn greater 0");
               // runMotors(5000,5000,5000);
            else
                printf("turn less 0");
               // runMotors(-5000,-5000,-5000);
        else
            printf("move forward!");
           //runMotors(5000,-5000,0);
    }
    printf("ball lost");
}*/

// From SoftKinetic
// convert a YUY2 image to RGB

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
    if(!processingImage)
    {
        processingImage = true;
        int32_t w, h;
        //matrix storage for HSV image
	    Mat HSV;
	    //matrix storage for binary threshold image
	    Mat threshold;

        FrameFormat_toResolution(data.captureConfiguration.frameFormat,&w,&h);

        //yuy2rgb((unsigned char *)g_videoImage->imageData,data.colorMap,w,h);
        for(int i = 0; i < data.colorMap.size(); i++){
        	g_videoImage->imageData[i] = data.colorMap[i];
        }
        Mat image(g_videoImage, false);
        if(RGB_SAMPLE > 0)
        morphOpsRGB(image);
        cvtColor(image,HSV,COLOR_BGR2HSV);
	    //filter HSV image between values and store filtered image to
	    //threshold matrix
	    inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
	    //perform morphological operations on thresholded image to eliminate noise
	    //and emphasize the filtered object(s)
	    if(useMorphOps && HSV_SAMPLE > 0)
	    morphOpsHSV(threshold);
	    //pass in thresholded frame to our object tracking function
	    //this function will return the x and y coordinates of the
	    //filtered object
	    if(trackObjects)
		    trackFilteredObject(x,y,threshold,image);

	   
        //cvFlip(g_videoImage, NULL, 1);
        g_cFrames++;
        processingImage = false;
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
    if (data.node.is<ColorNode>() && (data.node.as<ColorNode>() == g_cnode))
        g_cnode.unset();
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
int main(int argc, char* argv[])
{
    
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
	  
    printf("dml@Fordham version of DS ConsoleDemo. June 2013.\n");
    printf("Click onto in image for commands. ESC to exit.\n");
    printf("Use \'W\' to toggle dumping of depth and visual images.\n");
    printf("Use \'w\' to toggle dumping of depth images only.\n\n");
    //create slider bars for HSV filtering
    g_context.startNodes();

    g_context.run();

    g_context.stopNodes();

    if (g_cnode.isSet()) g_context.unregisterNode(g_cnode);

    if (g_pProjHelper)
        delete g_pProjHelper;

    return 0;
}
