#ifndef __2048_H__
#define __2048_H__

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

/******************************************************
 * 			Constants 		      *
 *****************************************************/
#define BOARD_SIZE 	4

#define MAGIC_2048	2048

#define WINNING_SCORE	2048

#define MAX_LEVEL_UNDO	5
#define UNDO_ARRAY_SIZE MAX_LEVEL_UNDO

#define LEFT_KEY	260
#define RIGHT_KEY	261
#define UP_KEY		259
#define DOWN_KEY	258
#define CTRL_C_KEY	3
#define NEW_GAME_KEY	110
#define SAVE_GAME_KEY	115
#define RESUME_GAME_KEY	108
#define UNDO_GAME_KEY	117

#define GAME_DIR	/* getenv("HOME") + */ ".2048"
#define SCORE_FILE 	"score"
#define GAME_FILE	"game"

/******************************************************
 * 		Global Variables	      	      *
 *****************************************************/
extern int g_board[BOARD_SIZE][BOARD_SIZE];
extern int g_saved_high_score;
extern uint64_t g_undo_index;
extern int g_high_score;
extern int g_undo_level;
extern int g_exit_game;
extern int g_finished;
extern int g_score;
extern int g_won;

void save_highest_score();
int get_highest_score();
void save_game_data(int board[][BOARD_SIZE], int score);
int load_game_data(int board[][BOARD_SIZE], int *p_score);
void add_undo_level(int board[][BOARD_SIZE], int score, int high_score);
void undo_game(int board[][BOARD_SIZE], int *p_score, int *p_high_score);
void copy_game_data(int src_board[][BOARD_SIZE], int dst_board[][BOARD_SIZE]);

#endif /* #ifndef __2048_H__ */
