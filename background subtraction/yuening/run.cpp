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

// #define updated_index W_by_SD

using namespace cv;
using namespace std;


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
    int ret;
    Mat frame;

///////////////////////////////////////////////////////////// 0
    cap >> frame;
    // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
    Mat gaussian_mean_0(frame_height, frame_width, CV_8UC1, Scalar(0));
    Mat gaussian_stddev_0(frame_height, frame_width, CV_64FC1, Scalar(0));
    Mat gaussian_weights_0(frame_height, frame_width, CV_64FC1, i_weight);
    // cout << gaussian_i.channels() <<endl;
    // initilize the mean values
    cvtColor(frame, gaussian_mean_0, COLOR_BGR2GRAY);
    // imshow("dDisplay",gaussian_i.mean);
    // waitKey(0);
    // cout << "iteration "<< i <<endl;
    // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
    //initialize the standard deviation
    for (int k = 0; k < 3; k++){
        cap >> frame;
        Mat temp_gray, temp_diff, temp_diff_d;
        cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
        imshow("Display", temp_gray);
        // temp_diff = temp_gray - gaussian_mean_0;
        // abs(temp_diff, temp_diff);
        // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
        absdiff(temp_gray, gaussian_mean_0, temp_diff);
        for (int i = 0; i < 320; i++){
        	for (int j = 0; j < 240; j++){
        		// cout<< "k = "<<k<< " at " << i <<"," << j <<"is " << (int)temp_diff.at<uchar>(i,j)<<endl;
			}
		}
        temp_diff.convertTo(temp_diff_d, CV_64F);
        for (int i = 0; i < 320; i++){
        	for (int j = 0; j < 240; j++){
        		cout<< "k = "<<k<< " at " << i <<"," << j <<"is " << temp_diff_d.at<double>(i,j)<<endl;
			}
		}
        // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
        // cout<<temp_diff_d.at<double>(1,1)<<endl;
        gaussian_stddev_0 = temp_diff_d + gaussian_stddev_0;
        // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
    }
    // Get the average value for the standard deviation
    gaussian_stddev_0 = gaussian_stddev_0/3;
// ///////////////////////////////////////////////////////////////////////1
//     cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_1(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_1(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_1(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_1, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_1, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_1);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_1 = temp_diff_d + gaussian_stddev_1;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_1 = gaussian_stddev_1/3;

