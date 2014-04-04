// The Header file for the VisionObject Class

#include "cv.h"
#include "highgui.h"

using namespace cv;

// Color Definitions
typedef struct
{
	int H_MIN;
	int H_MAX;
	int S_MIN;
	int S_MAX;
	int V_MIN;
	int V_MAX;
} Color;

const Color Orange =
{
	0,   	// H_MIN
	15, 	// H_MAX
	121,   	// S_MIN
	209,	// S_MAX
	130,   	// V_MIN
	208, 	// V_MAX
};

const Color Yellow =
{
	33,   	// H_MIN
	49, 	// H_MAX
	63,   	// S_MIN
	166,	// S_MAX
	82,   	// V_MIN
	256, 	// V_MAX
};

// ColorObject Definition
typedef struct
{
	bool found;
	int x;
	int y;
	const Color *color;
	Scalar min;
	Scalar max;
} ColorObject;

// VisionObject Definition
typedef struct
{
	bool found;
	int x;
	int y;
	float angle;
	float depth;
	//ColorObject cObject;
} VisionObject;

// Extern Definitions
extern VisionObject Ball;

// Function Prototypes
void InitializeVisionObjects();
void InitializeBall();
float findXAngle(int x);
float findYAngle(int y);

void trackBall(Mat &HSV);

void trackSingleObject(int &x, int &y, bool &objectFound, Mat threshold);
Scalar MinHSV(const Color *c);
Scalar MaxHSV(const Color *c);
void morphOperations(Mat &thresh);


