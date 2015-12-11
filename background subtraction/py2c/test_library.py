import pbcvt
import numpy as np
import cv2
from scipy import ndimage

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
    gaussian_mean = np.zeros((K,frame_height*frame_width))
    gaussian_std = np.zeros((K,frame_height*frame_width))
    #Initialization of gaussian weight
    gaussian_weight = np.ones((K,frame_height*frame_width))*(1.0/K)
    #Initialize of gaussian mean
    for i in range(0,K):
        ret,frame = cap.read()
        #Convert frame to gray
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray_frame_1D = gray_frame.flatten()
        gaussian_mean[i,:] = gray_frame_1D
   #initialization of gaussian std
    for j in range(0,initial_num_sample):
        ret,frame = cap.read()
        #Convert frame to gray frame
        gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray_frame_1D = gray_frame.flatten()
        for k in range(0,K):
            gaussian_std[k,:] = gaussian_std[k,:] + np.abs(gray_frame_1D-gaussian_mean[k,:])
    gaussian_std = gaussian_std/initial_num_sample
    return (gaussian_mean,gaussian_std,gaussian_weight)

if __name__ == '__main__':
    print ('Background Subtraction Start')
    #Define the number of gaussina models in the mixed model
    K = 3
    #Define initial weight parameter
    initial_num_sample = 50

    #Start to capture the video
    cap = cv2.VideoCapture('highwayI_raw.AVI')
    #Get the size dimension of the frame
    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    #Initialize the gaussian model
    gaussian_mean,gaussian_std,gaussian_weight = gaussian_model_initialization(cap,frame_width,frame_height,K,initial_num_sample)

    while(cap.isOpened()):
        ret,frame = cap.read()
        if ret == True:
            gray_frame = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
            background_subtraction = pbcvt.MOG_Background_Process(gray_frame,gaussian_mean, gaussian_std,gaussian_weight);
            #background_subtraction = cv2.medianBlur(background_subtraction,5)
            #background_subtraction = background_subtraction.astype(dtype = 'int64')
            #background_subtraction = neighbour_filter(background_subtraction)
            #background_subtraction = background_subtraction.astype(dtype = 'uint8')
            cv2.imshow('frame',background_subtraction)

            if cv2.waitKey(1) == 27:
                break
        else:
            break

    cap.release()
    cv2.destroyAllWindows()