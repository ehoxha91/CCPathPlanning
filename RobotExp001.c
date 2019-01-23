/* gcc -o moveit RobotExp001.c /usr/lib/libXbgi.a -lX11 -lm */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "myStack.h"		/* Stack for DFS */
#include "Frame002.h"
#include "xydStack.h"		/* Stack for backtrack. */
#include <string.h>
#include "../graphics.h"

#define _XSIZE 202		/* Since professors said we have 100x100 graphs. */
#define _YSIZE 202

#define NOTVISITED 0		/* Node not visited. */
#define VISITED 1		/* Node visited. */
#define EXPLORED 2		/* Node/vertex explored(inside the stack). */
#define FINISHED 3		/* When node and all it's neighbours are visited. */

int v_frontier[_XSIZE][_YSIZE];	/* We will keep track of visited vertices on this vector. */
int v_status[_XSIZE][_YSIZE];	/* We will save the status of the vertex with coordinates[x][y] = 0 - BLOCKED, 1 - OPEN. */
int x, y; 			/* Variables used for coordinates, as this problem is plannar graph exploring. */
int _direction; 		/* Increasing Y (direction = 0). Increasing X (direction = 1). Decreasing Y(direction = 2). Decreasing X (direction = 3).*/
int moved = 1;			/* when moved=2 it means that we need to calculate direction for next movement since we already visited the actual node*/
static int runStatus;		/* Search Running = 1, Stopped = 0. */
int pr_x, pr_y;			/* Previous coordinates. */

void initialize_status();	/* Initialize matrices for search. */


void RobotExplore();						/* Algorithm.*/

void mark_node(int _status, int _access, int _mov);		/* Mark the visited/explored/finished vertex/node with access and status. */
int isNodeFinished(int _x, int _y);				/* Check if the node is finished. */
void GenerateNeighbours(int _x, int _y);			/* Generate neighbour nodes and put them on stack. */
void xp_node(int _x1, int _y1);					/* Take node on the x-positive direction and put it on the stack. */
void xn_node(int _x1, int _y1);					/* Take node on the x-negative direction and put it on the stack. */
void yp_node(int _x1, int _y1);					/* Take node on the y-positive direction and put it on the stack. */
void yn_node(int _x1, int _y1);					/* Take node on the y-negative direction and put it on the stack. */

int GoBackOneStep();						/* Go one step back and let the algorithm check next step from that vertex. */
int CalculateDistance(int _x, int _y, int _stack_x, int _stack_y);	/* We calculate the distance from two vertices.*/
void CalculateMovement();					/* When we move forward, we have to calculate the new position.*/
void CalculateNextDirection_B();				/* Set the direction using the item last node out of stack.
												This function must be called only when the last node visited was blocked
												and next node is only a step away from the current position(moved == 2). */

void CalculateNextDirection();					/* Even when we successfully moved, we need to do calculation about next movement, 									   because we're taking next pos. from stack*/

int CheckNeighbours(int _x, int _y);				/* When our next node is far we first check if we have some unvisited/unfinished
													nodes in neighbourhood. Retruns 1 if we have.*/

int CheckF(int _x, int _y, int _checktype);	/*This function is used from CheckNeighbours() to check if neighbours are all finished.*/
int CheckV(int _x, int _y, int _checktype);	/*This function is used from CheckNeighbours() to check if neighbours are all finished and visited.*/

int c, gd, gm, dx, dy;
void initialize_drawing();
void draw_node(int _x, int _y, int COLOR);

int main()
{
	start();						/* Prepare graph, make it ready to be explored. */
	runStatus = 1;					/* Run Mode. */
	initialize_status();
	initialize_drawing();
	RobotExplore();					/* Start exploring. */
	return 0;
}

void initialize_status()
{
	for (size_t i = 0; i < _XSIZE; i++)
	{
		for (size_t j = 0; j < _YSIZE; j++)
		{
			v_status[i][j] = 3;
			v_frontier[i][j] = NOTVISITED;
		}
	}
}

void initialize_drawing()
{
	gd = X11;
	gm = X11_1152x900;
	initgraph (&gd, &gm, "");
	setbkcolor (BLACK);
	setcolor (YELLOW);
	cleardevice ();
	dx = 300;
	dy = 770;
	outtextxy (0, 0, "ROBOT EXPLORING");
	setcolor (RED);
	circle (dx, dy, 4);

}


