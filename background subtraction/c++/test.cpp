#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
//Foundamental Libraries
#include <iostream>
#include <vector>
#include <algorithm>
//#include <cmath>

using namespace cv;
using namespace std;

int main()
{
	int K = 5;
    //Define the initialization_model_size
    int initial_sample_size = 40;

    // Define the learning reate
    double alpha = 0.05;

	//Capture from live camera
	VideoCapture cap(0);

	//Access the video size
	int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
   	int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);

   	
	while(1)
	{
		Mat frame;
		bool bSuccess = cap.read(frame);
		if(!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		imshow("MyVideo",frame);
		if(waitKey(30) == 27)
		{
			cout << "esc is pressed" << endl;
			break;
		}
	}
	return 0;
}