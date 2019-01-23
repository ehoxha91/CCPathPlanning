#define XSIZE 100
#define YSIZE 100
#define STARTX 2
#define STARTY 96
#define OPEN 1
#define BLOCKED 0

static int board[XSIZE][YSIZE];
static int visit[XSIZE][YSIZE];
static int posx, posy, dir, movecount, turncount;

void PrintBoard();

void start()
{
	int i, j;
	/* initialize direction, start position */
	dir = 0;    posx = STARTX;    posy = STARTY;
	movecount = 0; turncount = 0;
	/* fill board */
	for (i = 0; i<XSIZE; i++)
	{
		for (j = 0; j< YSIZE; j++)
		{
			board[i][j] = OPEN;
			visit[i][j] = 0;
		}
	}
	/* block boundary */
	for (i = 0; i< XSIZE; i++)
	{
		board[i][0] = BLOCKED;
		board[i][YSIZE - 1] = BLOCKED;
	}
	for (j = 0; j< YSIZE; j++)
	{
		board[0][j] = BLOCKED;
		board[XSIZE - 1][j] = BLOCKED;
	}
	/* now put in some obstacles */
	for (i = 3; i< XSIZE; i++)
		board[i][YSIZE - 4] = BLOCKED;
	for (i = 0; i< XSIZE - 2; i++)
		board[i][YSIZE - 6] = BLOCKED;
	for (i = 3; i< XSIZE - 5; i++)
		board[i][YSIZE - 8] = BLOCKED;
	for (i = 4; i<7; i++)
		for (j = 3; j<7; j++)
			board[i][j] = BLOCKED;
	for (int g = 15; g<92; g++)
		board[18][g] = BLOCKED;
	for (int g = 15; g<60; g++)
		board[40][g] = BLOCKED;
	for (int n = 2; n<99; n++)
		board[n][33] = BLOCKED; 
}


int move_forward()
{  movecount +=1;
   switch( dir )
   {  case 0:
		 visit[posx+1][posy] +=1;
		 if(board[posx+1][posy] == OPEN)
	 {  posx = posx+1; return(1); }
		 else
	   return(0); 
		 break; 
	  case 1:
		 visit[posx][posy+1] +=1;
		 if(board[posx][posy+1] == OPEN)
	 {  posy = posy+1; return(1); }
		 else
	   return(0); 
		 break;
	  case 2:
		 visit[posx-1][posy] +=1;
		 if(board[posx-1][posy] == OPEN)
	 {  posx = posx-1; return(1); }
		 else
	   return(0);
		 break; 
	  case 3:
		 visit[posx][posy-1] +=1;
		 if(board[posx][posy-1] == OPEN)
	 {  posy = posy-1; return(1); }
		 else
	   return(0);
		 break; 
	  default: 
	 printf("something wrong; direction value %d\n",dir); 
		 //fflush(stdio); 
	 exit(0);
   }

}

void turn_left()
{  dir = (dir+1)%4; turncount +=1;
}

void turn_right()
{  
	dir = (dir+3)%4; turncount +=1; 
}

void end()
{
	int i, j, as =0;
   if( posx != STARTX || posy != STARTY )
	 printf("Did not return to start position\n");  
   int rev = 0;
   for(i=0; i<XSIZE; i++)
   {  for( j=0; j< YSIZE; j++)
	  { if( board[i][j]== BLOCKED && visit[i][j] >1 )
		{ 
		   printf("revisit of blocked cell found at %d,%d\n", i, j);
		   rev++;
		}
	  }
   }
   for(i=0; i<XSIZE; i++)
   {  for( j=0; j< YSIZE; j++)
   {
	   if (board[i][j] == OPEN && visit[i][j] < 1)
	   {
		   printf("unvisited open cell found at %d,%d\n", i, j);
		   as++;
	   }
	  }
   }
   printf("\nUnvisited Cells: %d", as);
   printf("\nRevisited Cells: %d", rev);
   printf("\nMoves: %d", movecount);
   printf("\nTurns: %d", turncount);
}