void draw_node(int _x, int _y, int COLOR)
{

if(v_status[x][y] != BLOCKED)
{
	switch (_direction) 
	{
	    case 0:
	      dx += 8;
	      break;
	    case 1:
	      dy -= 8;
	      break;
	    case 2:
	      dx -= 8;
	      break;
	    case 3:
	      dy += 8;
	      break;    
	    default:
	      break;
	} 
	setcolor (RED);
	circle (dx, dy, 4);
	delay (1);
	setcolor (COLOR);
	circle (dx, dy, 4);
	
    	if (dx < 0)
     	 dx = 0;
    	if (dx > getmaxx ())
   	  dx = getmaxx();
    	if (dy < 0)
    	  dy = 0;
    	if (dy > getmaxy ())
     	 dy = getmaxy();
	
}

}


int _moveBlocked = 0;

void RobotExplore()
{
	_direction = 0;						/* We start moving towards y coordinate. */
	x = 101, y = 101;					/* Local starting position(center of search). Prof said graph is max 100x100 grid.*/

	v_frontier[x][y] = VISITED;			/* Mark the first vertex as visited. */
	v_status[x][y] = OPEN;				/* Mark the first vertex as OPEN. */
	GenerateNeighbours(x, y);			/* Generate neighbours of the starting position. */
	stack_push(x, y, _direction, x, y);
	while (runStatus)
	{

		if (CalculateDistance(x, y, Top_x(), Top_y()) == 1)	/* If the distance of the current node is more than 1 unit away from the [stack_x, stack_y] */
		{
			moved = 4;
			_moveBlocked = 0;
			if (CheckNeighbours(x, y) == 1)			/* Check if we have un-finished nodes on neighbourhood*/
			{
				pr_x = x;
				pr_y = y;
				CalculateMovement();				/* We just moved into a new position to an un-finished node, so we have to calculate new coordinate [x, y].*/
				moved = move_forward();				/* Move Forward. */
				if (moved == 1)
				{
					draw_node(x,y, WHITE);	
					stack_push(x, y, _direction, pr_x, pr_y);	/* On this stack we will keep the move order.*/
					mark_node(VISITED, OPEN, 1);
					GenerateNeighbours(x, y);		/* If we do not have explored all the neighbours of this vertex, explore them.*/
				}
				if (moved == 0)
				{
					mark_node(FINISHED, BLOCKED, 1);
					x = pr_x;	/* If we didn't move mark the node and return coordinates back to the real position.*/
					y = pr_y;
				}
			}
			else
			{
				if (GoBackOneStep() == 1)
				{
					CalculateMovement();
					draw_node(x,y, WHITE);	
				}
				/* if we are here it means that we have no not visited and not finished node
				in neighbourhood and we should travel to the node that is popped out of
				the stack = [stack_x][stack_y]*/
			}
		}

		else
		{		
			pop();						/* Pop the node from stack. [stack_x, stack_y]. */
			if (v_frontier[stack_x][stack_y] != FINISHED && v_status[stack_x][stack_y] == 3)
			{
				if (moved == 1)			/* If last time we moved successfully*/
				{
					/* There are chances that we will visit the node before the stack will pop it.*/
					if (v_frontier[stack_x][stack_y] != VISITED && v_status[stack_x, stack_y] != BLOCKED)
					{
						CalculateNextDirection();		/*Sets the direction when the top element of the stack is not in the actual direction.*/
						moved = move_forward();
					}

				}

				else if (_moveBlocked == 1)					/* If last attempt to move was not successful but we still have some neighbour nodes to visit.*/
				{
					CalculateNextDirection_B();
					moved = move_forward();
					_moveBlocked = 0;
				}

				if (moved == 1)								/* If it moved it means we are on an another position now, and node on the front is not BLOCKED */
				{
					draw_node(x,y, WHITE);	
					stack_push(stack_x, stack_y, _direction, x, y);
					x = stack_x;							/* We moved in the direction of the node out of stack and we succeded.*/
					y = stack_y;
					mark_node(VISITED, OPEN, moved);		/* Mark the node we arrived as VISITED and OPEN. */
					GenerateNeighbours(x, y);				/* Generate neighbour nodes of the current position */
				}
				else if (moved == 0)						/* If moving forward was blocked! */
				{
					mark_node(FINISHED, BLOCKED, moved);	/* Mark the first blocked node, with stack coordinates. */
					_moveBlocked = 1;						/* Next loop tell the program that we couldn't move forward. */
				}
			}
		}

		if (counter == 0 || s_count ==0)
		{
			runStatus = 0;
			if (x == 101 && y == 101)
			{
				//end();
				//printf("\nFinal position: [%d,%d] \n\n", x, y);
				outtextxy (100, 100, "MAZE EXPLORED");
			}
			else
			{
				while (x != 101 || y != 101)
				{
					GoBackOneStep();
					CalculateMovement();
				}
				//end();
				//printf("\nFinal position: [%d,%d] \n\n", x, y);
				outtextxy (100, 100, "MAZE EXPLORED");

			}
			delay(10000);
			return;
		}
	}
}

