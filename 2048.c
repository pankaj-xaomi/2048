#include "2048.h"

/******************************************************
 * 		Global Variables	      	      *
 *****************************************************/
int g_board[BOARD_SIZE][BOARD_SIZE];
int g_saved_high_score;
int g_high_score;
int g_exit_game;
int g_finished;
int g_score;
int g_won;

/******************************************************
 * 	Functions for writing on screen       	      *
 *****************************************************/
void print_hborder()
{
    int num_chars = BOARD_SIZE * 6 + (BOARD_SIZE + 1);
    char *buf = calloc (num_chars + 1, 1);
    if (buf == NULL)
    {
	return;
    }
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

void init_screen()
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
}

/******************************************************
 * 	           Game Logic			      *
 *****************************************************/
void start_new_game (int board[][BOARD_SIZE])
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

    /* Reset some global variables */
    g_won = 0;
    g_score = 0;
    g_finished = 0;
}


/******************************************************
 * 	       Game Logic: Key Processing      	      *
 *****************************************************/
int shift_all_left (int *row)
{
    int status = 0;
    int i = 0, j = 0;
    for (j = 0; j < BOARD_SIZE; j++)
    {
	int swapped = 0;
	for (i = BOARD_SIZE-1; i>0; i--)
	{
	    if (row[i] == 0)
		continue;
	    if (row[i-1] == 0)
	    {
		row[i-1] = row[i];
		row[i] = 0;
		status++;
		swapped = 1;
	    }
	}

	/* If no swap happenned, it is already shifted */
	if (swapped == 0)
	    break;
    }

    return status;
}

int shift_all_right (int *row)
{
    int status = 0;
    int i = 0, j = 0;

    for (j = 0; j < BOARD_SIZE; j++)
    {
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


int process_row (int *row, int key)
{
    int i = 0;
    int status = 0;
    if (key == LEFT_KEY)
    {
	//We will go 2 passes through the row
	//1. Merge everything if possible
	//2. Put everything to left if there is any space created because
	//   of merge.
	//Try to merge everything
	for (i=0; i < BOARD_SIZE-1; i++)
	{
	    int tmp = row[i];
	    int tmp_ind = i;
	    if (tmp == 0)
		continue;
	    /* tmp is non zero. Try to find next element which is of same value
	     * which is separated by zero */
	    for (; i < BOARD_SIZE-1; i++)
	    {
		if (row[i+1] == 0)
		    continue;

		if (tmp != row[i+1])
		    break;

		/* tmp == row[i] */
		row[i+1] = 0;
		row[tmp_ind] *= 2;
		g_score += row[tmp_ind];
		status++;
		/* Merge will happen only once for 2 elements */
		break;
	    }
	}
	//Again shift everything in left
	status += shift_all_left (row);
    }
    else
    {
	//We will go 2 passes through the row
	//1. Merge everything if possible
	//2. Put everything to right if there is any space created because
	//   of merge.

	//Try to merge everything
	for (i = BOARD_SIZE-1; i>0; i--)
	{
	    int tmp = row[i];
	    int tmp_ind = i;

	    if (tmp == 0)
		continue;

	    for(; i > 0; i--)
	    {
		if (row[i-1] == 0)
		    continue;

		if (tmp != row[i-1])
		    break;

		row[i-1] = 0;
		row[tmp_ind] *= 2;
		g_score += row[tmp_ind];
		status++;
		/* Merge will happen only once for 2 elements */
		break;
	    }
	}
	status += shift_all_right(row);
    }
    return status;
}

int process_left_right_key(int board[][BOARD_SIZE], int key)
{
    int i = 0;
    int status = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
	status += process_row(board[i], key);
    }

    return status;
}

int process_up_down_key (int board[][BOARD_SIZE], int key)
{
    int row[BOARD_SIZE];
    int status = 0;
    int i,j;

    if (key == UP_KEY)
	key = LEFT_KEY;
    else
	key = RIGHT_KEY;

    /* Idea is to put all elements in the column in an array
     * Then UP key becomes LEFT and DOWN becomes RIGHT
     * After processing copy the row back to column. */
    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    row[j] = board[j][i];
	}
	status += process_row(row, key);
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
	    status = g_finished ? 0 : process_left_right_key(board, key);
	    break;
	case RIGHT_KEY:
	    status = g_finished ? 0 : process_left_right_key(board, key);
	    break;

	case UP_KEY:
	    status = g_finished ? 0 : process_up_down_key(board, key);
	    break;
	case DOWN_KEY:
	    status = g_finished ? 0 : process_up_down_key(board, key);
	    break;

	case CTRL_C_KEY:
	    save_highest_score();
	    g_exit_game = 1;
	    break;

	case NEW_GAME_KEY:
	    save_highest_score ();
	    start_new_game(board);
	    break;

	case SAVE_GAME_KEY:
	    save_game_data(g_board, g_score);
	    break;

	case RESUME_GAME_KEY:
	    load_game_data(g_board, &g_score);
	    break;

	default:
	    break;
    }

    return status;
}

