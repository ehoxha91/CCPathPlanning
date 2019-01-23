struct node1
{
	int x;
	int y;
	int px;			//Parent vertex coordinates
	int py;
	int _dr;
	struct node1 *next;
};
typedef struct node1 node1;

struct node1 *top1 = NULL;

int s_x, s_y, prev_x, prev_y, s_dir, s_count = 0;

void stack_push(int _x,int _y, int _drc, int _px, int _py)
{
	node1 *tmp = (struct node1*)malloc(sizeof(node1));
	tmp -> x = _x;
	tmp -> y = _y;
	tmp -> px = _px;
	tmp -> py = _py;
	tmp->_dr = _drc;
	tmp -> next = top1;
	top1 = tmp;
	s_count ++;
}

void stack_pop()
{
	node1 *tmp;
	int n;
	tmp = top1;
	s_x = tmp->x;
	s_y = tmp->y;
	prev_x = tmp->px;
	prev_y = tmp->py;
	s_dir = tmp->_dr;
	top1 = top1->next;
	free(tmp);
	s_count--;
}

int Top1_x()
{
	int __x = top1->x;
	return __x;
}

int Top1_y()
{
	int __y = top1->y;
	return __y;
}

int Top1_px()
{
	return top1->px;
}

int Top1_py()
{
	return top1->px;
}

int Top_dir()
{
	return top1->_dr;
}


int Is1Empty(struct node1* tmp)
{
	return tmp==NULL ? 1 : 0; 
}

