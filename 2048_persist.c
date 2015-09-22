/******************************************************
 *
 * This file contains functions for saving game state
 * and scores
 *
******************************************************/
#include "2048.h"

static char *g_game_dir;
static char *g_score_file;
static char *g_game_filename;


/******************************************************
 *
 * Functions related to getting filenames
 *
******************************************************/
static char *get_game_dir()
{
    size_t len = 0;
    char *home_dir = NULL;

    if (g_game_dir)
    {
	return g_game_dir;
    }

    home_dir = getenv("HOME");
    len = strlen(home_dir) + strlen(GAME_DIR) + 2;
    g_game_dir = malloc (len);

    if (g_game_dir == NULL)
    {
	return NULL;
    }

    snprintf(g_game_dir, len, "%s/%s", home_dir, GAME_DIR);

    return g_game_dir;
}

static char *get_score_file()
{
    size_t len = 0;
    char *game_dir = NULL;
   
    if (g_score_file)
    {
       return g_score_file;
    }

    game_dir = get_game_dir();

    if (game_dir == NULL)
    {
	return NULL;
    }

    len = strlen(game_dir) + strlen(SCORE_FILE) + 2;
    g_score_file = malloc (len);
    if (g_score_file == NULL)
    {
	return NULL;
    }

    snprintf (g_score_file, len, "%s/%s", game_dir, SCORE_FILE);

    return g_score_file;
}

static int create_game_dir()
{
    int err = 0;
    struct stat stat_buf;
    char *game_dir = get_game_dir();

    if (game_dir == NULL)
    {
	return ENOMEM;
    }

    if (stat (game_dir, &stat_buf) == -1)
    {
	err = mkdir (game_dir, 0755);
	goto exit;
    }

    /* If not directory, throw an error */
    if (! S_ISDIR(stat_buf.st_mode))
    {
	err = 1;
    }

exit:
    return err;
}

static char *get_game_filename()
{
    size_t len = 0;

    if (g_game_filename)
	return g_game_filename;

    len = strlen(get_game_dir()) + strlen(GAME_FILE) + 2;

    g_game_filename = malloc (len);

    if (g_game_filename == NULL)
	return NULL;

    snprintf(g_game_filename, len, "%s/%s", get_game_dir(), GAME_FILE);

    return g_game_filename;
}

/******************************************************
 *
 * Functions related to getting/setting highest score
 *
******************************************************/
int get_highest_score()
{
    int score = 0;
    FILE *fp = NULL;
    char *score_file = NULL;

    struct stat stat_buf;

    score_file = get_score_file();
    if (score_file == NULL)
    {
	return 0;
    }

    if (stat (score_file, &stat_buf) == -1)
    {
	return 0;
    }

    fp = fopen(score_file, "r");
    if (fp)
    {
	if ( fscanf(fp, "%d", &score) != 1)
	{
	    score = 0;
	}
	fclose (fp);
    }

    return score;
}

void save_highest_score()
{
    int err = 0;
    FILE *fp = NULL;
    if (g_saved_high_score >= g_high_score)
    {
	return;
    }

    err = create_game_dir();
    if (err)
	return;

    fp = fopen (get_score_file(), "w");
    if (fp)
    {
	fprintf(fp, "%d", g_high_score);
	fclose (fp);
    }

    g_saved_high_score = g_score;

    return;
}

/******************************************************
 *
 * Functions related to saving/resuming game.
 *
******************************************************/
void save_game_data(int board[][BOARD_SIZE], int score)
{
    int err = 0;
    FILE *fp = NULL;
    char *game_filename = get_game_filename();

    create_game_dir();

    fp = fopen (game_filename, "w");

    if (fp)
    {
	int i,j;
	/* First write marker and Board size in file */
	err = fprintf(fp, "%d %d %d\n", MAGIC_2048, BOARD_SIZE, score);
	if (err < 0) goto out;
	for (i = 0; i < BOARD_SIZE; i++)
	{
	    for (j = 0; j < BOARD_SIZE; j++)
	    {
		err = fprintf(fp, "%d ", board[i][j]);
		if (err < 0) goto out;

	    }
	    err = fprintf(fp, "\n");
	    if (err < 0) goto out;
	}
    }
    else
    {
	err = ENOENT;
    }

out:

    if (fp)
    {
	fclose(fp);
    }
    if (err < 0)
    {
	/* Some error happened. Remove the file. */
	unlink (game_filename);
    }
}

int load_game_data(int board[][BOARD_SIZE], int *p_score)
{
    int err = 0;
    int board_size, marker, score;
    FILE *fp = NULL;
    int tmp_board[BOARD_SIZE][BOARD_SIZE];
    char *game_filename = get_game_filename();

    fp = fopen(game_filename, "r");

    /* First try to load game data in a temp board.
     * If everything goes well, we should copy it to
     * the passed parameter board.  */
    if (fp)
    {
	int i,j;
	err = fscanf(fp, "%d %d %d", &marker, &board_size, &score);
	if (err < 0) goto out;
	if (board_size != BOARD_SIZE)
	{
	    err = EINVAL;
	    goto out;
	}
	if (marker != MAGIC_2048)
	{
	    err = EINVAL;
	    goto out;
	}

	for (i = 0; i < BOARD_SIZE; i++)
	{
	    for (j = 0; j < BOARD_SIZE; j++)
	    {
		err = fscanf(fp, "%d", &(tmp_board[i][j]));
		if (err < 0) goto out;
	    }
	}

	for (i = 0; i < BOARD_SIZE; i++)
	{
	    for (j = 0; j < BOARD_SIZE; j++)
	    {
		board[i][j] = tmp_board[i][j];
	    }
	}
	*p_score = score;
    }
    else
    {
       	err = ENOENT;
    }

out:

    if (fp) fclose (fp);
    return err;
}