void mark_node(int _status, int _access, int _mov)
{
	if (_mov == 1)
	{
		v_frontier[x][y] = _status;		/* This point is visited if we are here. */
		v_status[x][y] = _access; 		/* Set status of the visited node. */
		isNodeFinished(x, y);			/* Check if we finished this node. */
	}
	else
	{
		v_frontier[stack_x][stack_y] = _status;		/* This point is visited if we are here. */
		v_status[stack_x][stack_y] = _access; 		/* Set status of the visited node. */
	}
}

int isNodeFinished(int _x, int _y)
{
	if (v_status[_x][_y] == BLOCKED)
	{
		v_frontier[_x][_y] = FINISHED;
		return 1;
	}

	int _vis = 0;
	if (v_frontier[_x][_y + 1] == VISITED || v_frontier[_x][_y+1] == FINISHED)
	{
		if (v_frontier[_x + 1][_y] == VISITED || v_frontier[_x+1][_y] == FINISHED)
		{
			if (v_frontier[_x][_y - 1] == VISITED || v_frontier[_x][_y - 1] == FINISHED)
			{
				if (v_frontier[_x - 1][_y] == VISITED || v_frontier[_x-1][_y] == FINISHED)
				{
					v_frontier[_x][_y] = FINISHED;	/* Mark this node as finished.*/
					return 1;
				}
			}
		}
	}

	return 0;		/* Not finished.*/
}

void CalculateNextDirection_B()
{
	if (v_status[stack_x][stack_y] != BLOCKED)
	{
		switch (_direction)
		{
		case 0:
		{
			if (x > stack_x)
			{
				turn_left();
				_direction = 3;
				return;
			}
			else if (x < stack_x)
			{
				turn_right();
				_direction = 1;
				return;
			}
			else if (y < stack_y)
			{
				return;
			}
			else if (y > stack_y)
			{
				turn_right();
				turn_right();
				_direction = 2;
				return;
			}
		}
		break;
		case 1:
		{
			if (x < stack_x)
			{
				return;
			}
			else if (x > stack_x)
			{
				turn_left();
				turn_left();
				_direction = 3;
				return;
			}
			else if (y > stack_y)
			{
				turn_right();
				_direction = 2;
				return;
			}
			else if (y < stack_y)
			{
				turn_left();
				_direction = 0;
				return;
			}
		}
		break;
		case 2:
		{
			if (x < stack_x)
			{
				turn_left();
				_direction = 1;
				return;
			}
			else if (x > stack_x)
			{
				turn_right();
				_direction = 3;
				return;
			}
			else if (y > stack_y)
			{
				return;
			}
			else if (y < stack_y)
			{
				turn_right();
				turn_right();
				_direction = 0;
				return;
			}
		}
		break;
		case 3:
		{
			if (x < stack_x)
			{
				turn_right();
				turn_right();
				_direction = 1;
				return;
			}
			else if (x > stack_x)
			{
				return;
			}
			else if (y > stack_y)
			{
				turn_left();
				_direction = 2;
				return;
			}
			else if (y < stack_y)
			{
				turn_right();
				_direction = 0;
				return;
			}
		}
		break;
		default:
			break;
		}
	}

}

