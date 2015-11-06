#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;
struct mixture_gaussian
{
vector<double> Mean;
vector<double> Std;
vector<double> Weight;
vector<double> W_S;
}
int main()
{
cout << "Background Subtraction Start" << endl;

// Define the number of gaussina models in the mixed model
int K = 5;
// Define the learning reate
double alpha = 0.05;
// Define initial weight parameter
double i_weight = 0.2;

// Start to capture the video
VideoCapture cap("highwayI_raw.AVI");

// Get the size dimension of the frame
int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);

vector<mixture_gaussian> miture_gaussian_model();
Mat initial_frame;
cap >> initial_frame;
Mat initial_gray_frame;
cvtColor(initial_frame,intial_gray_frame,COLOR_BGR2GRAY);


while(1)
{
    Mat frame;
    cap >> frame;
    Mat gray_frame;
    cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
    imshow("video",gray_frame);

    if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
   {
    cout << "esc key is pressed by user" << endl;
    break;
   }
}


return 0;
}