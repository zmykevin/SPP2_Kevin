#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

class Gaussian_i
{
public:
    Mat mean;
    Mat stddev;
    Mat weights;
    Gaussian_i(int col, int row){
        mean = Mat(col, row, CV_8U, Scalar(0));
        stddev = Mat(col, row, CV_64F, Scalar(0));
        weights = Mat(col, row, CV_64F, Scalar(0));
    }

};

// bool sortMat (Mat i,Mat j) { return (i<j); }

int background_process_heuristic(vector<Gaussian_i>*, double);

int main(){

    cout << "Background Subtraction Start" << endl;
    /*
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
    */
    // cout<<frame_width<<endl;
    // cout<<frame_height<<endl;

    /*
    while(1)
    {
        cap >> frame;
        Mat gray_frame;
        cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
        imshow("video",gray_frame);
    }
    */
    }