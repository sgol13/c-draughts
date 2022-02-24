// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *

#include "game.h"
#include "defs.h"
#include "interface.h"

void game(int game_size, bool new_game) {

    // CREATE GAME
    int board[12][12];
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            board[x][y] = 0;
        }
    }

    int turn = 1;
    if (new_game) {
        initialGameSet(board, game_size);
    } else {
        loadGame(board, &game_size, &turn);
    }

    char *top_pattern, *row_pattern;
    loadBoardPattern(game_size, &top_pattern, &row_pattern);

    bool finish = false, captured = false;
    bool incorrect_format = false, impossible_move = false, save_impossible = false;
    int from_x, from_y, to_x, to_y, pr_x, pr_y, result = 0;

    int pieces_A = countPieces(board, 1, game_size);
    int pieces_B = countPieces(board, 2, game_size);

    while (!finish) {

        pr_x = pr_y = 0;
        do {

            printBoard(board, top_pattern, row_pattern, game_size);
            printInputLine(turn);

            result = readInput(&from_x, &from_y, &to_x, &to_y, game_size);


            incorrect_format = impossible_move = save_impossible = captured = false;
            if (result == 1) {
                incorrect_format = true;
                printf("\n\t%s", FORMAT_ERROR);
                sleep(1);
                continue;
            }
            if (result == 2) {
                if (pr_x || pr_y) {
                    save_impossible = true;
                    printf("\n\t%s", SAVE_ERROR);
                    sleep(2);
                    continue;
                }

                saveGame(board, game_size, turn);

                finish = true;
                break;
            }

            if (!isPossible(board, turn, from_x, from_y, to_x, to_y, pr_x, pr_y)) {
                impossible_move = true;
                printf("\n\t%s", MOVE_ERROR);
                sleep(1);
                continue;
            }

            captured = move(board, turn, from_x, from_y, to_x, to_y);
            pr_x = to_x;
            pr_y = to_y;

            if (captured) {
                if (turn == 1)
                    pieces_B--;
                else
                    pieces_A--;

                if (!pieces_A || !pieces_B) {
                    printBoard(board, top_pattern, row_pattern, game_size);
                    printEndGame(turn);
                    finish = true;
                    break;
                }
            }

        } while ((checkCapture(board, pr_x, pr_y, turn, game_size) && captured) ||
                 save_impossible || incorrect_format || impossible_move);

        if (turn == 1 && pr_y == game_size)
            board[pr_x][pr_y] = 3;
        else if (turn == 2 && pr_y == 1)
            board[pr_x][pr_y] = 4;

        turn = OP(turn);
    }


    free(top_pattern);
    free(row_pattern);
}

void initialGameSet(int board[12][12], int game_size) {
    if (game_size == 8) {
        for (int y = 1; y <= 3; y++) {
            int x = (y + 1) % 2 + 1;
            for (int i = 0; i < 4; i++) {
                board[x][y] = 1;
                x += 2;
            }
        }
        for (int y = 6; y <= 8; y++) {
            int x = (y + 1) % 2 + 1;
            for (int i = 0; i < 4; i++) {
                board[x][y] = 2;
                x += 2;
            }
        }
    } else {
        for (int y = 1; y <= 4; y++) {
            int x = (y + 1) % 2 + 1;
            for (int i = 0; i < 5; i++) {
                board[x][y] = 1;
                x += 2;
            }
        }
        for (int y = 7; y <= 10; y++) {
            int x = (y + 1) % 2 + 1;
            for (int i = 0; i < 5; i++) {
                board[x][y] = 2;
                x += 2;
            }
        }
    }
}

