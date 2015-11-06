#include "opencv2/core.hpp"
// #include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
// #include "opencv2/core/core_c.h"
// #include "opencv2/core/core.hpp"
// #include "opencv2/flann/miniflann.hpp"
// #include "opencv2/imgproc/imgproc_c.h"
// #include "opencv2/imgproc/imgproc.hpp"
// #include "opencv2/video/video.hpp"
// #include "opencv2/features2d/features2d.hpp"
// #include "opencv2/objdetect/objdetect.hpp"
// #include "opencv2/calib3d/calib3d.hpp"
// #include "opencv2/ml/ml.hpp"
// #include "opencv2/highgui/highgui_c.h"
// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/contrib/contrib.hpp"

#include <iostream>
// #include <cmath>
#include <vector>
#include <algorithm>

#define updated_index W_by_SD

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

    // cout<<frame_width<<endl;
    // cout<<frame_height<<endl;
    
    // Initialize the gaussian mixture
    vector<Gaussian_i> mixture_gaussian;
    int ret;
    Mat frame;

    for (int i=0; i<10; i++){
        cap >> frame;
        Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
        // cout << gaussian_i.channels() <<endl;
        // initilize the mean values
        cvtColor(frame, gaussian_i.mean, COLOR_BGR2GRAY);
        // imshow("dDisplay",gaussian_i.mean);
        // waitKey(0);
        // cout << "iteration "<< i <<endl;
        // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
        //initialize the standard deviation
        for (int j = 0; j < 3; j++){
            cap >> frame;
            Mat temp_gray, temp_diff, temp_diff_d;
            cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
            absdiff(temp_gray, gaussian_i.mean, temp_diff);
            // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
            temp_diff = abs(temp_gray - gaussian_i.mean);
            temp_diff.convertTo(temp_diff_d, CV_64F);
            // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
            // cout<<temp_diff_d.at<double>(1,1)<<endl;
            gaussian_i.stddev = temp_diff_d + gaussian_i.stddev;
            // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
        }
        // Get the average value for the standard deviation
        gaussian_i.stddev = gaussian_i.stddev/3;
        // cout<<"mean_dev: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
        // Initialize the weights
        gaussian_i.weights = Mat(frame_height, frame_width, CV_64F, i_weight);

        //Append the ith gaussian model to the mixture
        mixture_gaussian.push_back(gaussian_i);
        // cout << "Gaussian_i vector size: "<<mixture_gaussian.size()<<endl;
    }

    cout << "# "<<&mixture_gaussian[0]<<endl;
    cout << "# "<<&mixture_gaussian[1]<<endl;


    //Initialize the probability history
    // probability_history = []
    //Initialize the background gaussian process
    int 
    background_gaussian_process = background_process_heuristic(&mixture_gaussian,0.5);
    // Define the video writer
    // define the codec and create Video Writer oject
    // int fourcc = fourcc('X','V','I','D');
    // out = VideoWriter("background_subtraction.avi",fourcc,30.0,(2*frame_width,frame_height));
    // while(cap.isOpened()){
    //     ret = cap.read(frame);
    //     if (ret){
    //         Mat gray_frame;
    //         cvtColor(frame, gray_frame, COLOR_BGR2GRAY);


            // //compute probability and store it in the current probability
            // current_probability = mixture_gaussian_probability(gray_frame,mixture_gaussian)
            // probability_history.append(current_probability)

            // //evaluate the matching models
            // matching_models = matching_gaussian(gray_frame,mixture_gaussian)
            // //Compute matching_model_matrix
            // matching_model_matrix = np.zeros((gray_frame.shape[0],gray_frame.shape[1]))
            // for i in range(0,len(matching_models)):
            //     matching_model_matrix += (i+1)*matching_models[i]
            // matching_model_matrix = np.where(matching_model_matrix == 0, 10,matching_model_matrix)

            // //Determine which are background and which are foreground
            // subtraction_mask = np.where(matching_model_matrix > background_gaussian_process,255,0)
            // subtraction_mask = subtraction_mask.astype(dtype = 'uint8')

            // //update the gaussian parameters
            // update_mixture_gaussian(gray_frame,matching_models,mixture_gaussian,alpha)

            // //background heuristic
            // background_gaussian_process = background_process_heuristic(mixture_gaussian,0.5)

            // //subtraction_mask = cv2.medianBlur(subtraction_mask,5)
            // image = np.hstack((gray_frame,subtraction_mask))

            // //Write out the image
            // out.write(image)
            // cv2.imshow('frame',image)
            // if cv2.waitKey(1) == 27:
            //     break
        // }
        // else{
        //     break;
        // }
            
    }



