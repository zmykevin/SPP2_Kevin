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
#include <cmath>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace cv;
using namespace std;

#define NUM_GAUSS 5

typedef vector<float> float_vec_t;
struct mixture_gaussian
{
    float_vec_t Mean;
    float_vec_t Std;
    float_vec_t Weight;
    float_vec_t W_S;
};

// class mixture_gaussian_test {
// public:
//     vector<float> Mean = vector<float>(NUM_GAUSS);
//     vector<float> Std = vector<float>(NUM_GAUSS);
//     vector<float> Weight = vector<float>(NUM_GAUSS);
//     vector<float> W_S = vector<float>(NUM_GAUSS);

// };

vector<float> frame2vector(Mat , int, int);
void vec4to1(vector<vector<float> >*, vector<vector<float> >*, vector<vector<float> >*, vector<vector<float> >*, vector<mixture_gaussian> *);

void printMat(const Mat* image, int flag=0);
void printVector(const vector<float>* array, int wrapAround=0);
void printVectorInt(const vector<int>* array, int wrapAround=0);
void printVectorBool(const vector<bool>* array, int wrapAround=0);
void printAllVector(const vector<mixture_gaussian>* array, int flag, int wrapAround=0, int k=NUM_GAUSS);
void printAllVectorInt(const vector<vector<int>>* array, int wrapAround, int k=NUM_GAUSS);



float gaussProbability(float x, float mean, float stddev);
int background_process_heuristic(vector<mixture_gaussian>*, double);
int matching_gaussian(vector<float>*, vector<vector<float> >*, vector<vector<float> >*, vector<vector<float> >*);
void sortGaussian(mixture_gaussian* gaussian_model);
int backgroundGaussianProcess(mixture_gaussian* gaussian_model, float T);



