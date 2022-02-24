// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *

#include "defs.h"
#include "game.h"
#include "interface.h"


int main() {

    bool finish = false;
    int choice;

    while (!finish) {
        cleanScreen();
        setColor(MENU_COLOR);

        printf("\n\n\n\tDRAUGHTS\n\n");

        printf("\t1 - new 8x8 game\n");
        printf("\t2 - new 10x10 game\n");
        printf("\t3 - load game\n");
        printf("\t4 - manual\n");
        printf("\t5 - exit game\n\n\t");
        scanf("%d", &choice);
        cleanBuffer();
        setColor(NULL);

        switch (choice) {
        case 1:
            game(8, true);
            break;
        case 2:
            game(10, true);
            break;
        case 3:
            if (access(SAVE_FILENAME, F_OK)) {
                printf("\tYou don't have any save.\n");
                sleep(3);
                break;
            }
            game(10, false);
            break;
        case 4:
            printManual();
            break;
        case 5:
            cleanScreen();
            printf("\n\tThank you for playing.\n\n");
            finish = true;
            break;
        }
    }

    return 0;
}