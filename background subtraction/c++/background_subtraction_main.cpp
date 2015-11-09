#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

typedef vector<float> float_vec_t;
struct mixture_gaussian
{
float_vec_t Mean;
float_vec_t Std;
float_vec_t Weight;
float_vec_t W_S;
};

void Initialization(vector<Mat>* initial_mean, Mat* initial_std);
vector<float> frame2vector(Mat frame,int frame_height, int frame_width);


int main()
{
    cout << "Background Subtraction Start" << endl;

    // Define the number of gaussina models in the mixed model
    int K = 5;
    //Define the initialization_model_size
    int initial_sample_size = 40;

    // Define the learning reate
    double alpha = 0.05;


    // Start to capture the video
    VideoCapture cap("highwayI_raw.AVI");

    // Get the size dimension of the frame
    int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
//------------------------------------------------//
//Initialization of the model
    
    int ret;
    Mat frame;
    //Define the initialization of mixture_gaussian_mean
    vector<float_vec_t> initial_mixture_gaussian_mean(K);
    vector<float_vec_t> initial_mixture_gaussian_std(K);
    vector<float_vec_t> initial_mixture_gaussian_weight(K);
    vector<float_vec_t> initial_mixture_gaussian_WbySD(K);

    for (int i=0; i<K; i++)
    {
        cap >> frame;
        // initilize the mean values
        float_vec_t gaussian_mean = frame2vector(frame, frame_height, frame_width);
        //Append the ith gaussian model to the mixture
        initial_mixture_gaussian_mean[i] = gaussian_mean;
        //Initialize the gaussian_stdev
        float_vec_t gaussian_stddev(frame_height*frame_width, 0);
        initial_mixture_gaussian_std[i] = gaussian_stddev;
        //Initialie the gaussian_weight
        float_vec_t gaussian_weight(frame_height*frame_width,0.2);
        initial_mixture_gaussian_weight[i] = gaussian_weight;
        //initialize the gaussian WbySD
        float_vec_t gaussian_wbySD(frame_height*frame_width,0);
        initial_mixture_gaussian_WbySD[i] = gaussian_wbySD;
    }
    
    
    // initialize the standard deviation
    for (int j = 0; j < initial_sample_size; j++)
    {
        cap >> frame;
        float_vec_t gaussian_i = frame2vector(frame, frame_height, frame_width);
        //cout << "value at 0,0:" << gaussian_i[0] << endl;
        for (int k = 0; k < K; k ++)
        {
        float_vec_t temp_diff;
        //cout << "mean at 0,0" << initial_mixture_gaussian_mean[k].at(0) << endl;
        absdiff(gaussian_i, initial_mixture_gaussian_mean[k], temp_diff);
        //cout << "temp_diff at 0,0" << temp_diff[0] << endl;
        add(temp_diff, initial_mixture_gaussian_std[k], initial_mixture_gaussian_std[k]); 
        //cout << "gaussian_accumulation at 0,0" << initial_mixture_gaussian_std[k][0] << endl;
        }
    }

    for (int i = 0; i < K; i ++)
    {
        divide(initial_mixture_gaussian_std[i],initial_sample_size,initial_mixture_gaussian_std[i]);
        add(initial_mixture_gaussian_std[i],pow(10,-8),initial_mixture_gaussian_std[i]);
        //Define W_bySD
        divide(initial_mixture_gaussian_weight[i],initial_mixture_gaussian_std[i],initial_mixture_gaussian_WbySD[i]);
    }
    cout << "initialization is done" << endl;



while(1)
{
    Mat frame;
    cap >> frame;
    float_vec_t frame_vec = frame2vector(frame,frame_height,frame_width);
    Mat background_subtraction_result;
    background_subtraction_result = Mat::zeros(frame_height,frame_width,CV_32F);
    cout << background_subtraction_result << endl;

    for (int i = 0; i < frame_height*frame_width; i ++)
    {

    }
    
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

vector<float> frame2vector(Mat frame,int frame_height, int frame_width)
{
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    resize(frame, frame, Size(frame_height, frame_width)); //For debugging
    frame.convertTo(frame, CV_32F);
    float_vec_t array((float*)frame.data,(float*)frame.data + frame.rows * frame.cols);
    return array;
}