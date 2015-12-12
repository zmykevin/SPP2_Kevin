__author__ = 'kevin'
import pbcvt
import numpy as np
import cv2
from scipy import ndimage
from camera_setup import *

def neighbour_filter(subtraction_background):
    neighbour_filter = np.array([[1,1,0],[1,0,1],[0,1,1]],dtype = 'uint8');
    result = ndimage.convolve(subtraction_background,neighbour_filter,mode = 'constant',cval = 0.0);
    foreground = np.where(subtraction_background == 255,1,0);
    background = 1-foreground;
    #Filter the false foreground
    updated_foreground = np.where(np.multiply(result,foreground) >= 5*255,255,0);
    #Filter the false background
    updated_background = np.where(np.multiply(result,background) >= 3*255,255,0);
    updated_result = updated_foreground + updated_background
    return updated_result

def gaussian_model_initialization(cap,frame_width,frame_height,K,initial_num_sample):
    #Initialization process
    #gaussian_mean = np.zeros((K,frame_height*frame_width))
    #gaussian_std = np.zeros((K,frame_height*frame_width))
    #Initialization of gaussian weight
    gaussian_weight = np.ones((K,frame_height*frame_width))*(1.0/K)
    #Initialize of gaussian mean
    '''
    for i in range(0,K):
        ret,frame = cap.read()
        #Convert frame to gray
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray_frame_1D = gray_frame.flatten()
        gaussian_mean[i,:] = gray_frame_1D
    '''
    #initialization of gaussian std
    gray_frame_list = np.zeros((initial_num_sample,frame_height*frame_width))
    for j in range(0,initial_num_sample):
        ret,frame = cap.read()
        if ret:
            frame = cv2.resize(frame,(frame_width,frame_height))
            #Convert frame to gray frame
            gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
            gray_frame_1D = gray_frame.flatten()
            gray_frame_list[j,:] = gray_frame_1D;
    '''
        for k in range(0,K):
            gaussian_std[k,:] = gaussian_std[k,:] + np.abs(gray_frame_1D-gaussian_mean[k,:])
    gaussian_std = gaussian_std/initial_num_sample'''
    gaussian_mean = np.ones((K,1))*np.median(gray_frame_list,axis = 0)

    gaussian_std_1D = np.sum(np.abs(gray_frame_list-np.median(gray_frame_list,axis = 0)),axis = 0)/initial_num_sample
    gaussian_std = np.ones((K,1))*gaussian_std_1D
    return (gaussian_mean,gaussian_std,gaussian_weight)

def flycap_gaussian_model_initialization(flycam,frame_width,frame_height,K,initial_num_sample):
    #Initialization process
    #gaussian_mean = np.zeros((K,frame_height*frame_width))
    #gaussian_std = np.zeros((K,frame_height*frame_width))
    #Initialization of gaussian weight
    gaussian_weight = np.ones((K,frame_height*frame_width))*(1.0/K)
    #Initialize of gaussian mean
    '''
    for i in range(0,K):
        frame = undistort_image(flycam)
        #[frame,H] = four_pts_transormation(frame,reference_4_points)
        #Convert frame to gray
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray_frame_1D = gray_frame.flatten()
        gaussian_mean[i,:] = gray_frame_1D
    '''
   #initialization of gaussian std
    gray_frame_list = np.zeros((initial_num_sample,frame_height*frame_width))
    for j in range(0,initial_num_sample):
        frame = undistort_image(flycam)
        [frame,H] = four_pts_transormation(frame,reference_4_points)
        #frame = cv2.resize(frame,(frame_width,frame_height))
        #Convert frame to gray frame
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray_frame_1D = gray_frame.flatten()
        gray_frame_list[j,:] = gray_frame_1D;
        '''
        for k in range(0,K):
            gaussian_std[k,:] = gaussian_std[k,:] + np.abs(gray_frame_1D-gaussian_mean[k,:])'''
    gaussian_mean = np.ones((K,1))*np.median(gray_frame_list,axis = 0)
    gaussian_std_1D = np.sum(np.abs(gray_frame_list-np.median(gray_frame_list,axis = 0)),axis = 0)/initial_num_sample
    gaussian_std = np.ones((K,1))*gaussian_std_1D
    return (gaussian_mean,gaussian_std,gaussian_weight)

if __name__ == '__main__':
    print ('Background Subtraction Start')
    #Define the number of gaussina models in the mixed model
    K = 3
    #Define initial weight parameter
    initial_num_sample = 200

    #Start to capture the video
    #cap = cv2.VideoCapture('highwayI_raw.AVI')
    #cap = cv2.VideoCapture(0)
    Flycam = flycamera_init()
    Flycam.start_capture()

    first_im = undistort_image(Flycam)
    [first_im,H] = four_pts_transormation(first_im,reference_4_points)
    frame_width = first_im.shape[1]
    frame_height = first_im.shape[0]
    #frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    #frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    #resize_width = frame_width/3
    #resize_height = frame_height/3

    #Initialize the gaussian model
    #gaussian_mean,gaussian_std,gaussian_weight = gaussian_model_initialization(cap,resize_width,resize_height,K,initial_num_sample)
    #gaussian_mean,gaussian_std,gaussian_weight = gaussian_model_initialization(cap,frame_width,frame_height,K,initial_num_sample)
    gaussian_mean,gaussian_std,gaussian_weight = flycap_gaussian_model_initialization(Flycam,frame_width,frame_height,K,initial_num_sample)
    #fgbg = cv2.createBackgroundSubtractorMOG2(history = 500,varThreshold = 20,detectShadows= True)
    while(1):
        frame = undistort_image(Flycam)
        [frame,H] = four_pts_transormation(frame,reference_4_points)
        #ret,frame = cap.read()
        #frame = cv2.resize(frame,(resize_width,resize_height))
        #if ret:
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        #background_subtraction = fgbg.apply(gray_frame)
        background_subtraction = pbcvt.MOG_Background_Process(gray_frame,gaussian_mean, gaussian_std,gaussian_weight)
        background_subtraction = background_subtraction.astype(dtype = 'int64')
        background_subtraction = neighbour_filter(background_subtraction)
        background_subtraction = background_subtraction.astype(dtype = 'uint8')
        cv2.imshow('frame',background_subtraction)
        cv2.imshow('original_frame',gray_frame)
        if cv2.waitKey(1) == 27:
            break
        #else:
            #break
    #Terminate the camera
    Flycam.stop_capture()
    Flycam.disconnect()
    #cap.release()
    cv2.destroyAllWindows()