#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//



/**************************** LIST FUNCTIONS ******************************************************/
#define LIST_SIZE 50

typedef struct{
  int x;
  int y;
} list_n;

typedef struct{
  list_n list_nodes[LIST_SIZE];
  int last_node_pos;
} list;


void initialize_list(list *list){
	int i;

	for(i=0;i<LIST_SIZE;i++){
		list->list_nodes[i].x = -1;
		list->list_nodes[i].y = -1;
	}
	list->last_node_pos = 0;
}

int pop(list *list, list_n *node){
	if(list->last_node_pos >= 0){
		node->x = list->list_nodes[list->last_node_pos-1].x;
		node->y = list->list_nodes[list->last_node_pos-1].y;
		list->list_nodes[list->last_node_pos-1].x = -1;
		list->list_nodes[list->last_node_pos-1].y = -1;
		list->last_node_pos--;
		return 1;
	}else{
		return 0;
	}
}

int push(list *list, list_n node){
	if(list->last_node_pos < LIST_SIZE - 1){
		list->last_node_pos++;
		list->list_nodes[list->last_node_pos-1].x = node.x;
		list->list_nodes[list->last_node_pos-1].y = node.y;
		return 1;
	}else{
		return 0;
	}
}


/**************************** GRID MOVEMENT FUNCTIONS *********************************************/

#define MINDISTANCE 20
#define TIME_BEFORE_TURN 2400
#define TARGETCOLOR 8
#define TARGETCOLOR_SILVER 30
#define CORRECTION_FACTOR 1
#define MAX_TIME_OUTSIDE_TAPE 2000
#define TOLERANCE 8
#define TRY_TO_TURN_LEFT_DEFAULT false
#define DIREITA 1
#define ESQUERDA 0
#define TURN_TIME 120
#define CELL_SIZE 627 // Cell size: 30. Angular distance: 30*20,9 = 627

void move_straight(int ammount)
{
	int counter = 1;
	int error;
	int FORWARDSPEED = 20;
	int adjustment;
	int traveled_distance;
	bool try_to_turn_left = TRY_TO_TURN_LEFT_DEFAULT;

	while(counter <= ammount){
		resetMotorEncoder(motorC);
		resetMotorEncoder(motorB);
		try_to_turn_left = TRY_TO_TURN_LEFT_DEFAULT;
		do                                      //Percorre linha
		{
			error = abs(SensorValue[colorSensor] - TARGETCOLOR);
			adjustment = round(error * CORRECTION_FACTOR);
			if(error < TOLERANCE){
				try_to_turn_left = TRY_TO_TURN_LEFT_DEFAULT;
			}

			if(!try_to_turn_left){
				motor[leftMotor] = FORWARDSPEED*1.5 - adjustment;
				motor[rightMotor] = FORWARDSPEED*0.5 + adjustment;

			}else{
				motor[leftMotor] = FORWARDSPEED*0.5 - adjustment;
				motor[rightMotor] = FORWARDSPEED*1.5 + adjustment;
			}
			wait1Msec(50);
			traveled_distance = (getMotorEncoder(motorB)+getMotorEncoder(motorC))/2;
		}while(traveled_distance <= CELL_SIZE);
		counter++;
	}
}

void turn(int direction){
	int distance;
	int error;
	int FORWARDSPEED = 20;
	resetMotorEncoder(motorC);
	resetMotorEncoder(motorB);

	if(direction == DIREITA){
		do{
			motor[leftMotor] = FORWARDSPEED;
			motor[rightMotor] = -FORWARDSPEED;
			error = abs(SensorValue[colorSensor] - TARGETCOLOR);
			distance = getMotorEncoder(motorB);
		}while(distance < TURN_TIME || error > TOLERANCE);
	}else if(direction == ESQUERDA){
		do{
			motor[leftMotor] = -FORWARDSPEED;
			motor[rightMotor] = FORWARDSPEED;
			error = abs(SensorValue[colorSensor] - TARGETCOLOR);
			distance = getMotorEncoder(motorC);
		}while(distance < TURN_TIME || error > TOLERANCE);
	}
}

/**************************** MAIN ROUTINE ********************************************************/

task main()
{
	int grid[4][6] = {-1};
	move_straight(5);
	turn(DIREITA);
	move_straight(1);
	turn(DIREITA);
	move_straight(3);

	return;
}