// //Define a function to evaluate whether the pixel matches with one of the gaussian component
// def matching_gaussian(X,mixture_gaussian_model):
//     matching_scale = 2.5
//     num_gaussian = len(mixture_gaussian)
//     matching_models = []
//     for i in range(0,num_gaussian):
//         current_gaussian = mixture_gaussian_model[i]
//         current_mean = current_gaussian[:,:,0]
//         current_SD = current_gaussian[:,:,1]+10**(-8)
//         difference = np.abs(X-current_mean)-matching_scale*current_SD

//         //This process assure the first matching models will be kept
//         if i == 0:
//             matching_models_matrix = np.where(difference <= 0,1,0)
//         else:
//             matching_models_matrix = np.where(difference <= 0,1,0)
//             matching_models_history = sum(matching_models)
//             repetition_check = matching_models_matrix-matching_models_history
//             matching_models_matrix = np.where(repetition_check == 1,1,0)
//         matching_models.append(matching_models_matrix)
//     return matching_models


// //Define function to compute probability
// def mixture_gaussian_probability(X,mixture_gaussian_model):
//     //initialize mixture gaussian probability
//     MGP = np.zeros((X.shape[0],X.shape[1]))
//     num_gaussian = len(mixture_gaussian_model)
//     for i in range (0,num_gaussian):
//         current_gaussian = mixture_gaussian_model[i]
//         current_mean = current_gaussian[:,:,0]
//         current_SD = current_gaussian[:,:,1]+10**(-8)
//         current_weight = current_gaussian[:,:,2]
//         inside_exponential = -0.5 *np.divide(np.square(X-current_mean),np.square(current_SD))//should return a matrix same as frame size
//         GPD = np.divide(np.exp(inside_exponential),math.sqrt(2*math.pi)*current_SD)
//         GPD = np.where(GPD > 1,0,GPD)
//         MGP += np.multiply(current_weight,GPD)
//     return MGP

// //Define the function to update the mixture gaussian
// def update_mixture_gaussian(X,matching_models,mixture_gaussian_model,alpha):
//     //Define the low_weight
//     low_weight = 0

//     //This evaluates whether it is matching or non-matching gaussian matching
//     K_Gaussian_matching = sum(matching_models)

//     //define the total number of gaussian models in the mixture
//     num_gaussian = len(mixture_gaussian_model)

//     //Deal with the non_matching K Gaussion
//     Non_Matching_GPD_Mixture = np.zeros([X.shape[0],X.shape[1],num_gaussian])
//     for i in range(0,num_gaussian):
//         current_gaussian = mixture_gaussian_model[i]
//         current_mean = current_gaussian[:,:,0]
//         current_SD = current_gaussian[:,:,1]+10**(-8)

//         //compute the probability
//         inside_exponential = -0.5 *np.divide(np.square(X-current_mean),np.square(current_SD))//should return a matrix same as frame size
//         GPD = np.multiply(np.true_divide((1/math.sqrt(2*math.pi)),current_SD),np.exp(inside_exponential))
//         Non_Matching_GPD_Mixture[:,:,i] = np.multiply(1-K_Gaussian_matching,GPD)

//     //Min_Potential_Distribution = np.argmin(Non_Matching_GPD_Mixture,2) + (1-K_Gaussian_matching)
//     Min_Potential_Distribution = np.argmin(Non_Matching_GPD_Mixture,2)
//     old_mixture_gaussian_model = deepcopy(mixture_gaussian_model)
//     for i in range(0,num_gaussian):
//         Min_i = np.where(Min_Potential_Distribution == i+1,1,0)
//         //Extract the current gaussian model
//         current_gaussian = old_mixture_gaussian_model[i]
//         current_mean = current_gaussian[:,:,0]
//         current_SD = current_gaussian[:,:,1]+10**(-8)
//         current_weight = current_gaussian[:,:,2]

