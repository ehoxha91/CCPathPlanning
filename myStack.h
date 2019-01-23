struct node
{
	int x;
	int y;
	int px;			//Parent vertex coordinates
	int py;
	struct node *next;
};
typedef struct node node;

struct node *top = NULL;

int stack_x, stack_y, p_x, p_y, counter = 0;

void push(int _x,int _y, int _px, int _py)
{
	node *tmp = (struct node*)malloc(sizeof(node));
	tmp -> x = _x;
	tmp -> y = _y;
	tmp -> px = _px;
	tmp -> py = _py;
	tmp -> next = top;
	top = tmp;
	//printf("\nPushed: [x,y] = [%d,%d]", stack_x, stack_y);
	counter ++;
}

void pop()
{
	node *tmp;
	int n;
	tmp = top;
	stack_x = tmp->x;
	stack_y = tmp->y;
	p_x = tmp->px;
	p_y = tmp->py;
	top = top->next;
	free(tmp);
	counter--;
}

int Top_x()
{
	int __x = top->x;
	return __x;
}

int Top_y()
{
	int __y = top->y;
	return __y;
}

int Top_px()
{
	return top->px;
}

int Top_py()
{
	return top->px;
}


int IsEmpty(struct node* tmp)
{
	return tmp==NULL ? 1 : 0; 
}

