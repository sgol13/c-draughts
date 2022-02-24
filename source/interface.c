// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *


#include "interface.h"
#include "defs.h"

void cleanScreen() { printf("\033[2J\033[1;1H"); }

void cleanBuffer() {
    int buffer;
    while ((buffer = getchar()) != '\n' && buffer != EOF) {
    }
}

void setColor(char *color) {
    if (color)
        printf("\033%s", color);
    else
        printf("\033[0m");
}

int CTI(char x) {
    if (x >= 'a' && x <= 'z')
        return x - 'a' + 1;
    else if (x >= '0' && x <= '9')
        return x - '1' + 1;
    else
        return 0;
}

bool checkRange(int x, int game_size) {
    return x >= 1 && x <= game_size ? true : false;
}

void printEndGame(int turn) {
    sleep(1);
    cleanScreen();

    if (turn == 1) {
        setColor(player_A_color);
        printf("\n\tPlayer blue won!\n\n");
    } else {
        setColor(player_B_color);
        printf("\n\tPlayer red won!\n\n");
    }
    setColor(NULL);
    sleep(3);
    remove(SAVE_FILENAME);
}

void printBoard(int board[12][12], char *top_pattern, char *row_pattern,
                int game_size) {
    cleanScreen();

    char column_sign = 'A';
    int i = 0;
    while (top_pattern[i] != 'E') {
        if (top_pattern[i] == 'C')
            printf("%c", column_sign++);
        else
            printf("%c", top_pattern[i]);
        ++i;
    }

    int row_sign = game_size;
    int board_x = 1, board_y = game_size;
    for (int i_rows = 0; i_rows < game_size; i_rows++) {

        i = 0;
        while (row_pattern[i] != 'E') {
            if (row_pattern[i] == 'R') {
                if (row_sign == 10) {
                    printf("%d", 10);
                    i++;
                    row_sign--;
                } else
                    printf("%d", row_sign--);
            } else if (row_pattern[i] == 'X') {

                switch (board[board_x][board_y]) {
                case 1:
                    setColor(player_A_color);
                    printf("%c", player_A_piece);
                    break;
                case 2:
                    setColor(player_B_color);
                    printf("%c", player_B_piece);
                    break;
                case 3:
                    setColor(player_A_color);
                    printf("%c", player_A_king);
                    break;
                case 4:
                    setColor(player_B_color);
                    printf("%c", player_B_king);
                    break;
                default:
                    printf(" ");
                }
                setColor(NULL);

                board_x++;
                if (board_x > game_size) {
                    board_x = 1;
                    board_y--;
                }
            } else
                printf("%c", row_pattern[i]);
            ++i;
        }
    }

    printf("\n\n");
}

void loadBoardPattern(int game_size, char **top_pattern, char **row_pattern) {

    xmlDoc *board_pattern = xmlReadFile(BOARD_PATTERN_FILENAME, NULL, 0);
    xmlNode *root_el = xmlDocGetRootElement(board_pattern);

    char *top_pattern_xml, *row_pattern_xml;

    if (game_size == 10) {
        root_el = root_el->children->next->children->next;
        top_pattern_xml = xmlNodeGetContent(root_el);
        row_pattern_xml = xmlNodeGetContent(root_el->next->next);

    } else if (game_size) {
        root_el = root_el->children->next->next->next->children->next;
        top_pattern_xml = xmlNodeGetContent(root_el);
        row_pattern_xml = xmlNodeGetContent(root_el->next->next);
    }

    int p = 0, r = 0;
    while (top_pattern_xml[p + 1])
        ++p;

    *top_pattern = malloc((p + 2) * sizeof(char));

    while (top_pattern_xml[p] == ' ' || top_pattern_xml[p] == '\n' ||
           top_pattern_xml[p] == '\t')
        top_pattern_xml[p--] = 'E';

    strcpy(*top_pattern, top_pattern_xml);

    while (row_pattern_xml[r + 1])
        ++r;

    *row_pattern = malloc((r + 2) * sizeof(char));

    while (row_pattern_xml[r] == ' ' || row_pattern_xml[r] == '\n' ||
           row_pattern_xml[r] == '\t')
        row_pattern_xml[r--] = 'E';


    strcpy(*row_pattern, row_pattern_xml);

    xmlFreeDoc(board_pattern);
    xmlCleanupParser();
    xmlFree(top_pattern_xml);
    xmlFree(row_pattern_xml);
}

int readInput(int *from_x, int *from_y, int *to_x, int *to_y, int game_size) {
    size_t input_size = INPUT_STRING_MAX_SIZE;
    char *input = malloc(input_size * sizeof(char));

    getline(&input, &input_size, stdin);

    int r = 0;
    while (input[r] == '\t' || input[r] == ' ')
        ++r;

    if (!strncmp(input + r, "exit", 4) || input[r] == '0') {
        free(input);
        return 2;
    }

    bool correct_format = true;

    // FROM
    if (isalpha(input[r]) && checkRange(CTI(input[r]), game_size))
        *from_x = CTI(input[r++]);
    else
        correct_format = false;

    if (isdigit(input[r]) && checkRange(CTI(input[r]), game_size)) {
        if (game_size == 10 && input[r] == '1' && input[r + 1] == '0') {
            *from_y = 10;
            r += 2;
        } else
            *from_y = CTI(input[r++]);
    } else
        correct_format = false;

    // OMIT SPACES
    while (input[r] == '\t' || input[r] == ' ')
        ++r;

    // TO
    if (isalpha(input[r]) && checkRange(CTI(input[r]), game_size))
        *to_x = CTI(input[r++]);
    else
        correct_format = false;

    if (isdigit(input[r]) && checkRange(CTI(input[r]), game_size)) {
        if (game_size == 10 && input[r] == '1' && input[r + 1] == '0') {
            *to_y = 10;
            r += 2;
        } else
            *to_y = CTI(input[r++]);
    } else
        correct_format = false;

    while (input[r]) {
        if (input[r] != ' ' && input[r] != '\t' && input[r] != '\n')
            correct_format = false;
        ++r;
    }

    free(input);
    return !correct_format;
}

void printManual() {
    xmlDoc *manual = xmlReadFile(MANUAL_FILENAME, NULL, 0);
    xmlNode *root_el = xmlDocGetRootElement(manual);

    root_el = root_el->children->next;

    while (root_el) {
        if (root_el->type == XML_ELEMENT_NODE) {
            cleanScreen();
            printf("%s", xmlNodeGetContent(root_el));
            setColor(INFO_COLOR);
            printf("\n\n\tPress enter to continue...\n\n\t");
            setColor(NULL);
            getchar();
        }

        root_el = root_el->next;
    }

    xmlFreeDoc(manual);
    xmlCleanupParser();
}

void printInputLine(int turn) {
    if (turn == 1) {
        setColor(player_A_color);
        printf("\n\tPlayer blue ->  ");
    } else {
        setColor(player_B_color);
        printf("\n\tPlayer red  ->  ");
    }
    setColor(NULL);
}