int main(){

    cout << "Background Subtraction Start" << endl;

    // Define the number of gaussina models in the mixed model
    int K = 5;
    // Define the learning reate
    double alpha = 0.05;
    //Define the initialization_model_size
    int initial_sample_size = 40;

    // Start to capture the video
    VideoCapture cap("highwayI_raw.AVI");

    // Get the size dimension of the frame
    int frame_width = (int)cap.get(CAP_PROP_FRAME_WIDTH)/10;
    int frame_height = (int)cap.get(CAP_PROP_FRAME_HEIGHT)/10;

/////////////////////////// Initialzation

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
    
    cout << "initialization of mean is done" << endl;
    
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
        // cout << initial_mixture_gaussian_weight[i][0] << endl;
        // cout << initial_mixture_gaussian_std[i][0] << endl;
        divide(initial_mixture_gaussian_weight[i],initial_mixture_gaussian_std[i],initial_mixture_gaussian_WbySD[i]);
        // cout << initial_mixture_gaussian_WbySD[i][0] << endl;
    }
    cout << "initialization of std is done" << endl;
    

    // // Initialize the gaussian mixture
    // vector<vector<float> > mixture_gaussian_mean;
    // vector<vector<float> > mixture_gaussian_stddev;
    // vector<vector<float> > mixture_gaussian_weights;

    // int ret;
    // Mat frame;

    // for (int i=0; i<10; i++){
    //     cap >> frame;
    //     // initilize the mean values
    //     vector<float> gaussian_mean = frame2vector(frame, frame_height, frame_width);
    //     vector<float> gaussian_stddev(frame_height*frame_width, 0);
    //     // printVector(&gaussian_mean, frame_width);
    //     // initialize the standard deviation
    //     for (int j = 0; j < 3; j++){
    //         cap >> frame;
    //         vector<float> gaussian_i = frame2vector(frame, frame_height, frame_width);
    //         vector<float> temp_diff;
    //         absdiff(gaussian_i, gaussian_mean, temp_diff);
    //         // printVector(&temp_diff, frame_width);
    //         add(temp_diff, gaussian_stddev, gaussian_stddev);
    //         // printVector(&gaussian_stddev, frame_width);
    //     }
    //     // Get the average value for the standard deviation
    //     vector<float> ones(frame_height*frame_width, 3);
    //     divide(gaussian_stddev, ones, gaussian_stddev);
    //     // printVector(&gaussian_stddev, frame_width);

    //     // Initialize the weights
    //     vector<float> gaussian_weights(frame_height*frame_width, i_weight);

    //     //Append the ith gaussian model to the mixture
    //     mixture_gaussian_mean.push_back(gaussian_mean);
    //     mixture_gaussian_stddev.push_back(gaussian_stddev);
    //     mixture_gaussian_weights.push_back(gaussian_weights);
    //     // cout << "Gaussian_i vector size: "<<mixture_gaussian_mean.size()<<endl;
    // }

    // for (int i=0; i<K; i++){
    //     cout<<"vector"<<endl;
    //     printVector(&initial_mixture_gaussian_mean[i], frame_width);

    // }

    vector<mixture_gaussian> mixture_gaussian_model(frame_height*frame_width);
    // cout << "# "<<&mixture_gaussian_model[0]<<endl;
    // cout << "# "<<&mixture_gaussian_model[1]<<endl;
    vec4to1(&initial_mixture_gaussian_mean,&initial_mixture_gaussian_std, &initial_mixture_gaussian_weight, &initial_mixture_gaussian_WbySD, &mixture_gaussian_model);
    // printAllVector(&mixture_gaussian_model, 0, frame_width);
    // printAllVector(&mixture_gaussian_model, 1, frame_width);
    // printAllVector(&mixture_gaussian_model, 2, frame_width);
    // printAllVector(&mixture_gaussian_model, 3, frame_width);

    // vector<mixture_gaussian_test> mixture_try(frame_height*frame_width);
    // cout << "# "<<&mixture_try[0]<<endl;
    // cout << "# "<<&mixture_try[1]<<endl;

    // cout << "# "<<&mixture_gaussian_stddev[0]<<endl;
    // cout << "# "<<&mixture_gaussian_stddev[1]<<endl;
    // cout << "# "<<&mixture_gaussian_weights[0]<<endl;
    // cout << "# "<<&mixture_gaussian_weights[1]<<endl;
    // cout << "# "<<&mixture_gaussian_model[0]<<endl;
    // cout << "# "<<&mixture_gaussian_model[1]<<endl;




    //Initialize the probability history
    // probability_history = []
    //Initialize the background gaussian process
    vector<int> background_gaussian(frame_height*frame_width);
    for (int i=0; i<frame_height*frame_width; i++){
        sortGaussian(&mixture_gaussian_model[i]);
        background_gaussian[i] = backgroundGaussianProcess(&mixture_gaussian_model[i], 0.5);

    }

    float x = 1;
    float mean = 2;
    float stddev = 0.2;

    cout << gaussProbability(x, mean, stddev)<<endl;
    // printAllVector(&mixture_gaussian_model, 3, frame_width);
    // printVectorInt(&background_gaussian, frame_width);
    // int background_gaussian_process = background_process_heuristic(&mixture_gaussian_model, 0.5);
    // Define the video writer
    // define the codec and create Video Writer oject
    // int fourcc = VideoWriter::fourcc('X','V','I','D');
    // VideoWriter out("background_subtraction.avi",fourcc,30.0,Size(2*frame_width,frame_height));
    // while(cap.isOpened()){
    //     ret = cap.read(frame);
    //     if (ret){
    //         // imshow("display", frame);
    //         // waitKey(0);
    //         vector<float> gray_frame = frame2vector(frame, frame_height, frame_width);

    //         //////////////////////////////////////////////////////////////////// Not used
    //         //compute probability and store it in the current probability
    //         // current_probability = mixture_gaussian_probability(gray_frame,mixture_gaussian)
    //         // probability_history.append(current_probability)
    //         ///////////////////////////////////////////////////////////////////

    //         //evaluate the matching models
    //         int matching_models = matching_gaussian(&gray_frame,&mixture_gaussian_mean, &mixture_gaussian_stddev, &mixture_gaussian_weights);
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
        // }
    // }
}


