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
Mat compare_to_scalar(Mat matrix_1, int scalar);
void Sort_Gaussian(Mat* Gaussian_Mean, Mat* Gaussian_std, Mat* Gaussian_weight);

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
    //Mat T = Mat::ones(K,frame_width*frame_height,CV_64F)*0.1;
    Sort_Gaussian(&gaussian_mean,&gaussian_std,&gaussian_weight);
    if(waitKey(1) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
   {
    cout << "ESC Key is pressed by user" << endl;
    break;
   }
}

	return 0;
}

Mat compare_to_scalar(Mat matrix_1, int scalar)
{
	//First of all, convert it to a specific type
	matrix_1.convertTo(matrix_1,CV_64F);
	//Construct a scalar matrix
	Mat scalar_matrix = Mat::ones(matrix_1.size[0],matrix_1.size[1],CV_64F)*double(scalar);
	//Now Compare the two matrix
	Mat compare_result;
	compare(matrix_1,scalar_matrix,compare_result,CMP_EQ);
	compare_result = compare_result/double(255);
	compare_result.convertTo(compare_result,CV_64F);
	return compare_result;
}

void Sort_Gaussian(Mat* Gaussian_mean, Mat* Gaussian_std, Mat* Gaussian_weight)
{
	Mat result;
	int num_gaussian = Gaussian_mean->size[0];
	Mat W_by_SD = Mat::zeros(num_gaussian,Gaussian_mean->size[1],CV_64F);
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat division_result;
		divide((*Gaussian_weight).row(i),(*Gaussian_std).row(i),division_result);
		division_result.copyTo(W_by_SD.row(i));
	}
	//Sort W_by_SD and return index
	Mat sort_index;
	sortIdx(W_by_SD,sort_index,SORT_EVERY_COLUMN+SORT_DESCENDING);
	//The next step is to reorder all the parameters with respect to this rank
	Mat original_gaussian_mean = (*Gaussian_mean).clone();
	Mat original_gaussian_std = (*Gaussian_std).clone();
	Mat original_gaussian_weight = (*Gaussian_weight).clone();
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat new_gaussian_mean_i = Mat::zeros(1,Gaussian_mean->size[1],CV_64F);
		Mat new_gaussian_std_i = Mat::zeros(1,Gaussian_std->size[1],CV_64F);
		Mat new_gaussian_weight_i = Mat::zeros(1,Gaussian_weight->size[1],CV_64F);
		for (int j = 0; j < num_gaussian; j ++)
		{
			Mat comparison_result = compare_to_scalar(sort_index.row(i),j);
			new_gaussian_mean_i += (original_gaussian_mean.row(j)).mul(comparison_result);
			new_gaussian_weight_i += (original_gaussian_weight.row(j)).mul(comparison_result);
			new_gaussian_std_i += (original_gaussian_std.row(j)).mul(comparison_result);
		}
		new_gaussian_mean_i.copyTo(Gaussian_mean->row(i));
		new_gaussian_weight_i.copyTo(Gaussian_weight->row(i));
		new_gaussian_std_i.copyTo(Gaussian_std->row(i));
	}
}

