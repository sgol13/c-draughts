// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *

#ifndef DEFS_H
#define DEFS_H

#include <ctype.h>
#include <libxml/parser.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define player_A_piece 'O'
#define player_A_king 'M'
#define player_B_piece 'O'
#define player_B_king 'M'
#define player_A_color "[1;34m"
#define player_B_color "[1;31m"
#define MENU_COLOR "[1;32m"
#define INFO_COLOR "[1;35m"

#ifndef RESOURCES_DIR
#define RESOURCES_DIR ""
#endif

#define BOARD_PATTERN_FILENAME RESOURCES_DIR "board.xml"
#define SAVE_FILENAME RESOURCES_DIR "save.xml"
#define MANUAL_FILENAME RESOURCES_DIR "manual.xml"
#define FORMAT_ERROR "incorrect format\n"
#define MOVE_ERROR "invalid move\n"
#define SAVE_ERROR "You can save the game only at the beginning of your turn.\n"
#define INPUT_STRING_MAX_SIZE 64

#define markline printf("line %d\n", __LINE__);
#define show(a) printf("%s= %d\n", #a, a);

#endif