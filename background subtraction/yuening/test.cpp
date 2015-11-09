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
#include <iomanip>


using namespace cv;
using namespace std;

void printMat(const Mat* image, int flag=0);
void printVector(const vector<float>* array, int wrapAround=0);

int main(){
    Mat image;
    image = imread("pic1_cropped.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imshow("show", image);
    Mat resized_image;
    resize(image,resized_image,Size(10,10));
    // printMat(&resized_image,0);
    resized_image.convertTo(resized_image, CV_32F);
    // printMat(&resized_image,1);
    vector<float> array((float*)resized_image.data,(float*)resized_image.data + resized_image.rows * resized_image.cols);
    printVector(&array, 10);
    // imshow("show_resized", resized_image);
    // waitKey(0);

    vector<float> onesarray(array.size(),2);
    vector<float> result;
    vector<bool> mask(array.size(),1);
    mask[0] = 0;
    add(array, onesarray, result, mask);
    printVector(&result, 10);
    return 0;   
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
        int rows = (*array).size()%wrapAround;
        for (int i=0; i<cols; i++){
            for (int j=0; j<cols; j++){
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