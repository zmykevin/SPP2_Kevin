//Opencv Libraries Included
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

const double PI = 3.1415926;

//backgournd Gaussian Process helps to rerank the gaussian_weight
Mat compare_to_scalar(Mat matrix_1, int scalar);

void Sort_Gaussian(Mat* Gaussian_Mean, Mat* Gaussian_std, Mat* Gaussian_weight);

Mat backgroundGaussianProcess(Mat Gaussian_weight, Mat T);

Mat Matching_Gaussian(Mat Gaussian_mean, Mat Gaussian_std, Mat Current_Frame,Mat* history, Mat* Matching_Matrix);

void Update_Gaussian(Mat* Gaussian_mean, Mat* Gaussian_std, Mat* Gaussian_weight, Mat Current_Frame, Mat matching_result, Mat matching_matrix,double alpha);

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
    bool read_success = cap.read(frame);
    if (read_success)
    {
    Mat gray_frame;
    cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
    //Start the main process here
//--------------------------Main Process-------------------------------------//
    //Convert the frame to 1d
    Mat gray_frame_1d = gray_frame.reshape(0,1);
   	gray_frame_1d.convertTo(gray_frame_1d,CV_64F);
    
    //Create a Threshold
    Sort_Gaussian(&gaussian_mean,&gaussian_std,&gaussian_weight);
    Mat T = gaussian_weight.row(0);
    //Generate the Background Process
    Mat background_gaussian_model = backgroundGaussianProcess(gaussian_weight,T);

    //Find the Matching Gaussian
    Mat matching_result = Mat::zeros(1,frame_height*frame_width,CV_64F);
    Mat matching_matrix = Mat::zeros(K,frame_height*frame_width,CV_64F);
    Mat matching_model = Matching_Gaussian(gaussian_mean,gaussian_std,gray_frame_1d,&matching_result,&matching_matrix);
    //Print out the result
    Mat subtraction_result_1d;
    //cout << matching_model.colRange(300,500) << endl;
    //cout << background_gaussian_model.colRange(300,500) << endl;
    compare(matching_model,background_gaussian_model,subtraction_result_1d,CMP_GT);
    Mat subtraction_result = subtraction_result_1d.reshape(0,frame_height);
    //Update Parameters
    Update_Gaussian(&gaussian_mean,&gaussian_std,&gaussian_weight,gray_frame_1d,matching_result,matching_matrix,alpha);
    
    imshow("original_video",gray_frame);
    imshow("background_subtraction",subtraction_result);
    if(waitKey(1) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
   {
    cout << "ESC Key is pressed by user" << endl;
    break;
   }
}
else
{
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



Mat backgroundGaussianProcess(Mat Gaussian_weight, Mat T)
{
	Mat result = Mat::zeros(1,Gaussian_weight.size[1],CV_64F);
	int num_gaussian = Gaussian_weight.size[0];
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat sum_i;
		reduce(Gaussian_weight.rowRange(0,i+1),sum_i,0,CV_REDUCE_SUM, CV_64F);
		Mat compare_result;
		compare(sum_i,T,compare_result,CMP_GE);
		compare_result = compare_result/255;
		compare_result.convertTo(compare_result,CV_64F);
		result += compare_result; 
	}
	result = num_gaussian-result+1;
	return result;
}


Mat Matching_Gaussian(Mat Gaussian_mean, Mat Gaussian_std, Mat Current_Frame,Mat* history, Mat* Matching_Matrix)
{
	Mat result = Mat::zeros(1,Gaussian_mean.size[1],CV_64F);
	double std_scale = 2.5;
	int num_gaussian = Gaussian_mean.size[0];
	for(int i = 0; i <num_gaussian; i ++)
	{
		Mat difference = abs(Gaussian_mean.row(i)-Current_Frame);
		Mat scaled_std = Gaussian_std.row(i)*std_scale;
		//cout << difference.colRange(329,331) << endl;
		//cout << scaled_std.colRange(329,331) << endl;

		Mat compare_result;
		compare(difference,scaled_std,compare_result,CMP_LE);
		compare_result = compare_result/255;
		compare_result.convertTo(compare_result,CV_64F);
		//cout << compare_result.colRange(329,331) << endl;

		Mat result_i;

		compare(compare_result-*history,Mat::zeros(1,Gaussian_mean.size[1],CV_64F),result_i,CMP_GT);
		result_i = result_i/255;
		//cout << history->colRange(329,331) << endl;
		//cout << result_i.colRange(329,331) << endl;

		result_i.convertTo(result_i,CV_64F);
		result_i.copyTo(Matching_Matrix->row(i));//updated Matching_Matrix
		result += (i+1)*result_i;
		
		Mat new_history;
		bitwise_or(compare_result,*history,new_history);
		new_history.convertTo(*history,CV_64F);
	}
	Mat zero_matrix = Mat::zeros(1,Gaussian_mean.size[1],CV_64F);
	Mat modify_zero;
	bitwise_or(*history,zero_matrix,modify_zero);
	modify_zero = (1-modify_zero)*10;
	modify_zero.convertTo(modify_zero,CV_64F);
	result = result + modify_zero;

	return result;
}

void Update_Gaussian(Mat* Gaussian_mean, Mat* Gaussian_std, Mat* Gaussian_weight, Mat Current_Frame, Mat matching_result, Mat matching_matrix,double alpha)
{
	double low_weight = 0;
	//First let's update the non-matching model
	int num_gaussian = Gaussian_mean->size[0];
	Mat non_matching_model = (double)1-matching_result;
	//cout << non_matching_model.colRange(102,104) << endl;
	Mat original_gaussian_mean = Gaussian_mean->clone();
	Mat original_gaussian_std = Gaussian_std->clone();
	Mat original_gaussian_weight = Gaussian_weight->clone();

	//Update Mean
	Mat non_matching_new_mean = Current_Frame.mul(non_matching_model)+(original_gaussian_mean.row(num_gaussian-1)).mul(matching_result);
	//cout << Current_Frame.colRange(102,104) << endl;
	//cout << non_matching_new_mean.colRange(102,104) << endl;
	non_matching_new_mean.copyTo(Gaussian_mean->row(num_gaussian-1));
	//cout << (Gaussian_mean->row(num_gaussian-1)).colRange(102,104) << endl;

	//update Std
	Mat A = abs(255-Gaussian_mean->row(num_gaussian-1));
	Mat B = abs(0-Gaussian_mean->row(num_gaussian-1));
	//cout << A.colRange(102,104) << endl;
	//cout << B.colRange(102,104) << endl;

	Mat updated_std;
	max(A,B,updated_std);
	//cout << updated_std.colRange(102,104) << endl;

	updated_std = updated_std.mul(non_matching_model)+(original_gaussian_std.row(num_gaussian-1)).mul(matching_result);
	//cout << updated_std.colRange(102,104) << endl;
	updated_std.copyTo(Gaussian_std->row(num_gaussian-1));
	//update weight
	Mat updated_weight = (Gaussian_weight->row(num_gaussian-1)).mul(matching_result);
	updated_weight.copyTo(Gaussian_weight->row(num_gaussian-1));
	Mat weight_distribution = (original_gaussian_weight.row(num_gaussian-1)).mul(non_matching_model)/(double)(num_gaussian-1);
	for (int i = 0; i < num_gaussian-1; i ++)
	{
		Mat new_weight_i = original_gaussian_weight.row(i) + weight_distribution;
		new_weight_i.copyTo(Gaussian_weight->row(i)); 
	}
	//cout << Gaussian_weight->col(102) << endl;
	original_gaussian_mean = Gaussian_mean->clone();
	original_gaussian_std = Gaussian_std->clone();
	original_gaussian_weight = Gaussian_weight->clone();
	//Update the Matching Model
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat gaussian_mean_i = original_gaussian_mean.row(i);
		Mat gaussian_weight_i = original_gaussian_weight.row(i);
		Mat gaussian_std_i = original_gaussian_std.row(i)+pow(10.0,-8);

		Mat matching_matrix_i = matching_matrix.row(i);

		//Define the gaussian distribution probability
		Mat difference_square_i;
		pow(abs(Current_Frame-gaussian_mean_i),2,difference_square_i);
		//define the square of standard deviation as varaince
		Mat gaussian_variance_i;
		pow(gaussian_std_i,2,gaussian_variance_i);

		//compute the result inside the exponential function
		Mat inside_exponential = (-0.5)*(difference_square_i/gaussian_variance_i);

		//compute the front part of the gaussian_model
		Mat head_part = (1/sqrt(2*PI))/gaussian_std_i;

		//compute the exponential compoenent
		Mat exponential_component;
		exp(inside_exponential,exponential_component);

		//compute the gaussian desnity distribution
		Mat GDP = head_part.mul(exponential_component);
		Mat P = alpha*GDP;
		
		//update_weightfor_the gaussian_model
		Mat matching_k_model_weight = matching_result.mul(gaussian_weight_i);
		Mat updated_matching_weight = non_matching_model.mul(gaussian_weight_i)+(1-alpha)*matching_k_model_weight+alpha*matching_k_model_weight;
		updated_matching_weight.copyTo(Gaussian_weight->row(i));

		//update mean for matching k models
		
		Mat updated_matching_mean = (1-matching_matrix_i).mul(gaussian_mean_i)+(1-P).mul(matching_matrix_i.mul(gaussian_mean_i))+P.mul(matching_matrix_i.mul(Current_Frame));
		updated_matching_mean.copyTo(Gaussian_mean->row(i));
		

		//update std for matching k models
		 Mat new_varaiance_i;
		 pow((Current_Frame-Gaussian_mean->row(i)),2,new_varaiance_i);
		 Mat updated_matching_variance = (1-P).mul(matching_matrix_i.mul(gaussian_variance_i))+P.mul(matching_matrix_i.mul(new_varaiance_i));
		 Mat updated_std;
		 sqrt(updated_matching_variance,updated_std);
		 updated_std = (1-matching_matrix_i).mul(gaussian_std_i) + updated_std;
		 updated_std.copyTo(Gaussian_std->row(i));
	}
}