// //Define a function to evaluate whether the pixel matches with one of the gaussian component
int matching_gaussian(vector<float>* frame, vector<vector<float> >* mixture_gaussian_mean, vector<vector<float> >* mixture_gaussian_stddev, vector<vector<float> >* mixture_gaussian_weights){
    float matching_scale = 2.5;
    int num_gaussian = (*mixture_gaussian_mean).size();

}
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


int background_process_heuristic(vector<mixture_gaussian>* mixture_gaussian_model, double T){
    //Return a matrix that includes the number of gaussians that matches the background procee
    //Return a updated order mixture_gaussian_model
    //Define the number of gaussia models
    int num_gaussian = (*mixture_gaussian_model)[0].Mean.size();

    // for i in range(0,num_gaussian):
    //     if i == 0:
    //         W_by_SD = np.divide(mixture_gaussian_model[i][:,:,2],mixture_gaussian_model[i][:,:,1]+10**(-8))
    //     else:
    //         current_W_by_SD = np.divide(mixture_gaussian_model[i][:,:,2],mixture_gaussian_model[i][:,:,1]+10**(-8))
    //         W_by_SD = np.dstack((W_by_SD,current_W_by_SD))
    // vector<vector<float> > W_by_SD;

    // for (int i=0; i<num_gaussian; i++){
    //     vector<float> current_W_by_SD;
    //     vector<float> adder;
    //     add(pow(10, -8), (*mixture_gaussian_stddev)[i], adder);
    //     divide((*mixture_gaussian_weights)[i], adder, current_W_by_SD);
    //     W_by_SD.push_back(current_W_by_SD);
    // }

    // updated_index = np.argsort(W_by_SD,axis = 2)
    int size = (*mixture_gaussian_model).size();
    vector<vector<int> > updated_index(size, vector<int>(num_gaussian));
    for (int i=0; i<size; i++){
        for (int j = 0; j <num_gaussian; j++){
            updated_index[i][j] = j;
        }
    }
    for (int i=0; i<size; i++){
        
        // vector<int> updated_index_by_pixel;
        vector<float> W_by_SD_by_pixel;
        // updated_index_by_pixel = updated_index[i];
        W_by_SD_by_pixel = (*mixture_gaussian_model)[i].W_S;

        // for (int j=0; j<num_gaussian; j++){
        //     updated_index_by_pixel.push_back(updated_index[i][j]);
        //     W_by_SD_by_pixel.push_back((*mixture_gaussian_model)[i].W_S[j]);
        // }
        // printVector(&W_by_SD_by_pixel);
        // printVectorInt(&updated_index_by_pixel);
        // Check the sorting order
        sort(updated_index[i].begin(), updated_index[i].end(), [&W_by_SD_by_pixel](int i1, int i2) {return W_by_SD_by_pixel[i1] > W_by_SD_by_pixel[i2];});
        // sort(updated_index_by_pixel.begin(), updated_index_by_pixel.end(), [&W_by_SD_by_pixel](int i1, int i2) {return W_by_SD_by_pixel[i1] > W_by_SD_by_pixel[i2];});
        // printVectorInt(&updated_index_by_pixel);
        // for (int j=0; j<num_gaussian; j++){
            // updated_index[i][j] = updated_index_by_pixel[j];
            // W_by_SD[j][i] = W_by_SD_by_pixel[updated_index_by_pixel[j]];
        // }
    }
    // for (int i=0; i<num_gaussian; i++){
    //     printVectorInt(&(updated_index[i]), 32);
    //     printVector(&(W_by_SD[i]), 32);
    // }

    // current_mixture_gaussian_model = deepcopy(mixture_gaussian_model)
    vector<mixture_gaussian> current_mixture_gaussian_model(*mixture_gaussian_model);
    // vector<vector<float> > current_mixture_gaussian_mean(*mixture_gaussian_mean); 
    // vector<vector<float> > current_mixture_gaussian_stddev(*mixture_gaussian_stddev); 
    // vector<vector<float> > current_mixture_gaussian_weights(*mixture_gaussian_weights); 
    // for i in range(0,num_gaussian):
    //     i_th_gaussian_index = updated_index[:,:,i]
    //     present_model = current_mixture_gaussian_model[i]
    //     updated_gaussian_model = np.zeros((present_model.shape[0],present_model.shape[1],3))
    //     for j in range(0,num_gaussian):
    //         mathing_model_j = np.where(i_th_gaussian_index == j,1,0)
    //         for k in range(0,3):
    //             updated_gaussian_model[:,:,k] = np.multiply(1-mathing_model_j,updated_gaussian_model[:,:,k])+np.multiply(mathing_model_j,current_mixture_gaussian_model[j][:,:,k])
    //     mixture_gaussian_model[4-i] = updated_gaussian_model
    // #mixture_gaussian_model = list(reversed(mixture_gaussian_model))
    for (int i=0; i<size; i++){
        for (int j=0; j<num_gaussian; j++){
            (*mixture_gaussian_model)[i].Mean[j] = current_mixture_gaussian_model[i].Mean[updated_index[i][j]];
            (*mixture_gaussian_model)[i].Std[j] = current_mixture_gaussian_model[i].Std[updated_index[i][j]];
            (*mixture_gaussian_model)[i].Weight[j] = current_mixture_gaussian_model[i].Weight[updated_index[i][j]];
            (*mixture_gaussian_model)[i].W_S[j] = current_mixture_gaussian_model[i].W_S[updated_index[i][j]];
            // (*mixture_gaussian_mean)[j][i] = current_mixture_gaussian_mean[updated_index[j][i]][i];
            // (*mixture_gaussian_stddev)[j][i] = current_mixture_gaussian_stddev[updated_index[j][i]][i];
            // (*mixture_gaussian_weights)[j][i] = current_mixture_gaussian_weights[updated_index[j][i]][i]; // unnecessary for first run?
        }
    }

    // for (int i=0; i<num_gaussian; i++){
    //     vector<float> printarray = (*mixture_gaussian_weights)[i];
    //     printVector(&printarray, 32);
    //     printVectorInt(&(updated_index[i]),32);
    //     printVector(&(current_mixture_gaussian_weights[i]),32);
    // }

    // printAllVector(mixture_gaussian_model, 2, 32);
    printAllVectorInt(&updated_index, 32);
        
    ///////////////////////////////////////////////// Old algorithm
    // for (int i=0; i<num_gaussian; i++){
    //     vector<int> i_th_gaussian_index = updated_index[i];
    //     vector<float> updated_gaussian_mean(present_gaussian_mean.size(),0);
    //     vector<float> updated_gaussian_stddev(present_gaussian_stddev.size(),0);
    //     vector<float> updated_gaussian_weights(present_gaussian_weights.size(),0);
    //     for (int j=0; j<num_gaussian; j++){
    //         vector<bool> masking_model(present_gaussian_weights.size(),0);
    //         vector<bool> masking_model_inverse(present_gaussian_weights.size(),1);
    //         for (int k=0; k<present_gaussian_weights.size(); k++){
    //             if (i_th_gaussian_index[k] == j){
    //                 masking_model[k] = 1;
    //                 masking_model_inverse = 0;
    //             }
    //         }
    //         add(updated_gaussian_mean)//Unfished, use other method.
    //     }
    // }
    /////////////////////////////////////////////////////////////


    // #The next step is to figure out how many num of gaussians for each background process
    // matching_history = []
    // vector<vector<bool>> matching_history;
    // // background_process_history = []
    // // for i in range(0,num_gaussian):
    // vector<float> accumulate_weight(size, 0);
    // // cout<< accumulate_weight.size()<<endl;
    // for (int j=0; j<num_gaussian; j++){
    //     vector<bool> matching_condition_accumulate(size,0);
    //     for (int i=0; i<size; i++){
    //         // cout<<i<<endl;
    //         accumulate_weight[i] = accumulate_weight[i] + (*mixture_gaussian_weights)[j][i];
    //         matching_condition_accumulate[i] = (accumulate_weight[i] >= T)? 1:0;
    //     }
        // printVectorBool(&matching_condition_accumulate,32);

    // }
    //     if i == 0:
    //         accumulate_weight = deepcopy(mixture_gaussian_model[i][:,:,2])
    //     else:
    //         accumulate_weight += mixture_gaussian_model[i][:,:,2]
    //     matching_condition_accumulate = np.where(accumulate_weight >= T,1,0)
    //     matching_condition_current = np.where(matching_condition_accumulate-sum(matching_history)>0,1,0)
    //     matching_history.append(matching_condition_current)
    //     background_process_history.append((i+1)*matching_condition_current)
    // background_process = sum(background_process_history)
    // return background_process
    return 1;
}


