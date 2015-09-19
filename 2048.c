#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define BOARD_SIZE 	4

#define LEFT_KEY	260
#define RIGHT_KEY	261
#define UP_KEY		259
#define DOWN_KEY	258
#define CTRL_C		3
#define NEW_GAME	110

int g_board[BOARD_SIZE][BOARD_SIZE];
int g_score;

void print_hborder()
{
    int num_chars = BOARD_SIZE * 6 + (BOARD_SIZE + 1);
    char *buf = calloc (num_chars + 1, 1);
    //TODO Handle NULL pointer case
    memset(buf, '-', num_chars);
    printw ("%s\n", buf);
    free(buf);
}

void print_row(int *row)
{
    int i = 0;

    printw("|");
    for (i = 0; i < BOARD_SIZE; i++)
    {
	if (row[i])
	{
	    printw(" %4d |", row[i]);
	}
	else
	{
	    printw("      |");
	}
    }
    printw("\n");
}

void print_board(int board[][BOARD_SIZE])
{
    int i = 0;
    print_hborder();
    for (i = 0; i < BOARD_SIZE; i++)
    {
	print_row(board[i]);
	print_hborder();
    }
    refresh();
}

void start_new_board (int board[][BOARD_SIZE])
{
    int i = 0, j = 0;
    int row, col;

    /* First zero fill the board */
    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    board[i][j] = 0; 
	}
    }

    row = rand() % BOARD_SIZE;
    col = rand() % BOARD_SIZE;
    board[row][col] = 2;

    while (1)
    {
	row = rand() % BOARD_SIZE;
	col = rand() % BOARD_SIZE;
	if (board[row][col])
	    continue;
	board[row][col] = 2;
	break;
    }
}

void init_screen()
{
    WINDOW *win = initscr();
    raw();
    keypad(stdscr, TRUE);
    srand(getpid());
}

int process_row (int *row, int key)
{
    int i = 0;
    int status = 0;
    if (key == LEFT_KEY)
    {
	//We will go 2 passes through the row

	//First pass: Try to merge as much as possible
	for (i = 0; i < BOARD_SIZE-1; i++)
	{
	    if (row[i] == 0)
		continue;

	    if (row[i+1] == 0)
	    {
		row[i+1] = row[i];
		row[i] = 0;
	    }
	    else if (row[i+1] == row[i])
	    {
		row[i] *= 2;
		row[i+1] = 0;
		g_score += row[i];
		status++;
		//Merge must happen only once for tuples.
		i++;
	    }
	}

	//Second pass: Fill the void created because of merge
	for (i = BOARD_SIZE-1; i > 0; i--)
	{
	    if (row[i] == 0)
		continue;

	    if (row[i-1] == 0)
	    {
		row[i-1] = row[i];
		row[i] = 0;
		status++;
	    }
	}
    }
    else
    {
	//TODO: Look for an elegent solution to handle left/right key in same
	//function/loop
	
	//We will go 2 passes through the row

	//First pass: Try to merge&shift as much as possible
	for (i = BOARD_SIZE-1; i > 0; i--)
	{
	    if (row[i] == 0)
		continue;

	    if (row[i-1] == 0)
	    {
		row[i-1] = row[i];
		row[i] = 0;
	    }
	    else if (row[i-1] == row[i])
	    {
		row[i] *= 2;
		row[i-1] = 0;
		g_score += row[i];
		status++;
		//Merge must happen only once for tuples.
		i--;
	    }
	}

	//Second pass: Fill the void created because of merge
	for (i = 0; i < BOARD_SIZE-1; i++)
	{
	    if (row[i] == 0)
		continue;

	    if (row[i+1] == 0)
	    {
		row[i+1] = row[i];
		row[i] = 0;
		status++;
	    }
	}
    }
    return status;
}

int process_left_key(int board[][BOARD_SIZE])
{
    int i = 0;
    int status = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
	status += process_row(board[i], LEFT_KEY);
    }

    return status;
}

int process_right_key(int board[][BOARD_SIZE])
{
    int i = 0;
    int status = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
	status += process_row(board[i], RIGHT_KEY);
    }
    return status;
}

int process_up_key (int board[][BOARD_SIZE])
{
    int row[BOARD_SIZE];
    int status = 0;
    int i,j;

    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    row[j] = board[j][i];
	}
	status += process_row(row, LEFT_KEY);
	if (status)
	{
	    for (j = 0; j < BOARD_SIZE; j++)
	    {
		board[j][i] = row[j];
	    }
	}
    }

    return status;
}

int process_down_key(int board[][BOARD_SIZE])
{
    int row[BOARD_SIZE];
    int status = 0;
    int i,j;

    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    row[j] = board[j][i];
	}
	status += process_row(row, RIGHT_KEY);
	if (status)
	{
	    for (j = 0; j < BOARD_SIZE; j++)
	    {
		board[j][i] = row[j];
	    }
	}
    }

    return status;
}

int process_key (int board[][BOARD_SIZE], int key)
{
    int status = 0;
    switch (key)
    {
	case LEFT_KEY:
	    status = process_left_key(board);
	    break;
	case RIGHT_KEY:
	    status = process_right_key(board);
	    break;

	case UP_KEY:
	    status = process_up_key(board);
	    break;

	case DOWN_KEY:
	    status = process_down_key(board);
	    break;

	default:
	    break;
    }

    return status;
}

void add_extra_num(int board[][BOARD_SIZE])
{
    int row, col;
    while (1)
    {
	//TODO: Better random number generator
	row = rand() % BOARD_SIZE;
	col = rand() % BOARD_SIZE;
	if (board[row][col])
	    continue;
	board[row][col] = 2;
	break;
    }
}

int is_game_finished(int board[][BOARD_SIZE])
{
    return 0;
}

int main()
{
    int c;
    int finished = 0;

    init_screen();
    start_new_board(g_board);
    

    //Main Game loop
    do
    {
	print_board(g_board);
	printw("\nScore: %d\n", g_score);
	if (finished)
	{
	    printw("\nGame finished!\n");
	}
	//printw("\nChar is %d\n", c);
	printw("\nHOW TO PLAY: Use your arrow keys to move the tiles. \n"
		"When two tiles with the same number touch, they mergee into one!\n");
	printw("\nPress Ctrl-C to exit any time. Press 'n' for new Gameme\n");
	refresh();
	c = getch();
	if (c == CTRL_C)
	    break;

	if (c == NEW_GAME)
	{
	    finished = 0;
	    g_score = 0;
	    start_new_board(g_board);
	}

	if (!finished)
	{
	    if (process_key (g_board, c))
	    {
		add_extra_num (g_board);
	    }
	    finished = is_game_finished(g_board);
	}
	erase();
    } while (1);

    endwin();
    //printf ("Got char %x \n", c);

    return 0;
}
