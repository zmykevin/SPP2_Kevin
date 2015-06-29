/*The demo code to control the servos*/
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <dynamixel.h>

//Control Table Address
#define P_GOAL_POSITION_L	30
#define P_GOAL_POSITION_H	31
#define P_PRESENT_POSITION_L	36
#define P_PRESENT_POSITION_H	37
#define P_MOVING		46
#define ANGLE_RANGE		180
#define POSITION_RANGE		2028	
	
// Defulat setting
#define DEFAULT_BAUDNUM		1 // 1Mbps
#define DEFAULT_ID_SERVO_1	1
#define DEFAULT_ID_SERVO_2	2
//Included Functions
void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);
double position_to_angle(int position);
int angle_to_position(double angle);
void move_to_position(double present_angle_1, double present_angle_2, double goal_angle_1, double goal_angle_2);

int main()
{
int baudnum = 1;
int deviceIndex = 0;
int CommStatus;
double presentangle1,presentangle2;
double goalangle1,goalangle2;
int moving;

printf("Welcome to the servo control program! \n");
if( dxl_initialize(deviceIndex, baudnum) == 0 )
{
	printf( "Failed to open USB2Dynamixel!\n" );
	printf( "Press Enter key to terminate...\n" );
	getchar();
	return 0;
}
else
	printf( "Succeed to open USB2Dynamixel!\n" );
while(1)
{
	printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
	if(getchar() == 0x1b)
		break;
	//read the locations of the two positions
	presentangle1 = position_to_angle(dxl_read_word(DEFAULT_ID_SERVO_1, P_PRESENT_POSITION_L));
	presentangle2 = position_to_angle(dxl_read_word(DEFAULT_ID_SERVO_2, P_PRESENT_POSITION_L));
        printf("The current position for motor 1 is: %lf \n", presentangle1);
	printf("The current position for motor 2 is: %lf \n", presentangle2);

	CommStatus = dxl_get_result();
        
		if( CommStatus == COMM_RXSUCCESS )
		{
			PrintErrorCode();
		}
		else
		{
			PrintCommStatus(CommStatus);	
			break;
		}
	
        printf("Please enter the destination angel fo motor 1(in degree from 0-180):");
	scanf("%lf",&goalangle1);
        printf("Please enter the destination angel fo motor 2(in degree from 0-180):");
	scanf("%lf",&goalangle2);	

	//Write the goal position
	//dxl_write_word(DEFAULT_ID_SERVO_1, P_GOAL_POSITION_L,angle_to_position(goalangle1));
	//dxl_write_word(DEFAULT_ID_SERVO_2, P_GOAL_POSITION_L,angle_to_position(goalangle2));
        move_to_position(presentangle1,presentangle2,goalangle1,goalangle2);
	
	//Move to the location
	moving = dxl_read_byte( DEFAULT_ID_SERVO_1, P_MOVING );
        CommStatus = dxl_get_result();
		if( CommStatus == COMM_RXSUCCESS )
		{
			if( moving == 0 )
			{
				printf("The moving process is done");				
			}

			PrintErrorCode();
		}
		else
		{
			PrintCommStatus(CommStatus);
			break;
		}
	moving = dxl_read_byte( DEFAULT_ID_SERVO_2, P_MOVING );
        CommStatus = dxl_get_result();
		if( CommStatus == COMM_RXSUCCESS )
		{
			if( moving == 0 )
			{
				printf("The moving process is done");				
			}

			PrintErrorCode();
		}
		else
		{
			PrintCommStatus(CommStatus);
			break;
		}
	usleep(300000);
}
// Close device
dxl_terminate();
printf( "Press Enter key to terminate...\n" );
getchar();
return 0;
}

// Print communication result
void PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		printf("COMM_TXFAIL: Failed transmit instruction packet!\n");
		break;

	case COMM_TXERROR:
		printf("COMM_TXERROR: Incorrect instruction packet!\n");
		break;

	case COMM_RXFAIL:
		printf("COMM_RXFAIL: Failed get status packet from device!\n");
		break;

	case COMM_RXWAITING:
		printf("COMM_RXWAITING: Now recieving status packet!\n");
		break;

	case COMM_RXTIMEOUT:
		printf("COMM_RXTIMEOUT: There is no status packet!\n");
		break;

	case COMM_RXCORRUPT:
		printf("COMM_RXCORRUPT: Incorrect status packet!\n");
		break;

	default:
		printf("This is unknown error code!\n");
		break;
	}
}

// Print error bit of status packet
void PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		printf("Input voltage error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		printf("Angle limit error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		printf("Overheat error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		printf("Out of range error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		printf("Checksum error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		printf("Overload error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		printf("Instruction code error!\n");
}

double position_to_angle(int position)
{
  double angle = ((double)position)/POSITION_RANGE*ANGLE_RANGE;
  return angle;
}

int angle_to_position(double angle)
{
  int position = angle/ANGLE_RANGE*POSITION_RANGE;
  return position;
}

void move_to_position(double present_angle_1, double present_angle_2, double goal_angle_1, double goal_angle_2)
{
	int commstatus;
	double speed = 0.05;
	double angle_increment_1 = (goal_angle_1-present_angle_1)*speed;
	double angle_increment_2 = (goal_angle_2-present_angle_2)*speed;
        double next_position_1 = present_angle_1 + angle_increment_1;
	double next_position_2 = present_angle_2 +angle_increment_2;
	double total_move = 1/speed;
	int i;
	for(i = 0; i < total_move; i ++)
{
	dxl_write_word(DEFAULT_ID_SERVO_1, P_GOAL_POSITION_L,angle_to_position(next_position_1));
	dxl_write_word(DEFAULT_ID_SERVO_2, P_GOAL_POSITION_L,angle_to_position(next_position_2));
	commstatus = dxl_get_result();
	if (commstatus != COMM_RXSUCCESS)
	{	
		PrintCommStatus(commstatus);
		break;
	}
	next_position_1 += angle_increment_1;
	next_position_2 += angle_increment_2;
	usleep(35000);
}

	return;
}

