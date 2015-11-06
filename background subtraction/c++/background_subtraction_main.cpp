#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

typedef vector<double> double_vec_t;
struct mixture_gaussian
{
double_vec_t Mean;
double_vec_t Std;
double_vec_t Weight;
double_vec_t W_S;
};

int main()
{
cout << "Background Subtraction Start" << endl;

// Define the number of gaussina models in the mixed model
int K = 5;
//Define the initialization_model_size
int initial_size = 40;

// Define the learning reate
double alpha = 0.05;


// Start to capture the video
VideoCapture cap("highwayI_raw.AVI");

// Get the size dimension of the frame
int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
//------------------------------------------------//
//Initialization of the model
vector<mixture_gaussian> miture_gaussian_model;
vector<Mat> initial_model;

for (int i = 0; i < K; i ++)
{
    Mat initial_frame;
    Mat initial_gray_frame;
    cap >> initial_frame;
    cvtColor(initial_frame,initial_gray_frame,COLOR_BGR2GRAY);
    initial_model.push_back(initial_gray_frame);
}
cout << "The initial mat is set up" << endl;

//for (int i = 0; i < K; i ++)
//{
/*
Mat initial_frame;
cap >> initial_frame;
Mat initial_gray_frame;
cvtColor(initial_frame,initial_gray_frame,COLOR_BGR2GRAY);
//The next step is to convert the mat file to a vector
double_vec_t array_frame;
for(int i = 0; i < frame_height; i ++)
    for(int j = 0; j < frame_width; j ++)
    {
        double pixel_intensity = (double)initial_gray_frame.at<uchar>(i,j);
        array_frame.push_back(pixel_intensity);
    }
*/
//}



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