void printMat(const Mat* image, int flag){
    cout<<"image: "<<endl;
    for (int i=0; i<(*image).cols; i++){
        for (int j=0; j<(*image).rows; j++){
            if (flag == 0){
                cout << setw(4) <<(int)(*image).at<uchar>(i,j);
            }
            else if (flag == 1){
                cout << setw(4) << (*image).at<float>(i,j);
            }
        }
        cout << endl;
    }
}

void printVector(const vector<float>* array, int wrapAround){
    cout<<"Vector:"<<endl;
    if (wrapAround!=0){
        int cols = (*array).size()/wrapAround;
        // int rows = (*array).size()%wrapAround;
        for (int i=0; i<cols; i++){
            for (int j=0; j<wrapAround; j++){
                cout<< setw(4)<< (*array).at(i*wrapAround+j);
            }
            cout<<endl;
        }
    }
    else{
        int num = (*array).size();
        for (int i=0; i<num; i++){
            cout<< (*array)[i]<<endl;
        }
    }
}

void printAllVector(const vector<mixture_gaussian>* array, int flag, int wrapAround, int k){
    for (int iter=0; iter<k; iter++){
        cout<<"Vector:"<<iter<<endl;
        if (flag == 0){
            if (wrapAround!=0){
                int cols = (*array).size()/wrapAround;
                // int rows = (*array).size()%wrapAround;
                for (int i=0; i<cols; i++){
                    for (int j=0; j<wrapAround; j++){
                        cout<< setw(4)<< (*array).at(i*wrapAround+j).Mean[iter];
                    }
                    cout<<endl;
                }
            }
            else{
                int num = (*array).size();
                for (int i=0; i<num; i++){
                    cout<< (*array)[i].Mean[iter]<<endl;
                }
            }        
        }
        else if (flag == 1){
            if (wrapAround!=0){
                int cols = (*array).size()/wrapAround;
                // int rows = (*array).size()%wrapAround;
                for (int i=0; i<cols; i++){
                    for (int j=0; j<wrapAround; j++){
                        cout<< setw(4)<< (*array).at(i*wrapAround+j).Std[iter];
                    }
                    cout<<endl;
                }
            }
            else{
                int num = (*array).size();
                for (int i=0; i<num; i++){
                    cout<< (*array)[i].Std[iter]<<endl;
                }
            }        
        }
        else if (flag == 2){
            if (wrapAround!=0){
                int cols = (*array).size()/wrapAround;
                // int rows = (*array).size()%wrapAround;
                for (int i=0; i<cols; i++){
                    for (int j=0; j<wrapAround; j++){
                        cout<< setw(4)<< (*array).at(i*wrapAround+j).Weight[iter];
                    }
                    cout<<endl;
                }
            }
            else{
                int num = (*array).size();
                for (int i=0; i<num; i++){
                    cout<< (*array)[i].Weight[iter]<<endl;
                }
            }        
        }
        else if (flag == 3){
            if (wrapAround!=0){
                int cols = (*array).size()/wrapAround;
                // int rows = (*array).size()%wrapAround;
                for (int i=0; i<cols; i++){
                    for (int j=0; j<wrapAround; j++){
                        cout<< setw(4)<< (*array).at(i*wrapAround+j).W_S[iter];
                    }
                    cout<<endl;
                }
            }
            else{
                int num = (*array).size();
                for (int i=0; i<num; i++){
                    cout<< (*array)[i].W_S[iter]<<endl;
                }
            }        
        }
    }
}

