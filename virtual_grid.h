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
#define TURN_TIME 89
#define CELL_SIZE 675 // Cell size: 30. Angular distance: 30*20,9 = 627

bool isAdjacent(int x1, int y1, int x2, int y2){
	if((abs(x1-x2) + abs(y1 - y2)) == 1)
		return true;
	else
		return false;
}

void print_path(char *grid){
	int i,j;
	int x,y;
	for(i=GRID_NROWS - 1;i>=0;i--){
		for(j=0;j<GRID_NCOLS;j++){
			x = grid[GRID_NCOLS*i + j]%GRID_NCOLS;
			y = (grid[GRID_NCOLS*i + j] - x)/GRID_NCOLS;
			writeDebugStream("%d,%d \t",x,y );
		}
		writeDebugStreamLine("");
	}

}

void print_grid(char *grid){
	int i,j;

	for(i=GRID_NROWS - 1;i>=0;i--){
		for(j=0;j<GRID_NCOLS;j++){
			writeDebugStream("%c\t", grid[GRID_NCOLS*i + j]);
		}
		writeDebugStreamLine("");
	}

}

bool set_grid(char *grid, int x, int y, char value){
	if(x >=0 && y >=0 && x < GRID_NCOLS && y < GRID_NROWS){
		grid[GRID_NCOLS*y + x] = value;
		return true;
	}
	return false;
}

