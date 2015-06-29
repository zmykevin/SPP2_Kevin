from dxl.dxlchain import DxlChain
from matrix_test import *
import time

#Define some cosntant
INITIAL_POSITION = 1024
INITIAL_POSITION_1 = 1024
INITIAL_POSITION_2 = 512
ANGLE_RANGE = 180
POSITION_RANGE = 2048
SPEED = 5

#Define functions to use
def position_to_angle(position):
    angle = float(position)/POSITION_RANGE*ANGLE_RANGE
    return angle

def angle_to_position(angle):
    position = angle/ANGLE_RANGE*POSITION_RANGE
    position = int(position)
    return position

def show_PID_config(chain_dxl,id):
    print('The PID config for motor %d:'% (id))
    reg_name_p = "p_gain"
    reg_name_i = "i_gain"
    reg_name_d = "d_gain"
    print('p_gain:%d'% (chain_dxl.get_reg(id,reg_name_p)))
    print('i_gain:%d'% (chain_dxl.get_reg(id,reg_name_i)))
    print('d_gain:%d'% (chain_dxl.get_reg(id,reg_name_d)))

def setup_PID(chain_dxl,id,pgain,igain,dgain):
    reg_name_p = "p_gain"
    reg_name_i = "i_gain"
    reg_name_d = "d_gain"
    chain_dxl.set_reg(id,reg_name_p,pgain)
    chain_dxl.set_reg(id,reg_name_i,igain)
    chain_dxl.set_reg(id,reg_name_d,dgain)
    show_PID_config(chain_dxl,id)

def straight_line_move(chain,cp,fp):
    increment = (fp-cp)*0.02
    process = cp
    if increment[0,0] > 0:
        while process[0,0] <= fp[0,0]:
            next = process + increment
            #yaw = position_to_angle(chain.get_position(1)[1])
            #pitch = position_to_angle(chain.get_position(2)[2])
            #lp = laser_point(yaw,pitch)
            #print('servo_coordinate: %f,%f\n'% (yaw,pitch))
            lp = np.array([[HEAD_POINT_X],[-0.5],[HEAD_POINT_Z]])
            [n_yaw,n_pitch] = inverse_kinematics(next,lp)
            chain.goto(1,angle_to_position(n_yaw),SPEED)
            chain.goto(2,angle_to_position(n_pitch),SPEED)
            print('%f,%f\n'% (process[0,0],process[1,0]))
            process = next
    elif increment[0,0] < 0:
        while process[0,0] >= fp[0,0]:
            next = process + increment
            #yaw = position_to_angle(chain.get_position(1)[1])
            #pitch = position_to_angle(chain.get_position(2)[2])
            #print('servo_coordinate: %f,%f\n'% (yaw,pitch))
            lp = np.array([[HEAD_POINT_X],[-0.5],[HEAD_POINT_Z]])
            [n_yaw,n_pitch] = inverse_kinematics(next,lp)
            chain.goto(1,angle_to_position(n_yaw),SPEED)
            chain.goto(2,angle_to_position(n_pitch),SPEED)
            print('%f,%f\n'% (process[0,0],process[1,0]))

            process = next
    elif increment[1,0] > 0:
        while process[1,0] <= fp[1,0]:
            next = process + increment
            #yaw = position_to_angle(chain.get_position(1)[1])
            #pitch = position_to_angle(chain.get_position(2)[2])
            #lp = laser_point(yaw,pitch))
            #print('servo_coordinate: %f,%f\n'% (yaw,pitch))
            lp = np.array([[HEAD_POINT_X],[-0.5],[HEAD_POINT_Z]])
            [n_yaw,n_pitch] = inverse_kinematics(next,lp)
            chain.goto(1,angle_to_position(n_yaw),SPEED)
            chain.goto(2,angle_to_position(n_pitch),SPEED)
            print('%f,%f\n'% (process[0,0],process[1,0]))
            process = next
    else:
        while process[1,0] > fp[1,0]:
            next = process + increment
            #yaw = position_to_angle(chain.get_position(1)[1])
            #pitch = position_to_angle(chain.get_position(2)[2])
            #lp = laser_point(yaw,pitch)
            #print('servo_coordinate: %f,%f\n'% (yaw,pitch))
            lp = np.array([[HEAD_POINT_X],[-0.5],[HEAD_POINT_Z]])
            [n_yaw,n_pitch] = inverse_kinematics(next,lp)
            chain.goto(1,angle_to_position(n_yaw),SPEED)
            chain.goto(2,angle_to_position(n_pitch),SPEED)
            print('%f,%f\n'% (process[0,0],process[1,0]))
            process = next