//         //Update mean as the current X
//         mixture_gaussian_model[i][:,:,0] = np.multiply(current_mean,(1-Min_i)) + np.multiply(X,Min_i)
//         //update SD to be high
//         //mixture_gaussian_model[i][:,:,1] = np.multiply(current_SD,(1-Min_i))+np.multiply(np.abs(255-mixture_gaussian_model[i][:,:,0]),Min_i)
//         mixture_gaussian_model[i][:,:,1] = np.multiply(current_SD,(1-Min_i))+np.multiply(255,Min_i)
//         //update weight to be low, set it to 0.01
//         mixture_gaussian_model[i][:,:,2] = np.multiply(current_weight,(1-Min_i)) + Min_i*low_weight
//         //weight_distribution = (np.multiply(current_weight,Min_i)-Min_i*low_weight)/(num_gaussian-1)
//         weight_distribution = (np.multiply(current_weight,Min_i)-Min_i*low_weight)/(num_gaussian-1)
//         for j in range(0,num_gaussian):
//             if j != i:
//                 mixture_gaussian_model[j][:,:,2] += weight_distribution

//     //Deal with the matching K Gaussion
//     old_mixture_gaussian_model = deepcopy(mixture_gaussian_model)
//     for i in range(0,num_gaussian):
//         current_gaussian = old_mixture_gaussian_model[i]
//         current_mean = current_gaussian[:,:,0]
//         current_SD = current_gaussian[:,:,1]+10**(-8)
//         current_weight = current_gaussian[:,:,2]

//         matching_gaussian_i = matching_models[i]

//         //Define the gaussian distribution probability
//         inside_exponential = -0.5 *np.divide(np.square(X-current_mean),np.square(current_SD))//should return a matrix same as frame size
//         GPD = np.multiply(np.true_divide((1/math.sqrt(2*math.pi)),current_SD),np.exp(inside_exponential))
//         GPD = np.where(GPD > 1,0,GPD)
//         p = alpha*GPD

//         //update_weight for matching k_models
//         matching_k_models_weight = np.multiply(K_Gaussian_matching,current_weight)
//         mixture_gaussian_model[i][:,:,2] = np.multiply(1-K_Gaussian_matching,current_weight)+(1-alpha)*matching_k_models_weight+alpha*matching_gaussian_i

//         //update mean for matching k_models
//         mixture_gaussian_model[i][:,:,0] = np.multiply(1-matching_gaussian_i,current_mean) + np.multiply(1-p,np.multiply(matching_gaussian_i,current_mean)) + np.multiply(p,np.multiply(matching_gaussian_i,X))

//         //update SD
//         updated_variance_matching_model = np.multiply((1-p),np.multiply(matching_gaussian_i,np.square(current_SD))) + np.multiply(p,np.multiply(matching_gaussian_i,np.square(X-mixture_gaussian_model[i][:,:,0])))
//         mixture_gaussian_model[i][:,:,1] = np.multiply(1-matching_gaussian_i,current_SD) + np.sqrt(updated_variance_matching_model)


