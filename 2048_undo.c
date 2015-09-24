/******************************************************
 *
 * This file contains functions for undoing game.
 *
******************************************************/

#include "2048.h"

/******************************************************
 *  Structure for storing game data
******************************************************/

typedef struct undo_board_st {
    int board[BOARD_SIZE][BOARD_SIZE];
    int score;
    int high_score;
} undo_board_t;

/******************************************************
 *  Global Variables
******************************************************/

int g_undo_level;
uint64_t g_undo_index;
static undo_board_t g_undo_board_array[MAX_LEVEL_UNDO];

/******************************************************
 *  Logic for undoing game
******************************************************/

void undo_game(int board[][BOARD_SIZE], int *p_score, int *p_high_score)
{
    uint64_t array_index;
    if (g_undo_level == 0)
	return;

    g_undo_index--;	//Can underflow if you play real long and have big board size
    g_undo_level--;

    array_index = g_undo_index;
    copy_game_data(g_undo_board_array[array_index % MAX_LEVEL_UNDO].board, board);
    *p_score = g_undo_board_array[array_index % MAX_LEVEL_UNDO].score;
    *p_high_score = g_undo_board_array[array_index % MAX_LEVEL_UNDO].high_score;
}

void add_undo_level(int board[][BOARD_SIZE], int score, int high_score)
{

    copy_game_data(board, g_undo_board_array[g_undo_index % MAX_LEVEL_UNDO].board);
    g_undo_board_array[g_undo_index % MAX_LEVEL_UNDO].score = score;
    g_undo_board_array[g_undo_index % MAX_LEVEL_UNDO].high_score = high_score;

    g_undo_index++;	//Can overflow if you play real long and have big board size
    if (g_undo_level < MAX_LEVEL_UNDO)
	g_undo_level++;
}

void copy_game_data(int src_board[][BOARD_SIZE], int dst_board[][BOARD_SIZE])
{
    int i,j;

    for (i = 0; i < BOARD_SIZE; i++)
    {
	for (j = 0; j < BOARD_SIZE; j++)
	{
	    dst_board[i][j] = src_board[i][j];
	}
    }
}