/******************************************************
 * 	       Game Logic: New numbers on board	      *
 *****************************************************/
void add_extra_num(int board[][BOARD_SIZE])
{
    int row, col;
    int val = 2;

    /* Approximately 10% times should be 4 */
    if (rand() % 11 > 9)
	val = 4;

    while (1)
    {
	//TODO: Better random number generator
	row = rand() % BOARD_SIZE;
	col = rand() % BOARD_SIZE;
	if (board[row][col])
	    continue;
	board[row][col] = val;
	break;
    }
}

/******************************************************
 * 	       Game Logic: End Game		      *
 *****************************************************/
int check_if_merge_possible (int *board)
{
    int i = 0;
    for (i = 0; i < BOARD_SIZE-1; i++)
    {
	if (board[i] == 0)
	    continue;

	if (board[i] == board[i+1])
	{
	    return 1;
	}
    }

    return 0;
}

int is_game_finished(int board[][BOARD_SIZE])
{
    //First check if any of cell contains winning score
    int i = 0, j = 0;
    int any_zero = 0;	//Check if there is any 0 in board
    int row[BOARD_SIZE];

    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    if (board[i][j] == WINNING_SCORE)
	    {
		g_won = 1;
		return 1;
	    }

	    if (board[i][j] == 0)
		any_zero = 1;
	}
    }

    /* There is atleast one zero in board. Game can continue */
    if (any_zero)
	return 0;
 
    /* Check if there is any possibility of merge in the board
     * It is basically a check in two adjacent cells are same or not  */

    //Start with rows
    for (i = 0; i < BOARD_SIZE; i++)
    {
	if (check_if_merge_possible(board[i]))
	    return 0;
    }

    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    row[j] = board[j][i];
	}

	if (check_if_merge_possible (row))
	    return 0;
    }

    /* No further moves possible. */
    return 1;

}

/******************************************************
 * 	       Main Entry Point			      *
 *****************************************************/
int main()
{
    int c;
    init_screen();
    srand(getpid());
    g_saved_high_score = g_high_score = get_highest_score();
    start_new_game(g_board);
    
    //Main Game loop
    do
    {
	print_board(g_board);
	printw("\nScore: %8d\t\tHigh Score: %8d\n", g_score, g_high_score);
	if (g_finished)
	{
	    if (g_won)
	    {
		printw("\nYou Won! Congrats :)");
	    }
	    printw("\nGame finished!\n");
	}
	printw("\nHOW TO PLAY: Use your arrow keys to move the tiles. \n"
		"When two tiles with the same number touch, they mergee into one!\n");
	printw("Press 'n' for new Game.\n");
	printw("Press 's' for saving the game\n");
	printw("Press 'r' for resuming the saved game.\n");
	printw("Press 'u' for undo (upto 5 last moves).\n");
	printw("\nPress Ctrl-C to exit any time. \n");
	//printw("\nGot char %d\n", c);
	refresh();
	c = getch();
	if (process_key(g_board, c))
	{
	    add_extra_num(g_board);
	}
	/* Update high score. */
	g_high_score = (g_high_score < g_score) ? g_score : g_high_score;
	g_finished = is_game_finished(g_board);

	erase();
    } while (g_exit_game == 0);

    endwin();

    return 0;
}
