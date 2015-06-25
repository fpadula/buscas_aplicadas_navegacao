#define GRID_NROWS 4
#define GRID_NCOLS 6
#define MINDISTANCE 20
#define TARGETCOLOR 8
#define TARGETCOLOR_CITY 44
#define CORRECTION_FACTOR 1
#define MAX_TIME_OUTSIDE_TAPE 2000
#define TOLERANCE 8
#define TRY_TO_TURN_LEFT_DEFAULT false
#define DIREITA 1
#define ESQUERDA 0
#define NORTE 0
#define SUL 1
#define LESTE 2
#define OESTE 3
#define TURN_TIME 120
#define CELL_SIZE 627 // Cell size: 30. Angular distance: 30*20,9 = 627

bool isAdjacent(int x1, int y1, int x2, int y2){
	if((abs(x1-x2) + abs(y1 - y2)) == 1)
		return true;
	else
		return false;
}

void print_grid(int *grid){
	int i,j;

	for(i=0;i<GRID_NROWS;i++){
		for(j=0;j<GRID_NCOLS;j++){
			writeDebugStream("%d ", grid[GRID_NCOLS*i + j]);
		}
		writeDebugStreamLine("");
	}

}

bool set_grid(int *grid, int x, int y, int value){
	if(x >=0 && y >=0 && x < GRID_NROWS && y < GRID_NROWS){
		grid[GRID_NCOLS*x + y] = value;
		return true;
	}
	return false;
}

bool update_non_visited(int *grid, int x, int y, int value){
	if(grid[GRID_NCOLS*x + y] == -1)
		return set_grid(grid,x,y,value);
	else
		return false;
}

void move_straight(int ammount, int orientation, int *position)
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
		do
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
			switch(orientation){
				case (NORTE):
					position[1] = position[1] + 1;
				break;

				case (SUL):
					position[1] = position[1] - 1;
				break;

				case (LESTE):
					position[0] = position[0] + 1;
				break;

				case (OESTE):
					position[0] = position[0] - 1;
				break;

				default:
				break;
			}
		counter++;
	}
}

void turn(int direction, int *orientation){
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

		switch(*orientation){
			case (NORTE):
				*orientation = LESTE;
			break;

			case (SUL):
				*orientation = OESTE;
			break;

			case (LESTE):
				*orientation = SUL;
			break;

			case (OESTE):
				*orientation = NORTE;
			break;

			default:
			break;
		}
	}else if(direction == ESQUERDA){
		do{
			motor[leftMotor] = -FORWARDSPEED;
			motor[rightMotor] = FORWARDSPEED;
			error = abs(SensorValue[colorSensor] - TARGETCOLOR);
			distance = getMotorEncoder(motorC);
		}while(distance < TURN_TIME || error > TOLERANCE);

		switch(*orientation){
			case (NORTE):
				*orientation = OESTE;
			break;

			case (SUL):
				*orientation = LESTE;
			break;

			case (LESTE):
				*orientation = NORTE;
			break;

			case (OESTE):
				*orientation = SUL;
			break;

			default:
			break;
		}
	}
}

void check_surroundings(int *grid, int *position, int orientation, list *nodes_to_visit){
	bool obstacle[3];
	list_n new_node;

	turn(ESQUERDA, &orientation);
	if(SensorValue[sonarSensor] <= MINDISTANCE)
		obstacle[0] = true;
	else
		obstacle[0] = false;

	turn(DIREITA, &orientation);
	if(SensorValue[sonarSensor] <= MINDISTANCE)
		obstacle[1] = true;
	else
		obstacle[1] = false;

	turn(DIREITA, &orientation);
	if(SensorValue[sonarSensor] <= MINDISTANCE)
		obstacle[2] = true;
	else
		obstacle[2] = false;

	turn(ESQUERDA, &orientation);

	switch(orientation){
		case (NORTE):
			if (update_non_visited(grid, position[0] - 1,position[1], obstacle[0] ? 2 : 0)){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if (update_non_visited(grid, position[0],position[1] + 1, obstacle[1] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] + 1;
				push(nodes_to_visit, new_node);
			}
			if (update_non_visited(grid, position[0] + 1,position[1], obstacle[2] ? 2 : 0)){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
		break;

		case (SUL):
			if(update_non_visited(grid, position[0] + 1,position[1], obstacle[0] ? 2 : 0)){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[1] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] - 1,position[1], obstacle[2] ? 2 : 0)){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
		break;

		case (LESTE):
			if(update_non_visited(grid, position[0],position[1] + 1, obstacle[0] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] + 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] + 1,position[1], obstacle[1] ? 2 : 0)){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[2] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
		break;

		case (OESTE):
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[0] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] - 1,position[1], obstacle[1] ? 2 : 0)){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] + 1, obstacle[2] ? 2 : 0)){
				new_node.x = position[0];
				new_node.y = position[1] + 1;
				push(nodes_to_visit, new_node);
			}
		break;

		default:
		break;
	}

}