void printAllVectorInt(const vector<vector<int>>* array, int wrapAround, int k){
    for (int iter=0; iter<k; iter++){
        cout<<"Vector:"<<iter<<endl;
        if (wrapAround!=0){
            int cols = (*array).size()/wrapAround;
            // int rows = (*array)[0].size()%wrapAround;
            for (int i=0; i<cols; i++){
                for (int j=0; j<wrapAround; j++){
                    cout<< setw(4)<< (*array)[i*wrapAround+j].at(iter);
                }
                cout<<endl;
            }
        }
        else{
            int num = (*array).size();
            for (int i=0; i<num; i++){
                cout<< (*array)[i][iter]<<endl;
            }
        }        
    }
}

void printVectorInt(const vector<int>* array, int wrapAround){
    cout<<"Vector:"<<endl;
    if (wrapAround!=0){
        int cols = (*array).size()/wrapAround;
        // int rows = (*array).size()%wrapAround;
        for (int i=0; i<cols; i++){
            for (int j=0; j<wrapAround; j++){
                cout<< setw(4)<< (*array).at(i*wrapAround+j);
            }
            cout<<endl;
        }
    }
    else{
        int num = (*array).size();
        for (int i=0; i<num; i++){
            cout<< (*array)[i]<<endl;
        }
    }
}