def current_projection(chain):
    yaw = chain.get_position(1)[1]
    pitch = chain.get_position(2)[2]
    projection = forward_kinematics(position_to_angle(yaw),position_to_angle(pitch))
    print("The current point is: %f,%f"% (projection[0,0],projection[1,0]))

def main():
    chain = DxlChain("/dev/ttyUSB0",rate=1000000)
    motors=chain.get_motor_list()
    print motors

    reset_id = raw_input('Do you want to change your current motor id?[Y/N]:')
    if reset_id in ['y','Y','yes','Yes']:
        oldid = int(raw_input('Which motor ID do you want to change:'))
        newid = int(raw_input('What is the new ID you want to set:'))
        chain.set_reg(oldid,'id',newid)

    #Before we start to print out the motor we should first set up the P, I, D control
    for motor in motors:
        if motor == 1:
            setup_PID(chain,motor,10,10,8)#10,10,8
        elif motor == 2:
            setup_PID(chain,motor,25,100,23)#[30,100,10] [25,100,0]
        else:
            break

    #First step is to initialize the motor to a specific motion
    for motor in motors:
        if motor == 1:
            chain.goto(motor,INITIAL_POSITION_1,SPEED)
        elif motor == 2:
            chain.goto(motor,INITIAL_POSITION_2,SPEED)
        else:
            chain.goto(motor,INITIAL_POSITION,SPEED)

    while True:
        mode = int(raw_input("1.control servo 2. draw square: "))
        if mode != 1 and mode != 2:
            print ("The mode selection you enter is invalid please re-enter the mode")
        else:
            break
    #if the user select control servo then run the first process
    if mode == 1:
        while True:
            try:
                print ("the current positions for the motors are:")
                for motor in motors:
                    pos = chain.get_position(motor)
                    pos = pos[motor]
                    print('%d: %f'% (motor,position_to_angle(pos)))
                current_projection(chain)

                for motor in motors:
                    new_pos = raw_input('What is the new position you want to goto for motor %d(0-180):'% (motor))
                    new_pos = angle_to_position(float(new_pos))
                    chain.goto(motor,new_pos,SPEED)
                time.sleep(1)
            except KeyboardInterrupt:
                print('\n')
                print ("The program end")
                break
    else:
        while True:
            try:
                yaw = float(raw_input("please define the angle for the base motor: "))
                pitch = float(raw_input("please define the desired angle for the above motor: "))
                start_time = time.time()
                #initialize the position
                chain.goto(1,angle_to_position(yaw),SPEED)
                chain.goto(2,angle_to_position(pitch),SPEED)
                time.sleep(2)

                #The default direction will be clock wise
                cp = forward_kinematics(yaw,pitch)
                current_projection(chain)


                fp1 = cp + np.array([[100],
                                    [0],
                                    [0],
                                    [0]])

                straight_line_move(chain,cp,fp1)
                print("Arrive fp1")
                current_projection(chain)
                print fp1
                time.sleep(1)

                fp2 = fp1 +  np.array([[0],
                                    [-100],
                                    [0],
                                    [0]])

                straight_line_move(chain,fp1,fp2)
                current_projection(chain)
                print("Arrive fp2")
                current_projection(chain)
                print fp2
                time.sleep(1)

                fp3 = fp2 + np.array([[-100],
                                    [0],
                                    [0],
                                    [0]])

                straight_line_move(chain,fp2,fp3)
                print("Arrive fp3")
                current_projection(chain)
                print fp3
                time.sleep(1)

                fp4 = fp3 + np.array([[0],
                                    [100],
                                    [0],
                                    [0]])

                straight_line_move(chain,fp3,fp4)
                print("Arrive fp4")
                current_projection(chain)
                print fp4
                time.sleep(0)
                total_time = time.time()-start_time
                print("the process takes %f seconds"% (total_time))
            except KeyboardInterrupt:
                print('\n')
                print ("The program end")
                break
    #chain.disable()
main()