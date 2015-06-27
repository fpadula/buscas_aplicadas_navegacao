#define LIST_SIZE 30
#define QEUE '0'
#define STACK '1'

typedef struct{
	int x;
	int y;
} list_n;

typedef struct{
	char type;
	list_n list_nodes[LIST_SIZE];
	int last_node_pos;
} list;


void initialize_list(list *list, char type){
	int i;

	for(i=0;i<LIST_SIZE;i++){
		list->list_nodes[i].x = -1;
		list->list_nodes[i].y = -1;
	}
	list->last_node_pos = -1;
	list->type = type;
}

bool find(list *list, list_n *node){
	int i;
	for (i=0;i<=list->last_node_pos;i++){
		if((list->list_nodes[i].x == node->x) && (list->list_nodes[i].y == node->y))
			return true;	 
	}
	return false;
}

int pop(list *list, list_n *node){
	int i;

	if(list->type == STACK){
		if(list->last_node_pos > -1){
			node->x = list->list_nodes[list->last_node_pos].x;
			node->y = list->list_nodes[list->last_node_pos].y;
			list->list_nodes[list->last_node_pos].x = -1;
			list->list_nodes[list->last_node_pos].y = -1;
			list->last_node_pos--;
			return 1;
		}else{
			return 0;
		}
	}else if(list->type == QEUE){
		if(list->last_node_pos > -1){
			node->x = list->list_nodes[0].x;
			node->y = list->list_nodes[0].y;
			for (i=0;i<=list->last_node_pos-1;i++){
				list->list_nodes[i].x = list->list_nodes[i+1].x;
				list->list_nodes[i].y = list->list_nodes[i+1].y;
			}
			list->list_nodes[list->last_node_pos].x = -1;
			list->list_nodes[list->last_node_pos].y = -1;
			list->last_node_pos--;
			return 1;
		}else{
			return 0;
		}
	}else
		return 0;
}

int push(list *list, list_n node){
	int i;

	if(list->type == STACK){
		if(list->last_node_pos < LIST_SIZE - 1){
			list->last_node_pos++;
			list->list_nodes[list->last_node_pos].x = node.x;
			list->list_nodes[list->last_node_pos].y = node.y;
			return 1;
		}else{
			return 0;
		}
	}else if(list->type == QEUE){
		if(list->last_node_pos < LIST_SIZE - 1){
			list->last_node_pos++;
			list->list_nodes[list->last_node_pos].x = node.x;
			list->list_nodes[list->last_node_pos].y = node.y;
			return 1;
		}else{
			return 0;
		}
	}else
		return 0;
}