// #include "opencv2/core.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/highgui.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/opencv.hpp"
// #include "opencv2/core/core_c.h"
// #include "opencv2/core/core.hpp"

// using namespace cv;
// using namespace std;


#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc_c.h"

#include <iostream>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

#define PI 3.1415926

int main()
{
    char wndname[] = "Drawing Demo";
    const int DELAY = 3;
    const int RADIUS = 20;
    const int NUM_POINTS = 20;
    const double THRESH = 5;
    int width = 1000, height = 700;
    // Initial position
    Point center;
    center.x = 20;
    center.y = 20;

    Mat image = Mat::zeros(height, width, CV_8UC3);

    Point pt1, pt2;
    pt1.x = 500;
    pt1.y = 230;
    pt2.x = 550;
    pt2.y = 370;

    rectangle(image, pt1, pt2, CV_RGB(0,255,0), -1);

    Point center_C;
    center_C.x = 268;
    center_C.y = 189;

    circle(image, center_C, 50, CV_RGB(0,0,255),-1);

    center_C.x = 853;
    center_C.y = 478;

    circle(image, center_C, 70, CV_RGB(0,0,255),-1);


    Mat n_image = image.clone();
    circle(n_image, center, RADIUS, CV_RGB(255,0,0),-1);

    imshow(wndname, n_image);
    waitKey(DELAY);

    // Initialize the checkpoints
    double dangle = 2*PI/(double)NUM_POINTS;
    cout<<"dangle: "<<dangle<<endl;
    double Circle_x[NUM_POINTS];
    double Circle_y[NUM_POINTS];
    for (int i = 0; i < NUM_POINTS; i++){
        Circle_x[i] = RADIUS*cos(dangle*i);
        Circle_y[i] = RADIUS*sin(dangle*i);
    }

    // Initialize velocity at x, y direction.
    double theta = (double)30/180*PI;
    double dx = cos(theta);
    double dy = sin(theta);
    double speed = 10;

    int num_hit;
    double total_angle_idx;
    double next_x;
    double next_y;
    double alpha;
    double error;
    while (1){
        // Check if the next position will hit. 
        num_hit = 0;
        vector<int> idx_list;
        total_angle_idx = 0;
        next_x = center.x + speed*dx;
        next_y = center.y + speed*dy;
        // cout<<(unsigned int)max(intensity)<<endl;
        for (int i = 0; i<NUM_POINTS; i++){
            if ((round(next_x + Circle_x[i])<0)||
                (round(next_x + Circle_x[i])>=width)||
                (round(next_y + Circle_y[i])<0)||
                (round(next_y + Circle_y[i])>=height)){
                total_angle_idx+=i;
                idx_list.push_back(i);
                cout << i <<endl;
                num_hit+=1;
            }
            else{
                Vec3b intensity = image.at<Vec3b>(next_y + Circle_y[i], next_x + Circle_x[i]);
                if (max(max(intensity[0],intensity[1]),intensity[2]) == 255){
                    total_angle_idx+=i;
                    idx_list.push_back(i);
                    cout << i <<endl;
                    num_hit+=1;
                }
            }
        }
        
        // Two cases: if hit, if not hit.
        // If not hit
        if (num_hit == 0){
            center.x = next_x;
            center.y = next_y;
            n_image = image.clone();
            circle(n_image, center, RADIUS, CV_RGB(255,0,0),-1);
        }
        // If hit
        else{
            alpha = (double)total_angle_idx/(double)num_hit;
            // alpha = (double)idx_list[num_hit/2];
            error = 0;
            for (int i = 0; i<num_hit; i++){
                error += abs(idx_list[i] - alpha);
            }
            if (error/num_hit>THRESH){
                total_angle_idx = 0;
                for (int i = 0; i<num_hit; i++){
                    if (idx_list[i] > NUM_POINTS*3/4){
                        total_angle_idx += idx_list[i] - NUM_POINTS;
                    }
                    else{
                        total_angle_idx += idx_list[i];
                    }
                }
                alpha = (double)total_angle_idx/(double)num_hit;
            }

            theta = 2*alpha*dangle - theta - PI;
            dx = cos(theta);
            dy = sin(theta);
            cout<<"num_hit: "<<num_hit<<endl;
            // cout<<"total_angle_idx "<<total_angle_idx<<endl;
            // cout<<"alpha: "<<alpha<<endl;
            cout<<"theta is: "<<theta/PI*180<<endl;
        }

        imshow(wndname, n_image);
        if(waitKey(DELAY) >= 0)
            return 0;
    }
    // for (i = 0; i < NUMBER; i++)
    // {
    //     Point pt1, pt2;
    //     pt1.x = rng.uniform(x1, x2);
    //     pt1.y = rng.uniform(y1, y2);
    //     pt2.x = rng.uniform(x1, x2);
    //     pt2.y = rng.uniform(y1, y2);

    //     line( image, pt1, pt2, randomColor(rng), rng.uniform(1,10), lineType );

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    // for (i = 0; i < NUMBER; i++)
    // {
    //     Point pt1, pt2;
    //     pt1.x = rng.uniform(x1, x2);
    //     pt1.y = rng.uniform(y1, y2);
    //     pt2.x = rng.uniform(x1, x2);
    //     pt2.y = rng.uniform(y1, y2);
    //     int thickness = rng.uniform(-3, 10);

    //     rectangle( image, pt1, pt2, randomColor(rng), MAX(thickness, -1), lineType );

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    // for (i = 0; i < NUMBER; i++)
    // {
    //     Point center;
    //     center.x = rng.uniform(x1, x2);
    //     center.y = rng.uniform(y1, y2);
    //     Size axes;
    //     axes.width = rng.uniform(0, 200);
    //     axes.height = rng.uniform(0, 200);
    //     double angle = rng.uniform(0, 180);

    //     ellipse( image, center, axes, angle, angle - 100, angle + 200,
    //              randomColor(rng), rng.uniform(-1,9), lineType );

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    // for (i = 0; i< NUMBER; i++)
    // {
    //     Point pt[2][3];
    //     pt[0][0].x = rng.uniform(x1, x2);
    //     pt[0][0].y = rng.uniform(y1, y2);
    //     pt[0][1].x = rng.uniform(x1, x2);
    //     pt[0][1].y = rng.uniform(y1, y2);
    //     pt[0][2].x = rng.uniform(x1, x2);
    //     pt[0][2].y = rng.uniform(y1, y2);
    //     pt[1][0].x = rng.uniform(x1, x2);
    //     pt[1][0].y = rng.uniform(y1, y2);
    //     pt[1][1].x = rng.uniform(x1, x2);
    //     pt[1][1].y = rng.uniform(y1, y2);
    //     pt[1][2].x = rng.uniform(x1, x2);
    //     pt[1][2].y = rng.uniform(y1, y2);
    //     const Point* ppt[2] = {pt[0], pt[1]};
    //     int npt[] = {3, 3};

    //     polylines(image, ppt, npt, 2, true, randomColor(rng), rng.uniform(1,10), lineType);

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    // for (i = 0; i< NUMBER; i++)
    // {
    //     Point pt[2][3];
    //     pt[0][0].x = rng.uniform(x1, x2);
    //     pt[0][0].y = rng.uniform(y1, y2);
    //     pt[0][1].x = rng.uniform(x1, x2);
    //     pt[0][1].y = rng.uniform(y1, y2);
    //     pt[0][2].x = rng.uniform(x1, x2);
    //     pt[0][2].y = rng.uniform(y1, y2);
    //     pt[1][0].x = rng.uniform(x1, x2);
    //     pt[1][0].y = rng.uniform(y1, y2);
    //     pt[1][1].x = rng.uniform(x1, x2);
    //     pt[1][1].y = rng.uniform(y1, y2);
    //     pt[1][2].x = rng.uniform(x1, x2);
    //     pt[1][2].y = rng.uniform(y1, y2);
    //     const Point* ppt[2] = {pt[0], pt[1]};
    //     int npt[] = {3, 3};

    //     fillPoly(image, ppt, npt, 2, randomColor(rng), lineType);

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    

    // for (i = 1; i < NUMBER; i++)
    // {
    //     Point org;
    //     org.x = rng.uniform(x1, x2);
    //     org.y = rng.uniform(y1, y2);

    //     putText(image, "Testing text rendering", org, rng.uniform(0,8),
    //             rng.uniform(0,100)*0.05+0.1, randomColor(rng), rng.uniform(1, 10), lineType);

    //     imshow(wndname, image);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    // Size textsize = getTextSize("OpenCV forever!", FONT_HERSHEY_COMPLEX, 3, 5, 0);
    // Point org((width - textsize.width)/2, (height - textsize.height)/2);

    // Mat image2;
    // for( i = 0; i < 255; i += 2 )
    // {
    //     image2 = image - Scalar::all(i);
    //     putText(image2, "OpenCV forever!", org, FONT_HERSHEY_COMPLEX, 3,
    //             Scalar(i, i, 255), 5, lineType);

    //     imshow(wndname, image2);
    //     if(waitKey(DELAY) >= 0)
    //         return 0;
    // }

    waitKey();
    return 0;
}

#ifdef _EiC
main(1,"drawing.c");
#endif