void saveGame(int board[12][12], int game_size, int turn) {
    xmlDocPtr save = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_el = xmlNewNode(NULL, BAD_CAST "game_save");

    xmlDocSetRootElement(save, root_el);

    xmlNewChild(root_el, NULL, BAD_CAST "game_size",
                BAD_CAST(game_size == 10 ? "10" : "8"));

    xmlNewChild(root_el, NULL, BAD_CAST "turn", BAD_CAST(turn == 1 ? "1" : "2"));

    root_el = xmlNewChild(root_el, NULL, BAD_CAST "pieces", BAD_CAST "");

    int counter = 1;
    for (int y = 1; y <= game_size; y++) {
        for (int x = 1; x <= game_size; x++) {
            if (board[x][y]) {
                char piece[10], name[10];

                snprintf(piece, 10, "%d %d %d", board[x][y], x, y);
                snprintf(name, 10, "piece_%d", counter++);
                xmlNewChild(root_el, NULL, BAD_CAST name, BAD_CAST piece);
            }
        }
    }

    xmlSaveFormatFileEnc(SAVE_FILENAME, save, "UTF-8", 1);
    xmlFreeDoc(save);
    xmlCleanupParser();
}

void loadGame(int board[12][12], int *game_size, int *turn) {

    xmlDoc *save = xmlReadFile(SAVE_FILENAME, NULL, 0);
    xmlNode *root_el = xmlDocGetRootElement(save);
    char *read;

    root_el = root_el->children->next;
    read = (char *)xmlNodeGetContent(root_el);
    *game_size = atoi(read);

    root_el = root_el->next->next;
    read = (char *)xmlNodeGetContent(root_el);
    *turn = atoi(read);

    root_el = root_el->next->next->children;

    while (root_el) {
        if (root_el->type == XML_ELEMENT_NODE) {

            read = (char *)xmlNodeGetContent(root_el);

            int player = atoi(read);
            int x = atoi(read + 1);
            int y = atoi(read + 4);

            board[x][y] = player;
        }

        root_el = root_el->next;
    }

    xmlFree(read);
    xmlFreeDoc(save);
    xmlCleanupParser();
}

bool isPossible(int board[12][12], int turn, int from_x, int from_y, int to_x,
                int to_y, int pr_x, int pr_y) {

    // it's not our piece
    if (!isSame(board[from_x][from_y], turn)) {
        return 0;
    }

    // destination is occupied
    if (board[to_x][to_y]) {
        return 0;
    }

    // not first move
    if ((pr_x || pr_y) && (pr_x != from_x || pr_y != from_y)) {
        return 0;
    }

    // NORMAL PIECE
    if (board[from_x][from_y] == turn) {

        // NO CAPTURE
        if (!pr_x && !pr_y) {
            if (turn == 1) {
                if (to_x == from_x + 1 && to_y == from_y + 1)
                    return 1;
                if (to_x == from_x - 1 && to_y == from_y + 1)
                    return 1;
            } else {
                if (to_x == from_x + 1 && to_y == from_y - 1)
                    return 1;
                if (to_x == from_x - 1 && to_y == from_y - 1)
                    return 1;
            }
        }

        // CAPTURE
        int middle_x = (from_x + to_x) / 2, middle_y = (from_y + to_y) / 2;
        if (board[middle_x][middle_y] == OP(turn) ||
            board[middle_x][middle_y] == OP(turn) + 2) {

            if (to_x == from_x + 2 && to_y == from_y + 2)
                return 1;
            if (to_x == from_x - 2 && to_y == from_y + 2)
                return 1;
            if (to_x == from_x + 2 && to_y == from_y - 2)
                return 1;
            if (to_x == from_x - 2 && to_y == from_y - 2)
                return 1;
        }

    } else { // KING

        int diagonal[4][2] = {{-1, 1}, {1, -1}, {1, 1}, {-1, -1}};
        int diagonal_type = 0;
        if (from_x + from_y == to_x + to_y)
            diagonal_type = to_y > from_y ? 1 : 2;
        else if (from_x - from_y == to_x - to_y)
            diagonal_type = to_y > from_y ? 3 : 4;

        if (diagonal_type >= 1 && diagonal_type <= 4) {
            int x = from_x, y = from_y;
            bool captured = false;

            while (x != to_x || y != to_y) {
                x += diagonal[diagonal_type - 1][0];
                y += diagonal[diagonal_type - 1][1];

                if (isSame(board[x][y], turn))
                    return 0;

                if (isOpposite(board[x][y], turn)) {
                    if (captured)
                        return 0;
                    else
                        captured = true;
                }
            }
            if ((pr_x || pr_y) && !captured)
                return 0;
            return 1;
        }
    }

    return 0;
}