bool update_non_visited(char *grid, int x, int y, char value){
	if(grid[GRID_NCOLS*y + x] == '0')
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
			motor[leftMotor] = FORWARDSPEED;
			motor[rightMotor] = FORWARDSPEED;
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
	resetGyro(gyroSensor);

	if(direction == DIREITA){
		do{
			motor[leftMotor] = FORWARDSPEED;
			motor[rightMotor] = -FORWARDSPEED;
			error = abs(SensorValue[colorSensor] - TARGETCOLOR);
			distance = getGyroDegrees(gyroSensor);
		}while(distance <= TURN_TIME);

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
			distance = getGyroDegrees(gyroSensor);
		}while(distance >= -TURN_TIME);

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

void check_surroundings(char *grid, int *position, int orientation, list *nodes_to_visit){
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
			if (update_non_visited(grid, position[0] - 1,position[1], obstacle[0] ? '3' : '0') && !obstacle[0]){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if (update_non_visited(grid, position[0],position[1] + 1, obstacle[1] ? '3' : '0') && !obstacle[1]){
				new_node.x = position[0];
				new_node.y = position[1] + 1;
				push(nodes_to_visit, new_node);
			}
			if (update_non_visited(grid, position[0] + 1,position[1], obstacle[2] ? '3' : '0') && !obstacle[2]){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
		break;

		case (SUL):
			if(update_non_visited(grid, position[0] + 1,position[1], obstacle[0] ? '3' : '0') && !obstacle[0]){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[1] ? '3' : '0') && !obstacle[1]){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] - 1,position[1], obstacle[2] ? '3' : '0') && !obstacle[2]){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
		break;

		case (LESTE):
			if(update_non_visited(grid, position[0],position[1] + 1, obstacle[0] ? '3' : '0') && !obstacle[0]){
				new_node.x = position[0];
				new_node.y = position[1] + 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] + 1,position[1], obstacle[1] ? '3' : '0') && !obstacle[1]){
				new_node.x = position[0] + 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[2] ? '3' : '0') && !obstacle[2]){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
		break;

		case (OESTE):
			if(update_non_visited(grid, position[0],position[1] - 1, obstacle[0] ? '3' : '0') && !obstacle[0]){
				new_node.x = position[0];
				new_node.y = position[1] - 1;
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0] - 1,position[1], obstacle[1] ? '3' : '0') && !obstacle[1]){
				new_node.x = position[0] - 1;
				new_node.y = position[1];
				push(nodes_to_visit, new_node);
			}
			if(update_non_visited(grid, position[0],position[1] + 1, obstacle[2] ? '3' : '0') && !obstacle[2]){
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

void move_to_adjacent(char *grid, int *position, int *orientation, list_n where_to_move){
	int node_orientation;
	if(position[0] - where_to_move.x > 0)
		node_orientation = OESTE;
	if(position[0] - where_to_move.x < 0)
		node_orientation = LESTE;
	if(position[1] - where_to_move.y > 0)
		node_orientation = SUL;
	if(position[1] - where_to_move.y < 0)
		node_orientation = NORTE;

	switch(*orientation){
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
					turn(DIREITA,orientation);
					move_straight(1,*orientation,position);
				break;
				case (OESTE):
					turn(ESQUERDA,orientation);
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


	
bool move_to_next_node(char *grid, int *position, int *orientation, list *to_go){	
	
	list_n dest_node;	
	list_n node_to_move;
	list_n bsf_node_check, bsf_node_add;
	list bsf;
	char added_on_qeue[GRID_NCOLS * GRID_NROWS];

	int i;	
	char buffer;

	for(i=0;i<GRID_NROWS*GRID_NCOLS;i++)
		added_on_qeue[i] = '0';
	initialize_list(bsf,QEUE);
	if(!pop(to_go, dest_node)) // Has node to move to.
		return false;	
	
	if(isAdjacent(dest_node.x, dest_node.y, position[0], position[1])){
		move_to_adjacent(grid, position, orientation, dest_node);
		return true;
	}else{
		bsf_node_check.x = position[0];
		bsf_node_check.y = position[1];	
		push(bsf ,bsf_node_check);
		do{			
			pop(bsf, bsf_node_check);
			sprintf(&buffer, "%c", GRID_NCOLS*(bsf_node_check.y) + (bsf_node_check.x));			
			if(update_non_visited(added_on_qeue, bsf_node_check.x-1, bsf_node_check.y, buffer) && grid[GRID_NCOLS*(bsf_node_check.y) + (bsf_node_check.x-1)] == '2'){
				bsf_node_add.x = bsf_node_check.x-1;
				bsf_node_add.y = bsf_node_check.y;
				push(bsf ,bsf_node_add);		
			}				
			if(update_non_visited(added_on_qeue, bsf_node_check.x, bsf_node_check.y+1, buffer) && grid[GRID_NCOLS*(bsf_node_check.y+1) + (bsf_node_check.x)] == '2'){
				bsf_node_add.x = bsf_node_check.x;
				bsf_node_add.y = bsf_node_check.y+1;
				push(bsf ,bsf_node_add);
			}
			if(update_non_visited(added_on_qeue, bsf_node_check.x+1, bsf_node_check.y,buffer) && grid[GRID_NCOLS*(bsf_node_check.y) + (bsf_node_check.x+1)] == '2'){
				bsf_node_add.x = bsf_node_check.x+1;
				bsf_node_add.y = bsf_node_check.y;
				push(bsf ,bsf_node_add);								
			}	
			if(update_non_visited(added_on_qeue, bsf_node_check.x, bsf_node_check.y-1, buffer) && grid[GRID_NCOLS*(bsf_node_check.y-1) + (bsf_node_check.x)] == '2'){
				bsf_node_add.x = bsf_node_check.x;
				bsf_node_add.y = bsf_node_check.y-1;
				push(bsf ,bsf_node_add);										
			}											
		}while(!find(bsf,dest_node) && bsf.last_node_pos != -1);
		
		initialize_list(bsf,STACK);
		node_to_move.x = added_on_qeue[GRID_NCOLS*dest_node.y + dest_node.x]%GRID_NCOLS;
		node_to_move.y = (added_on_qeue[GRID_NCOLS*dest_node.y + dest_node.x ] - node_to_move.x)/GRID_NCOLS;			
		push(bsf,dest_node);

		while(!isAdjacent(node_to_move.x,node_to_move.y,position[0], position[1])){
			push(bsf,node_to_move);
			i = node_to_move.x;
			node_to_move.x = added_on_qeue[GRID_NCOLS*node_to_move.y + node_to_move.x]%GRID_NCOLS;
			node_to_move.y = (added_on_qeue[GRID_NCOLS*node_to_move.y + i] - node_to_move.x)/GRID_NCOLS;			
		}
		push(bsf,node_to_move);

		while(bsf.last_node_pos != -1){
			pop(bsf,node_to_move);
			move_to_adjacent(grid, position, orientation, node_to_move);
		}
		return true;
	}

	return false;
}