int background_process_heuristic(vector<Gaussian_i>* mixture_gaussian_model, double T){
    //Return a matrix that includes the number of gaussians that matches the background procee
    //Return a updated order mixture_gaussian_model
    //Define the number of gaussia models
    int num_gaussian = mixture_gaussian_model->size();
    int cols = 320;
    int rows = 240;
    for (int i = 0; i < cols; i++){
        for (int j = 0; j < rows; j++){
            for (int k = 0; k < num_gaussian; k++){
                // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
                // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
                cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].weights.at<double>(i,j)<<endl;
            }
        }
    }


    vector<Mat> W_by_SD;

    for (int i=0; i<num_gaussian; i++){
        W_by_SD.push_back((*mixture_gaussian_model)[i].weights / ((*mixture_gaussian_model)[i].stddev+pow(10, -8)));
        // cout<<W_by_SD.size()<<endl;
        // cout<<W_by_SD[i].at<double>(1,1)<<endl;
    }

    // seems ugly
    // int cols = W_by_SD[0].cols;
    // int rows = W_by_SD[0].rows;
    // cout<<cols<<endl;
    // cout<<rows<<endl;
    // Mat W_by_SD_t = Mat(cols*rows, num_gaussian, CV_64F);
    // for (int i = 0; i < cols*rows; i++){
    //     for (int j = 0; j < num_gaussian; j++){
    //         cout<<"i/rows,i%%rows is :"<<i/rows<<" "<<i%rows<<endl;
    //         cout<< W_by_SD[j].at<double>(i/rows,i%rows)<<endl;
    //     }
    // }
    // sortIdx(W_by_SD_t, W_by_SD_t, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
    // Is this faster?? memory allocation?
    // for (int i = 0; i < cols; i++){
    //     for (int j = 0; j < rows; j++){
    //         for (int k = 0; k < num_gaussian; k++){
    //             W_by_SD[k].at<double>(i,j) = W_by_SD_t.at<double>(rows*i+j,k);
    //         }
    //     }
    // }

    // updated_index = np.argsort(W_by_SD,axis = 2)
    for (int i = 0; i < cols; i++){
        for (int j = 0; j < rows; j++){
            vector<double> sorted_array;
            for (int k = 0; k < num_gaussian; k++){
                // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << W_by_SD[k].at<double>(i,j)<<endl;
                // sorted_array.push_back(W_by_SD[k].at<double>(i,j));
            }
            // sort(sorted_array.begin(), sorted_array.end());
            // for (int k = 0; k < num_gaussian; k++){
            //     W_by_SD[k].at<double>(i,j) = sorted_array[k];
            // }
        }
    }



    // current_mixture_gaussian_model = deepcopy(mixture_gaussian_model)
    // vector<Gaussian_i> current_mixture_gaussian_model;
    // for (int i = 0; i< num_gaussian; i++){
    //     cout<<"iter: "<<i<<endl;
    //     Gaussian_i temp_mixture_gaussian_model(cols, rows);
    //     temp_mixture_gaussian_model.mean = mixture_gaussian_model[i].mean.clone();
    //     temp_mixture_gaussian_model.stddev = mixture_gaussian_model[i].stddev.clone();
    //     temp_mixture_gaussian_model.weights = mixture_gaussian_model[i].weights.clone();
    //     current_mixture_gaussian_model.push_back(temp_mixture_gaussian_model);
    // }

    // for i in range(0,num_gaussian):
    //     i_th_gaussian_index = updated_index[:,:,i]
    //     present_model = current_mixture_gaussian_model[i]
    //     updated_gaussian_model = np.zeros((present_model.shape[0],present_model.shape[1],3))
    //     for j in range(0,num_gaussian):
    //         mathing_model_j = np.where(i_th_gaussian_index == j,1,0)
    //         for k in range(0,3):
    //             updated_gaussian_model[:,:,k] = np.multiply(1-mathing_model_j,updated_gaussian_model[:,:,k])+np.multiply(mathing_model_j,current_mixture_gaussian_model[j][:,:,k])
    //     mixture_gaussian_model[4-i] = updated_gaussian_model
    // //mixture_gaussian_model = list(reversed(mixture_gaussian_model))
    // //The next step is to figure out how many num of gaussians for each background process
    // matching_history = []
    // background_process_history = []
    // for i in range(0,num_gaussian):
    //     if i == 0:
    //         accumulate_weight = deepcopy(mixture_gaussian_model[i][:,:,2])
    //     else:
    //         accumulate_weight += mixture_gaussian_model[i][:,:,2]
    //     matching_condition_accumulate = np.where(accumulate_weight > T,1,0)
    //     matching_condition_current = np.where(matching_condition_accumulate-sum(matching_history)>0,1,0)
    //     matching_history.append(matching_condition_current)
    //     background_process_history.append((i+1)*matching_condition_current)
    // background_process = sum(background_process_history)
    // return background_process
    return 1;
}