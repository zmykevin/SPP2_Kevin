__author__ = 'kevin'
import flycapture2 as fc2
import cv2
import numpy as np

#Define camera matrix and distortion coefficient
camera_M = np.array([[411.42948014, 0, 331.38683566], [0, 411.0871804, 208.84949458], [0, 0, 1]])
distortion_coefficient = np.array([-0.33504357,0.07164384, -0.00376871 ,0.0016508  ,0.15057044])

#Define Gaussian blur parameters:
Gaussian_Ratio = 5

def flycamera_init():
    flycamera = fc2.Context()
    flycamera.connect(*flycamera.get_camera_from_index(0))
    #set up the camera frame rate and mode
    flycamera.set_video_mode_and_frame_rate(fc2.VIDEOMODE_640x480Y8,fc2.FRAMERATE_60)
    #set up the camera exposure
    p = flycamera.get_property(fc2.AUTO_EXPOSURE)
    p['auto_manual_mode'] = False
    p['value_a'] = 25
    flycamera.set_property(**p)
    return flycamera

def undistort_image(flycamera):
    im = fc2.Image()
    flycamera.retrieve_buffer(im)
    gray_image = np.array(im)
    image = cv2.cvtColor(gray_image,cv2.COLOR_GRAY2BGR)
    #Undistort the Image
    h,w = image.shape[:2]
    #mask is saved to crop the image
    mask = np.zeros(image.shape[:2],dtype = "uint8")
    cx = image.shape[1]/2
    cy = image.shape[0]/2
    cv2.rectangle(mask,(cx-280,cy-220),(cx+280,cy+220),255,-1)

    #Undistort the original image
    newcamera, roi = cv2.getOptimalNewCameraMatrix(camera_M, distortion_coefficient, (w,h), 0)
    fixed_image = cv2.undistort(image,camera_M,distortion_coefficient,None,newcamera)
    #crop the undistorted image
    fixed_image = cv2.bitwise_and(fixed_image,fixed_image,mask = mask)
    return fixed_image

def find_brightest_point(image):
    gray_image = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    #Apply Gaussian Blur
    gray_image = cv2.GaussianBlur(gray_image,(Gaussian_Ratio,Gaussian_Ratio),0)
    (minVal, maxVal, minLoc,maxLoc) = cv2.minMaxLoc(gray_image)
    return maxVal,maxLoc