void CalculateNextDirection()
{
	if (v_status[stack_x][stack_y] != BLOCKED)
	{
		switch (_direction)
		{
		case 0:
		{
			if (x < stack_x)
			{
				turn_right();
				_direction = 1;
				return;
			}
			else if (x > stack_x)
			{
				turn_left();
				_direction = 3;
				return;
			}
			else if (y < stack_y)
			{
				_direction = 0;
				return;
			}
			else if (y > stack_y)
			{
				turn_right();
				turn_right();
				_direction = 2;
				return;
			}
		}
		break;
		case 1:
		{
			if (x < stack_x)
			{
				return;
			}
			else if (x > stack_x)
			{
				turn_right();
				turn_right();
				_direction = 3;
				return;
			}
			else if (y > stack_y)
			{
				turn_right();
				_direction = 2;
				return;
			}
			else if (y < stack_y)
			{
				turn_left();
				_direction = 0;
				return;
			}
		}
		break;
		case 2:
		{
			if (x < stack_x)
			{
				turn_left();
				_direction = 1;
				return;
			}
			else if (x > stack_x)
			{
				turn_right();
				_direction = 3;
				return;
			}
			else if (y > stack_y)
			{
				return;
			}
			else if (y < stack_y)
			{
				turn_left();
				turn_left();
				_direction = 0;
				return;
			}
		}
		break;
		case 3:
		{
			if (x < stack_x)
			{
				turn_right();
				turn_right();
				_direction = 1;
				return;
			}
			else if (x >stack_x)
			{
				return;
			}
			else if (y > stack_y)
			{
				turn_left();
				_direction = 2;
				return;
			}
			else if (y < stack_y)
			{
				turn_right();
				_direction = 0;
				return;
			}
		}
		break;
		default:
			break;
		}
	}

}

int GoBackOneStep()
{
	stack_pop();
	switch (_direction)
	{
	case 0:
		switch (s_dir)
		{
		case 0:
			turn_left();
			turn_left();
			_direction = 2;
			break;
		case 1:
			turn_left();
			_direction = 3;
			break;
		case 2:
			_direction = 0;
			break;
		case 3:
			turn_right();
			_direction = 1;
			break;
		default:
			break;
		}
		break;
	case 1:
		switch (s_dir)
		{
		case 0:
			turn_right();
			_direction = 2;
			break;
		case 1:
			turn_left();
			turn_left();
			_direction = 3;
			break;
		case 2:
			turn_left();
			_direction = 0;
			break;
		case 3:
			_direction = 1;
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (s_dir)
		{
		case 0:
			_direction = 2;
			break;
		case 1:
			turn_right();
			_direction = 3;
			break;
		case 2:
			turn_left();
			turn_left();
			_direction = 0;
			break;
		case 3:
			turn_left();
			_direction = 1;
			break;
		default:
			break;
		}
		break;
	case 3:
		switch (s_dir)
		{
		case 0:
			turn_left();
			_direction = 2;
			break;
		case 1:
			_direction = 3;
			break;
		case 2:
			turn_right();
			_direction = 0;
			break;
		case 3:
			turn_left();
			turn_left();
			_direction = 1;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return move_forward();
}

int CheckNeighbours(int _x, int _y)
{
	if (isNodeFinished(_x, _y) == 1)
	{
		return CheckF(_x, _y, FINISHED);
	}
	else /* If we have explored nodes in the neighbourhood. */
	{
		return CheckV(_x, _y, VISITED);
	}
}

int CheckV(int _x, int _y, int _checktype)
{
	switch (_direction)
	{
	case 0:
	{
		if (v_frontier[_x][_y + 1] != _checktype && v_frontier[_x][_y + 1] != FINISHED)
		{
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype && v_frontier[_x + 1][_y] != FINISHED)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 1;
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype && v_frontier[_x - 1][_y] != FINISHED) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 3;
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype && v_frontier[_x][_y - 1] != FINISHED) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 2;
			return 1;
		}
	}
	break;
	case 1:
	{
		if (v_frontier[_x + 1][_y] != _checktype && v_frontier[_x + 1][_y] != FINISHED)
		{
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype && v_frontier[_x][_y - 1] != FINISHED)  /* Did we finish/visit etc. node on the right before?*/
		{
			turn_right();
			_direction = 2;
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype && v_frontier[_x][_y + 1] != FINISHED) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 0;
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype && v_frontier[_x - 1][_y] != FINISHED) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 3;
			return 1;
		}
	}
	break;
	case 2:
	{
		if (v_frontier[_x][_y - 1] != _checktype && v_frontier[_x][_y - 1] != FINISHED)
		{
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype && v_frontier[_x - 1][_y] != FINISHED)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 3;
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype && v_frontier[_x + 1][_y] != FINISHED) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 1;
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype && v_frontier[_x][_y + 1] != FINISHED) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 0;
			return 1;
		}
	}
	break;
	case 3:
	{
		if (v_frontier[_x - 1][_y] != _checktype && v_frontier[_x - 1][_y] != FINISHED)
		{
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype && v_frontier[_x][_y + 1] != FINISHED)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 0;
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype && v_frontier[_x][_y - 1] != FINISHED) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 2;
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype && v_frontier[_x + 1][_y] != FINISHED) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 1;
			return 1;
		}
	}
	break;
	default:
		break;
	}
	return 0;
}

