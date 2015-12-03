#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
using namespace cv;
using namespace std;

#define PI 3.1415926

class Ball {
    int RADIUS;
    static const int NUM_POINTS=20;
    double THRESH;
    double speed;
    double dangle;
    double Circle_x[NUM_POINTS];
    double Circle_y[NUM_POINTS];
    
    Point center;
    double theta;
    double l_theta;
    double ll_theta;
    int mode;

public:
    Ball(int init_width, int init_height, int init_radius=5, double init_speed=5, double init_theta=0.5236);
    // Note that the radius should better be no smaller than speed/2.
    Mat updateBall(Mat image_frame, Mat subtraction);
};

//TODO: it will stuck when it will always go into obstacle.
//When it is in obstacle, it will not move