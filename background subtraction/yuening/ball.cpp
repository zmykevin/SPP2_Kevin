#include "ball.h"

Ball::Ball(int init_width, int init_height, int init_radius, double init_speed, double init_theta){
    THRESH = 5;
    RADIUS = init_radius;
    speed = init_speed;
    theta = init_theta;
    l_theta = init_theta;
    ll_theta = init_theta;
    center.x = init_width;
    center.y = init_height;
    mode = 255;

    dangle = 2*PI/(double)NUM_POINTS;
    for (int i = 0; i < NUM_POINTS; i++){
        Circle_x[i] = RADIUS*cos(dangle*i);
        Circle_y[i] = RADIUS*sin(dangle*i);
    }
}

Mat Ball::updateBall(Mat image_frame, Mat subtraction){
    int height = image_frame.rows;
    int width = image_frame.cols;

    Mat n_image = image_frame.clone();
    Mat n_subtraction;
    medianBlur(subtraction, n_subtraction, 5);
    medianBlur(subtraction, subtraction, 5);

    // imshow("blurred subtraction", subtraction);
    // waitKey(0);

    double dx = cos(theta);
    double dy = sin(theta);

    int num_hit = 0;
    double total_angle_idx = 0;
    double next_x = center.x + speed*dx;
    double next_y = center.y + speed*dy;

    vector<int> idx_list;
    int countNum = 0;
    for (int i = 0; i<NUM_POINTS; i++){
        if ((round(next_x + Circle_x[i])<0)||
            (round(next_x + Circle_x[i])>=width)||
            (round(next_y + Circle_y[i])<0)||
            (round(next_y + Circle_y[i])>=height)){
            total_angle_idx+=i;
            idx_list.push_back(i);
            // cout << i <<endl;
            num_hit++;
        }
        else{
            // Vec3b intensity = image.at<Vec3b>(next_y + Circle_y[i], next_x + Circle_x[i]);
            // if (max(max(intensity[0],intensity[1]),intensity[2]) == 255){
            Scalar intensity = n_subtraction.at<uchar>(next_y + Circle_y[i], next_x + Circle_x[i]);
            if (intensity[0] == mode){
                total_angle_idx+=i;
                idx_list.push_back(i);
                // cout << i <<endl;
                num_hit++;
                countNum++;
            }
        }
    }
    
    // Two cases: if hit, if not hit.
    // If not hit
    if (num_hit == 0){
        center.x = next_x;
        center.y = next_y;
        circle(n_image, center, RADIUS, 0,-1);
        circle(n_subtraction, center, RADIUS, 255,-1);
    }
    // If hit
    else if (countNum == NUM_POINTS){
        center.x = next_x;
        center.y = next_y;
        circle(n_image, center, RADIUS, 0,-1);
        mode = 255 - mode;
        cout<<"Yes"<<endl;
    }
    else{
        double alpha = (double)total_angle_idx/(double)num_hit;
        // alpha = (double)idx_list[num_hit/2];
        double error = 0;
        for (int i = 0; i<num_hit; i++){
            error += abs(idx_list[i] - alpha);
        }
        if (error/num_hit>THRESH){
            total_angle_idx = 0;
            for (int i = 0; i<num_hit; i++){
                if (idx_list[i] > NUM_POINTS*1/2){
                    total_angle_idx += idx_list[i] - NUM_POINTS;
                }
                else{
                    total_angle_idx += idx_list[i];
                }
            }
            alpha = (double)total_angle_idx/(double)num_hit;
        }

        ll_theta = l_theta;
        l_theta = theta;
        theta = 2*alpha*dangle - theta - PI;
        while (theta<0){
            theta+=2*PI;
        }
        while (theta>=2*PI){
            theta-=2*PI;
        }
        // cout<< ll_theta<<endl;
        // cout<< l_theta<<endl;
        // cout<< theta <<endl;
        if (abs(ll_theta - theta)<0.0001){
            //TODO: Make the range smaller than 360
            theta +=((double)(rand()%360))/360*PI;
            // cout<<"Yes"<<endl;
            // cout<<theta<<endl;
        }

        // These four lines are dangerous
        // dx = cos(theta);
        // dy = sin(theta);
        // next_x = center.x + speed*dx;
        // next_y = center.y + speed*dy;

        // int flag = 0;
        // for (int i = 0; i<NUM_POINTS; i++){
        //     if ((round(next_x + Circle_x[i])<0)||
        //         (round(next_x + Circle_x[i])>=width)||
        //         (round(next_y + Circle_y[i])<0)||
        //         (round(next_y + Circle_y[i])>=height)){
        //         flag++;
        //     }
        // }

        // while (flag){
        //     theta +=((double)(rand()%360))/360*PI;
        //     cout<<"Yes"<<endl;
        //     dx = cos(theta);
        //     dy = sin(theta);
        //     next_x = center.x + speed*dx;
        //     next_y = center.y + speed*dy;
        //     flag = 0;
        //     for (int i = 0; i<NUM_POINTS; i++){
        //         if ((round(next_x + Circle_x[i])<0)||
        //             (round(next_x + Circle_x[i])>=width)||
        //             (round(next_y + Circle_y[i])<0)||
        //             (round(next_y + Circle_y[i])>=height)){
        //             flag++;
        //         }
        //     }
        // }
        
        circle(n_image, center, RADIUS, 255,-1);
        circle(n_subtraction, center, RADIUS, 255,-1);

        // cout<<"num_hit: "<<num_hit<<endl;
        // cout<<"total_angle_idx "<<total_angle_idx<<endl;
        // cout<<"alpha: "<<alpha<<endl;
        // cout<<"theta is: "<<theta/PI*180<<endl;
    }

    return n_subtraction;
}