int CheckF(int _x, int _y, int _checktype)
{
	switch (_direction)
	{
	case 0:
	{
		if (v_frontier[_x][_y + 1] != _checktype)
		{
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 1;
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 3;
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 2;
			return 1;
		}
	}
	break;
	case 1:
	{
		if (v_frontier[_x + 1][_y] != _checktype)
		{
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype)  /* Did we finish/visit etc. node on the right before?*/
		{
			turn_right();
			_direction = 2;
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 0;
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 3;
			return 1;
		}
	}
	break;
	case 2:
	{
		if (v_frontier[_x][_y - 1] != _checktype)
		{
			return 1;
		}
		else if (v_frontier[_x - 1][_y] != _checktype)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 3;
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 1;
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 0;
			return 1;
		}
	}
	break;
	case 3:
	{
		if (v_frontier[_x - 1][_y] != _checktype)
		{
			return 1;
		}
		else if (v_frontier[_x][_y + 1] != _checktype)  /* Did we finish node on the right before?*/
		{
			turn_right();
			_direction = 0;
			return 1;
		}
		else if (v_frontier[_x][_y - 1] != _checktype) /* Did we finish node on the left? */
		{
			turn_left();
			_direction = 2;
			return 1;
		}
		else if (v_frontier[_x + 1][_y] != _checktype) /* Did we finish node on the back? */
		{
			turn_right();
			turn_right();
			_direction = 1;
			return 1;
		}
	}
	break;
	default:
		break;
	}
	return 0;
}

void CalculateMovement()
{
	switch (_direction)
	{
	case 0:
		y = y + 1;
		break;
	case 1:
		x = x + 1;
		break;
	case 2:
		y = y - 1;
		break;
	case 3:
		x = x - 1;
		break;
	default:
		break;
	}
}

int CalculateDistance(int _x, int _y, int _stack_x, int _stack_y)
{
	double x2_x1 = _stack_x - _x;
	x2_x1 = pow(x2_x1, 2);
	double y2_y1 = _stack_y - _y;
	y2_y1 = pow(y2_y1, 2);
	float distance = sqrt(x2_x1 + y2_y1);

	if (distance > 1.0)
		return 1;	/* It means we have to find the road from current node to the next node because it's not in the neighbourhood. */
	else
		return 0;	/* Calculate the next movement using the other function, since we have the next node in the neighbourhood. */

}

void GenerateNeighbours(int _x1, int _y1)
{
	/* On stack: UP, RIGHT, DOWN, LEFT */
	switch (_direction)
	{
	case 0:
	{
		xn_node(_x1, _y1);
		yn_node(_x1, _y1);
		xp_node(_x1, _y1);
		yp_node(_x1, _y1);
	}
	break;
	case 1:
	{
		yp_node(_x1, _y1);
		xn_node(_x1, _y1);
		yn_node(_x1, _y1);
		xp_node(_x1, _y1);
	}
	break;
	case 2:
	{
		xp_node(_x1, _y1);
		yp_node(_x1, _y1);
		xn_node(_x1, _y1);
		yn_node(_x1, _y1);
	}
	break;
	case 3:
	{
		yn_node(_x1, _y1);
		xp_node(_x1, _y1);
		yp_node(_x1, _y1);
		xn_node(_x1, _y1);
	}
	break;
	default:
		break;
	}
}
void xp_node(int _x1, int _y1)
{
	if (v_frontier[_x1 + 1][_y1] == NOTVISITED)
	{
		v_frontier[_x1 + 1][_y1] = EXPLORED;  /* What if we take out this? Discover faster but it may doubles in stack. */
		push(_x1 + 1, _y1, _x1, _y1);
	}
}
void xn_node(int _x1, int _y1)
{
	if (v_frontier[_x1 - 1][_y1] == NOTVISITED)
	{
		v_frontier[_x1 - 1][_y1] = EXPLORED;
		push(_x1 - 1, _y1, _x1, _y1);
	}
}
void yp_node(int _x1, int _y1)
{
	if (v_frontier[_x1][_y1 + 1] == NOTVISITED)
	{
		v_frontier[_x1][_y1 + 1] = EXPLORED;
		push(_x1, _y1 + 1, _x1, _y1);
	}
}
void yn_node(int _x1, int _y1)
{
	if (v_frontier[_x1][_y1 - 1] == NOTVISITED)
	{
		v_frontier[_x1][_y1 - 1] = EXPLORED;
		push(_x1, _y1 - 1, _x1, _y1);
	}
}
