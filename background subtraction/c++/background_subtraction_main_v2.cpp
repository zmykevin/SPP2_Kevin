//Opencv Libraries Included
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
//Foundamental Libraries
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

//backgournd Gaussian Process helps to rerank the gaussian_weight
Mat backgroundGaussianProcess(Mat* Gaussian_Mean, Mat* Gaussian_std, Mat* Gaussian_weight, Mat threshold);
int main()
{
	cout << "background subtraction" << endl;
	// Define the number of gaussina models in the mixed model
    int K = 5;
    //Define the initialization_model_size
    int initial_sample_size = 40;

    // Define the learning reate
    double alpha = 0.05;
    
    cout << "start to capture the video" << endl;
    
   	VideoCapture cap("highwayI_raw.AVI");
   	int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
   	int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
//--------------------------Initialization of Model-------------------------------------//
   	Mat gaussian_mean = Mat::zeros(K,frame_width*frame_height,CV_64F);
   	//Initialize the weight here
   	Mat gaussian_weight = Mat::ones(K,frame_width*frame_height,CV_64F)*0.2;
   	Mat gaussian_std = Mat::zeros(K,frame_width*frame_height,CV_64F);
   	for (int i = 0; i < K; i ++)
   	{
   		Mat frame;
   		cap >> frame;
   		Mat gray_frame;
   		cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
   		Mat gray_frame_1d = gray_frame.reshape(0,1);
   		gray_frame_1d.convertTo(gray_frame_1d,CV_64F);
   		//cout << gray_frame_1d.colRange(0,5) << endl;
   		gray_frame_1d.copyTo(gaussian_mean.row(i));
   		//cout << gaussian_mean.row(i).colRange(0,5) << endl;
   	}
   	for (int j = 0; j < initial_sample_size; j ++)
   	{
   		Mat frame;
   		cap >> frame;
   		Mat gray_frame;
   		cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
   		for (int k = 0; k < K; k ++)
   		{
   			Mat gray_frame_1d = gray_frame.reshape(0,1);
   			gray_frame_1d.convertTo(gray_frame_1d,CV_64F);
   			Mat difference = abs(gray_frame_1d-gaussian_mean.row(k))/(double)initial_sample_size;
   			//cout << difference.colRange(0,5) << endl;
   			Mat Sum = gaussian_std.row(k).clone();
   			//cout << Sum.colRange(0,5) << endl;
   			Sum += difference;
   			Sum.copyTo(gaussian_std.row(k));
   			//cout << gaussian_std.row(k).colRange(0,5) << endl;
   		}
   	}
   	//Next step is to define the function to do back process

while(1)
{
    Mat frame;
    cap >> frame;
    Mat gray_frame;
    cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
    imshow("original_video",gray_frame);
    //Start the main process here
//--------------------------Main Process-------------------------------------//
    //Create a Threshold
    Mat T = Mat::ones(K,frame_width*frame_height,CV_64F)*0.1;
    backgroundGaussianProcess(&gaussian_mean,&gaussian_std,&gaussian_weight,T);
    if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
   {
    cout << "ESC Key is pressed by user" << endl;
    break;
   }
}

	return 0;
}

Mat backgroundGaussianProcess(Mat* Gaussian_Mean, Mat* Gaussian_std, Mat* Gaussian_weight, Mat threshold)
{
	Mat result;
	int num_gaussian = Gaussian_Mean->size[0];
	cout << num_gaussian << endl;
	return result;
}