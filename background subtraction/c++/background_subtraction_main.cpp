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

float_vec_t frame2vector(Mat frame,int frame_height, int frame_width);

void vec4to1(vector<float_vec_t >* mixture_gaussian_mean, vector<float_vec_t >* mixture_gaussian_stddev, vector<float_vec_t >* mixture_gaussian_weights, vector<float_vec_t >* mixture_gaussian_W_S, vector<mixture_gaussian> * mixture_gaussian_model);

int matching_models(mixture_gaussian mixture_gaussian_model, float pixel_intensity);

void sortGaussian(mixture_gaussian* gaussian_model);

int backgroundGaussianProcess(mixture_gaussian* gaussian_model, float T);

//Start the Main Process
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
    //Now combined the initialized vector into one big model
    vector<mixture_gaussian> mixture_gaussian_model(frame_height*frame_width);
    vec4to1(&initial_mixture_gaussian_mean,&initial_mixture_gaussian_std, &initial_mixture_gaussian_weight, &initial_mixture_gaussian_WbySD, &mixture_gaussian_model);

//Suppose we have a vector of mixture gaussian already. 
while(1)
{
    Mat frame;
    cap >> frame;
    float_vec_t frame_vec = frame2vector(frame,frame_height,frame_width);
    Mat background_subtraction_result;
    background_subtraction_result = Mat::zeros(frame_height,frame_width,CV_8UC1);//Initialize the backgroudn subtraction result
    uint8_t* result_data = background_subtraction_result.data;
    for (int i = 0; i < frame_height*frame_width; i ++)
    {

        //background process heusristics
        sortGaussian(& mixture_gaussian_model[i]);
        
        // int background_model = backgroundGaussianProcess(& mixture_gaussian_model[i],0.5);
        
        //Find the matching models
        // float intensity = frame_vec[i];
        // int matching_model_num = matching_models(mixture_gaussian_model[i],intensity);
        //Define background or foreground
        // if (background_model < matching_model_num)
            // result_data[i] = 255;
        //update parameters I will leave this part as the last step to write    
        


    }
    
    Mat gray_frame;
    cvtColor(frame,gray_frame,COLOR_BGR2GRAY);
    imshow("video",gray_frame);
    imshow("background_subtraction",background_subtraction_result);

    if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
   {
    cout << "esc key is pressed by user" << endl;
    break;
   }
}


return 0;
}

//This function converts frame to 1D float vector
float_vec_t frame2vector(Mat frame,int frame_height, int frame_width)
{
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    resize(frame, frame, Size(frame_height, frame_width)); //For debugging
    frame.convertTo(frame, CV_32F);
    float_vec_t array((float*)frame.data,(float*)frame.data + frame.rows * frame.cols);
    return array;
}

//This function combines 4 different vectors into 1 vector that is the same size of the num of pixels, and each position has one mixture gaussian model
void vec4to1(vector<float_vec_t >* mixture_gaussian_mean, vector<float_vec_t >* mixture_gaussian_stddev, vector<float_vec_t >* mixture_gaussian_weights, vector<float_vec_t >* mixture_gaussian_W_S, vector<mixture_gaussian> * mixture_gaussian_model)
{    //Convert 3 vector<vector<float>> into vector<mixture_gaussian>
    int num_gaussian = (*mixture_gaussian_mean).size();
    int size = (*mixture_gaussian_mean)[0].size();
    for (int i=0; i<size; i++){
        for (int j=0; j<num_gaussian; j++){
            (*mixture_gaussian_model)[i].Mean.push_back((*mixture_gaussian_mean)[j][i]);
            (*mixture_gaussian_model)[i].Std.push_back((*mixture_gaussian_stddev)[j][i]);
            (*mixture_gaussian_model)[i].Weight.push_back((*mixture_gaussian_weights)[j][i]);
            (*mixture_gaussian_model)[i].W_S.push_back((*mixture_gaussian_W_S)[j][i]);
        }
    }
}
//This fuction returns the index of the first matching model
int matching_models(mixture_gaussian mixture_gaussian_model, float pixel_intensity)
{
    float std_scale = 2.5;
    int num_gaussian = mixture_gaussian_model.Mean.size();
    for(int i = 0; i < num_gaussian; i ++)
    {
        float mean_i = mixture_gaussian_model.Mean[i];
        float std_i = mixture_gaussian_model.Std[i];
        float difference = pixel_intensity-mean_i;
        if (difference >= -std_scale*std_i && difference <= std_scale*std_i)
            return i+1;
    }
    return 10;
}
//This function will sort the mixed gaussian model with respect to W/SD value
void sortGaussian(mixture_gaussian* gaussian_model)
{
    int num_gaussian = (*gaussian_model).Mean.size();

    vector<int> updated_index(num_gaussian);
    for (int i = 0; i <num_gaussian; i++){
        updated_index[i] = i;
    }
       
    float_vec_t W_by_SD_by_pixel;
    W_by_SD_by_pixel = (*gaussian_model).W_S;

    sort(updated_index.begin(), updated_index.end(), [&W_by_SD_by_pixel](int i1, int i2) {return W_by_SD_by_pixel[i1] > W_by_SD_by_pixel[i2];});

    mixture_gaussian current_mixture_gaussian(*gaussian_model);

    for (int i=0; i<num_gaussian; i++){
        (*gaussian_model).Mean[i] = current_mixture_gaussian.Mean[updated_index[i]];
        (*gaussian_model).Std[i] = current_mixture_gaussian.Std[updated_index[i]];
        (*gaussian_model).Weight[i] = current_mixture_gaussian.Weight[updated_index[i]];
        (*gaussian_model).W_S[i] = current_mixture_gaussian.W_S[updated_index[i]];
    }
}

//This function will return the index of the model when it is larger than the threshold
int backgroundGaussianProcess(mixture_gaussian* gaussian_model, float T)
{
    int num_gaussian = (*gaussian_model).Mean.size();
    float accumulate_weight = 0;

    for (int i=0; i<num_gaussian; i++){
        accumulate_weight += (*gaussian_model).Weight[i];
        if (accumulate_weight >= T){
            return i+1;
        }
    }
    return 10;
}