bool check_objective(){
	bool detected_collor;
	int distance;
	int error;
	int FORWARDSPEED = 20;

	resetMotorEncoder(motorC);
	resetMotorEncoder(motorB);
	detected_collor = false;
	do{
		motor[leftMotor] = FORWARDSPEED;
		motor[rightMotor] = -FORWARDSPEED;
		error = abs(SensorValue[colorSensor] - TARGETCOLOR);
		distance = getMotorEncoder(motorB);
	}while(distance < TURN_TIME);

	error = abs(SensorValue[colorSensor] - TARGETCOLOR_CITY);
	if (error < TOLERANCE)
		detected_collor = true;

	do{
		motor[leftMotor] = -FORWARDSPEED;
		motor[rightMotor] = FORWARDSPEED;
		error = abs(SensorValue[colorSensor] - TARGETCOLOR);
	}while(error > TOLERANCE);

	return detected_collor;
}

void move_to_adjacent(int *grid, int *position, int *orientation, list_n where_to_move){
	int node_orientation;
	if(position[0] - where_to_move.x > 0)
		node_orientation = OESTE;
	if(position[0] - where_to_move.x < 0)
		node_orientation = LESTE;
	if(position[1] - where_to_move.y > 0)
		node_orientation = SUL;
	if(position[1] - where_to_move.y < 0)
		node_orientation = NORTE;

	switch(orientation){
		case (NORTE):
			switch(node_orientation){
				case (NORTE):
					move_straight(1,*orientation,position);
				break;
				case (SUL):
					turn(ESQUERDA,orientation);
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (LESTE):
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (OESTE):
					turn(DIREITA,orientation);
					move_straight(1,*orientation,position);
				break;
				default:
				break;
			}
		break;

		case (SUL):
			switch(node_orientation){
				case (SUL):
					move_straight(1,*orientation,position);
				break;
				case (NORTE):
					turn(ESQUERDA,orientation);
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (LESTE):
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (OESTE):
					turn(DIREITA,orientation);
					move_straight(1,*orientation,position);
				break;
				default:
				break;
			}
		break;

		case (LESTE):
			switch(node_orientation){
				case (LESTE):
					move_straight(1,*orientation,position);
				break;
				case (OESTE):
					turn(ESQUERDA,orientation);
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (NORTE):
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (SUL):
					turn(DIREITA,orientation);
					move_straight(1,*orientation,position);
				break;
				default:
				break;
			}
		break;

		case (OESTE):
			switch(node_orientation){
				case (OESTE):
					move_straight(1,*orientation,position);
				break;
				case (LESTE):
					turn(ESQUERDA,orientation);
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (SUL):
					turn(ESQUERDA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (NORTE):
					turn(DIREITA,orientation);
					move_straight(1,*orientation,position);
				break;
				default:
				break;
			}
		break;

		default:
		break;
	}
}

bool move_to_next_node(int *grid, int *position, int *orientation, list *to_go, list *stacks){
	list_n dest_node;
	list_n node_to_move;
	list_n node_to_push;
	list list_to_push;
	int stack_to_push;

	stack_to_push = 0;

	if(!pop(to_go, dest_node)){ // Has node to move to.
		return false;
	}
	if(isAdjacent(dest_node.x, dest_node.y, position[0], position[1])){
		node_to_push.x = position[0];
		node_to_push.y = position[1];
		list_to_push = stacks[stack_to_push];
		push(list_to_push ,node_to_push);
		move_to_adjacent(grid, position, orientation, node_to_push);
		return true;
	}

	return false;
}