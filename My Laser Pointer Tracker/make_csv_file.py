__author__ = 'kevin'
import csv
import time
import numpy as np

def find_the_closest_pixels(the_pixel,pixel_list):
    min_dif = 100000000
    min = (0,0)
    if the_pixel not in pixel_list:
        for i in pixel_list:
            dif = (i[0]-the_pixel[0])**2+(i[1]-the_pixel[1])**2
            if dif < min_dif:
                min_dif = dif
                min = i
        return pixel_list.index(min)
    else:
        return pixel_list.index(the_pixel)

def v2_find_the_closest_pixels(the_pixel,pixel_list):
    if the_pixel not in pixel_list:
        found = False
        for dif in range(1,50):
            my_min_dif = 10000
            my_min = (0,0)
            tl = (max([the_pixel[0]-dif,0]),max([the_pixel[1]-dif,0]))
            br = (min([the_pixel[0]+dif,639]),min([the_pixel[1]+dif,479]))
            for i in range(tl[0],br[0]+1):
                for j in range(tl[1],br[1]+1):
                    if (i,j) in pixel_list:
                        found = True
                        the_dif = (i-the_pixel[0])**2+(j-the_pixel[1])**2
                        if the_dif < my_min_dif:
                            my_min_dif = the_dif
                            my_min = (i,j)
            if found:
                return pixel_list.index(my_min)
    else:
        return pixel_list.index(the_pixel)
if __name__ == "__main__":
    P = []
    T = []
    with open('reference_table.csv','r') as csvfile:
        csv_f = csv.reader(csvfile)
        for row in csv_f:
            r_pixel_point = (float(row[3]),float(row[2]))
            if r_pixel_point not in P:
                P.append(r_pixel_point)
                s_angel = (float(row[0]),float(row[1]))
                T.append(s_angel)

    ########Save a matrix to file
    saved_matrix_1 = np.zeros((640,480),dtype = "float32")
    saved_matrix_2 = np.zeros((640,480),dtype = "float32")
    for i in range(0,640):
        for j in range(0,480):
            print(i,j)
            the_index = find_the_closest_pixels((i,j),P)
            saved_matrix_1[i,j] = T[the_index][0]
            saved_matrix_2[i,j] = T[the_index][1]
    np.savetxt("angel_1.txt",saved_matrix_1)
    np.savetxt("angel_2.txt",saved_matrix_2)