int countPieces(int board[12][12], int player, int game_size) {
    int result = 0;
    for (int y = 1; y <= game_size; y++)
        for (int x = 1; x <= game_size; x++)
            if (player == 1 && (board[x][y] == 1 || board[x][y] == 3))
                result++;
            else if (player == 2 && (board[x][y] == 2 || board[x][y] == 4))
                result++;
    return result;
}

bool move(int board[12][12], int turn, int from_x, int from_y, int to_x, int to_y) {
    // NORMAL PIECE
    if (board[from_x][from_y] == 1 || board[from_x][from_y] == 2) {
        // NO CAPTURE
        if (abs(from_x - to_x) == 1) {

            board[to_x][to_y] = turn;
            board[from_x][from_y] = 0;
            return 0;

        } else { // CAPTURE

            board[to_x][to_y] = turn;
            board[from_x][from_y] = 0;
            board[(from_x + to_x) / 2][(from_y + to_y) / 2] = 0;
            return 1;
        }

    } else {
        int diagonal[4][2] = {{-1, 1}, {1, -1}, {1, 1}, {-1, -1}};
        int diagonal_type = 0;

        if (from_x + from_y == to_x + to_y)
            diagonal_type = to_y > from_y ? 1 : 2;
        else if (from_x - from_y == to_x - to_y)
            diagonal_type = to_y > from_y ? 3 : 4;

        int x = from_x, y = from_y;
        bool captured = false;

        while (x != to_x || y != to_y) {
            x += diagonal[diagonal_type - 1][0];
            y += diagonal[diagonal_type - 1][1];

            if (isOpposite(board[x][y], turn)) {
                board[x][y] = 0;
                captured = true;
            }
        }
        board[to_x][to_y] = board[from_x][from_y];
        board[from_x][from_y] = 0;

        if (captured)
            return 1;
    }

    return 0;
}

int OP(int turn) { return turn % 2 + 1; }

bool isOpposite(int x, int turn) { return x == OP(turn) || x == OP(turn) + 2; }

bool isSame(int x, int turn) { return x == turn || x == turn + 2; }

bool checkCapture(int board[12][12], int x, int y, int turn, int game_size) {
    if (x == 0 && y == 0)
        return 0;

    if (board[x][y] == turn) {
        bool up = (y + 2 <= game_size);
        bool right = (x + 2 <= game_size);
        bool down = (y - 2 >= 1);
        bool left = (x - 2 >= 1);

        if (isOpposite(board[x + 1][y + 1], turn) && up && right &&
            !board[x + 2][y + 2])
            return 1;

        if (isOpposite(board[x + 1][y - 1], turn) && down && right &&
            !board[x + 2][y - 2])
            return 1;

        if (isOpposite(board[x - 1][y - 1], turn) && down && left &&
            !board[x - 2][y - 2])
            return 1;

        if (isOpposite(board[x - 1][y + 1], turn) && up && left &&
            !board[x - 2][y + 2])
            return 1;

    } else if (board[x][y] == turn + 2) {

        int diagonal[4][2] = {{-1, 1}, {1, -1}, {1, 1}, {-1, -1}};
        for (int i = 0; i < 4; i++) {
            int xx = x, yy = y;

            xx += diagonal[i][0];
            yy += diagonal[i][1];

            while (checkRange(xx, game_size) && checkRange(yy, game_size)) {

                if (isSame(board[xx][yy], turn))
                    break;

                if (isOpposite(board[xx][yy], turn)) {
                    xx += diagonal[i][0];
                    yy += diagonal[i][1];
                    if (checkRange(xx, game_size) && checkRange(yy, game_size) &&
                        board[xx][yy] == 0)
                        return 1;
                    else
                        break;
                }

                xx += diagonal[i][0];
                yy += diagonal[i][1];
            }
        }
    }


    return 0;
}