void printVectorBool(const vector<bool>* array, int wrapAround){
    cout<<"Vector:"<<endl;
    if (wrapAround!=0){
        int cols = (*array).size()/wrapAround;
        // int rows = (*array).size()%wrapAround;
        for (int i=0; i<cols; i++){
            for (int j=0; j<wrapAround; j++){
                cout<< setw(4)<< (*array).at(i*wrapAround+j);
            }
            cout<<endl;
        }
    }
    else{
        int num = (*array).size();
        for (int i=0; i<num; i++){
            cout<< (*array)[i]<<endl;
        }
    }
}

vector<float> frame2vector(Mat frame,int frame_height, int frame_width){
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    resize(frame, frame, Size(frame_height, frame_width)); //For debugging
    frame.convertTo(frame, CV_32F);
    vector<float> array((float*)frame.data,(float*)frame.data + frame.rows * frame.cols);
    return array;
}

void vec4to1(vector<vector<float> >* mixture_gaussian_mean, vector<vector<float> >* mixture_gaussian_stddev, vector<vector<float> >* mixture_gaussian_weights, vector<vector<float> >* mixture_gaussian_W_S, vector<mixture_gaussian> * mixture_gaussian_model){
    //Convert 3 vector<vector<float>> into vector<mixture_gaussian>
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

void sortGaussian(mixture_gaussian* gaussian_model){
    int num_gaussian = (*gaussian_model).Mean.size();

    vector<int> updated_index(num_gaussian);
    for (int i = 0; i <num_gaussian; i++){
        updated_index[i] = i;
    }
       
    vector<float> W_by_SD_by_pixel;
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

int backgroundGaussianProcess(mixture_gaussian* gaussian_model, float T){
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

float gaussProbability(float x, float mean, float stddev){
    return exp(-(x-mean)*(x-mean)/(2*stddev*stddev))/stddev/sqrt(2*M_PI);
}