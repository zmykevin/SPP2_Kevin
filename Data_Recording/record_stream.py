__author__ = 'kevin'
import cv2
from camera_setup import *

if __name__ == '__main__':
    #Initialize the camera and start to capture
    camera = flycamera_init()
    camera.start_capture()

    #define the codec and create Video Writer oject
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('10_7_evening_1.avi',fourcc,60.0,(540,380))#Define the name with date, when in a day, and a index number

    while True:
        image = undistort_image(camera)
        cropped_image = image[50:430,50:590]
        cv2.imshow('frame',cropped_image)
        #output frame
        out.write(cropped_image)
        if cv2.waitKey(1) == 27:
            break

    cv2.destroyAllWindows()
    #Turn off the camera
    camera.stop_capture()
    camera.disconnect()