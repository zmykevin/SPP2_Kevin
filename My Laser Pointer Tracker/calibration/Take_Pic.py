__author__ = 'kevin'
import flycapture2 as fc2
import numpy as np
import cv2

 #print fc2.get_library_version()
c = fc2.Context()
c.connect(*c.get_camera_from_index(0))
p = c.get_property(fc2.FRAME_RATE)
c.set_property(**p)
c.start_capture()
im_name = 1
while True:
    im = fc2.Image()
    c.retrieve_buffer(im)
    image = cv2.cvtColor(np.array(im),cv2.COLOR_GRAY2BGR)
    cv2.imshow("image",image)
    k = cv2.waitKey(1)
    if k == ord('t'):
        cv2.imwrite('../pattern/'+str(im_name)+'.png',image)
        im_name += 1
    elif k == ord('q'):
        break
c.stop_capture()
c.disconnect()