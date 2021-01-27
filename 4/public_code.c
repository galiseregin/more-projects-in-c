#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "ass4.h"

// Move logical representation
typedef struct {
    char srcPiece, srcRow, srcCol, destPiece, destRow, destCol;
    int iSrc, jSrc, iDest, jDest;
    int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;
} Move;

// PGN characters
const char PAWN = 'P';
const char ROOK = 'R';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char QUEEN = 'Q';
const char KING = 'K';
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';
const char FIRST_COL = 'a';

// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';
const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

/**************************
 * convert char to digit
 **************************/
int toDigit(char c) {
    assert('0' <= c && c <= '9');
    return c - '0';
}

void printColumns() {
    char column = toupper(FIRST_COL);
    printf("* |");
    for (int i = 0; i < SIZE; i++) {
        if (i) {
            printf(" ");
        }
        printf("%c", column);
        column++;
    }
    printf("| *\n");
}

void printSpacers() {
    printf("* -");
    for (int i = 0; i < SIZE; i++) {
        printf("--");
    }
    printf(" *\n");
}

void printRow(char row[], int rowIdx) {
    printf("%d ", rowIdx);
    int i = 0;
    while (row[i]) {
        if (isdigit(row[i])) {
            int spaces = toDigit(row[i++]);
            for (int j = 0; j < spaces; j++) {
                printf("|%c", EMPTY);
            }
        } else {
            printf("|%c", row[i++]);
        }
    }
    printf("| %d\n", rowIdx);
}

void printBoardFromFEN(char fen[]) {
    printColumns();
    printSpacers();
    int rowIdx = SIZE;
    char* fenRow = strtok(fen, SEP);
    while (fenRow != NULL) {
        printRow(fenRow, rowIdx--);
        fenRow = strtok(NULL, SEP);
    }
    printSpacers();
    printColumns();
}