// /////////////////////////////////////////////////////////////////2
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_2(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_2(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_2(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_2, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_2, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_2);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_2 = temp_diff_d + gaussian_stddev_2;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_2 = gaussian_stddev_2/3;

//     /////////////////////////////////////////////////////////////////3
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_3(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_3(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_3(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_3, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_3, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_3);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_3 = temp_diff_d + gaussian_stddev_3;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_3 = gaussian_stddev_3/3;

//     /////////////////////////////////////////////////////////////////4
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//4 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_4(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_4(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_4(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_4, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(4);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_4, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_4);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_4 = temp_diff_d + gaussian_stddev_4;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_4 = gaussian_stddev_4/3;

//     /////////////////////////////////////////////////////////////////5
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_5(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_5(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_5(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_5, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_5, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_5);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_5 = temp_diff_d + gaussian_stddev_5;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_5 = gaussian_stddev_5/3;

//     /////////////////////////////////////////////////////////////////6
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_6(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_6(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_6(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_6, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_6, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_6);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_6 = temp_diff_d + gaussian_stddev_6;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_6 = gaussian_stddev_6/3;

//     /////////////////////////////////////////////////////////////////7
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_7(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_7(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_7(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_7, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_7, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_7);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_7 = temp_diff_d + gaussian_stddev_7;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_7 = gaussian_stddev_7/3;

//     /////////////////////////////////////////////////////////////////8
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_8(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_8(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_8(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_8, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_8, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_8);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_8 = temp_diff_d + gaussian_stddev_8;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_8 = gaussian_stddev_8/3;

//     /////////////////////////////////////////////////////////////////9
// 	cap >> frame;
//     // Gaussian_i gaussian_i(frame_height, frame_width);//0 = Mean, 1 = Standard Deviation, and 2 = Prior Weight
//     Mat gaussian_mean_9(frame_height, frame_width, CV_8UC1, Scalar(0));
//     Mat gaussian_stddev_9(frame_height, frame_width, CV_64FC1, Scalar(0));
//     Mat gaussian_weights_9(frame_height*2, frame_width*2, CV_64FC1, i_weight);
//     // cout << gaussian_i.channels() <<endl;
//     // initilize the mean values
//     cvtColor(frame, gaussian_mean_9, COLOR_BGR2GRAY);
//     // imshow("dDisplay",gaussian_i.mean);
//     // waitKey(0);
//     // cout << "iteration "<< i <<endl;
//     // cout<<"mean: "<<(int)gaussian_i.mean.at<uchar>(1,1)<<endl;
//     //initialize the standard deviation
//     for (int j = 0; j < 3; j++){
//         cap >> frame;
//         Mat temp_gray, temp_diff, temp_diff_d;
//         cvtColor(frame, temp_gray, COLOR_BGR2GRAY);
//         absdiff(temp_gray, gaussian_mean_9, temp_diff);
//         // cout<<(int)temp_gray.at<uchar>(1,1)<<endl;
//         temp_diff = abs(temp_gray - gaussian_mean_9);
//         temp_diff.convertTo(temp_diff_d, CV_64F);
//         // cout<<(int)temp_diff.at<uchar>(1,1)<<endl;
//         // cout<<temp_diff_d.at<double>(1,1)<<endl;
//         gaussian_stddev_9 = temp_diff_d + gaussian_stddev_9;
//         // cout<<"accu: "<<gaussian_i.stddev.at<double>(1,1)<<endl;
//     }
//     // Get the average value for the standard deviation
//     gaussian_stddev_9 = gaussian_stddev_9/3;

    // waitKey(0);

    for (int i = 0; i < 320; i++){
        for (int j = 0; j < 240; j++){
            // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
            // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
         	// cout<< "k = 0"<< " at " << i <<"," << j <<"is " << gaussian_stddev_0.at<double>(i,j)<<endl;
         	// cout<< "k = 0"<< " at " << i <<"," << j <<"is " << gaussian_weights_0.at<double>(i,j)<<endl;
        }
    }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 1"<< " at " << i <<"," << j <<"is " << gaussian_stddev_1.at<double>(i,j)<<endl;
    //      	cout<< "k = 1"<< " at " << i <<"," << j <<"is " << gaussian_weights_1.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 2"<< " at " << i <<"," << j <<"is " << gaussian_stddev_2.at<double>(i,j)<<endl;
    //      	cout<< "k = 2"<< " at " << i <<"," << j <<"is " << gaussian_weights_2.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 3"<< " at " << i <<"," << j <<"is " << gaussian_stddev_3.at<double>(i,j)<<endl;
    //      	cout<< "k = 3"<< " at " << i <<"," << j <<"is " << gaussian_weights_3.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 4"<< " at " << i <<"," << j <<"is " << gaussian_stddev_4.at<double>(i,j)<<endl;
    //      	cout<< "k = 4"<< " at " << i <<"," << j <<"is " << gaussian_weights_4.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 5"<< " at " << i <<"," << j <<"is " << gaussian_stddev_5.at<double>(i,j)<<endl;
    //      	cout<< "k = 5"<< " at " << i <<"," << j <<"is " << gaussian_weights_5.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 6"<< " at " << i <<"," << j <<"is " << gaussian_stddev_6.at<double>(i,j)<<endl;
    //      	cout<< "k = 6"<< " at " << i <<"," << j <<"is " << gaussian_weights_6.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 7"<< " at " << i <<"," << j <<"is " << gaussian_stddev_7.at<double>(i,j)<<endl;
    //      	cout<< "k = 7"<< " at " << i <<"," << j <<"is " << gaussian_weights_7.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 8"<< " at " << i <<"," << j <<"is " << gaussian_stddev_8.at<double>(i,j)<<endl;
    //      	cout<< "k = 8"<< " at " << i <<"," << j <<"is " << gaussian_weights_8.at<double>(i,j)<<endl;
    //     }
    // }
    // for (int i = 0; i < 320; i++){
    //     for (int j = 0; j < 240; j++){
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (int)(*mixture_gaussian_model)[k].mean.at<uchar>(i,j)<<endl;
    //         // cout<< "k = "<< k<< " at " << i <<"," << j <<"is " << (*mixture_gaussian_model)[k].stddev.at<double>(i,j)<<endl;
    //      	cout<< "k = 9"<< " at " << i <<"," << j <<"is " << gaussian_stddev_9.at<double>(i,j)<<endl;
    //      	cout<< "k = 9"<< " at " << i <<"," << j <<"is " << gaussian_weights_9.at<double>(i,j)<<endl;
    //     }
    // }

}
