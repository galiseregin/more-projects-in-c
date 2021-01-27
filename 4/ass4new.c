#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
//#include "ass4.h"

#define SIZE 8
#define ERROR 0
#define SUCCESS 1

// Move logical representation
typedef struct {
	char srcPiece, srcRow, srcCol, destPiece, destRow, destCol;//hard func :Row, srcCol,jSrc,iSrc
	int iSrc, jSrc, iDest, jDest;
	int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;//start here Capture_0, isPromotion_0, isCheck_0, isMate_0
} Move;
void tuningOffStruct(Move* move) {
	move->srcPiece = 0;
	move->srcRow = 0;
	move->srcCol=0;
	move->destPiece=0;
	move->destRow=0;
	move->destCol=0;
	move->iSrc = 0;
	move->jSrc = 0;
	move->iDest = 0;
	move->jDest = 0;
	move->isWhite=0;
	move->isCapture=0; 
	move->isPromotion = 0;
	move->isCheck = 0;
	move->isMate = 0;
	move->isLegal=0; 




}
// FEN separator for strtok()
const char SEP[] = "/";
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
int toDigit(char c) {
	assert('0' <= c && c <= '9');
	return c - '0';
}

int prevQLoci, prevQLocj, prevKLoci, prevKLocj, prevNLoci, prevNLocj, prevRLoci, prevRLocj, prevBLoci, prevBLocj;

void createBoard(char board[][SIZE], char fen[])
{  // char fen[] = "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR";
	int j = 0;
	int n = 0;
	int m = 0;
	for (int  i = 0; fen[i] != '\0'; i++)
	{	
			if (((fen[i] > 'a') && (fen[i] < 'z')) || ((fen[i] > 'A') && (fen[i] < 'Z')))
			{
				board[j][m] = fen[i];
				m++;
			}
			else if (fen[i] != '/')
			{
				int k = j;
				int spaces = toDigit(fen[i]);
				for (n = 0; n < spaces; n++)
				{
					board[k][m + n] = ' ';
				}

				m += n;
			}
			else
			{
				j++;
				m = 0;
			}

		
	}
			/*board[i][j] = fen[a++];*/
	/*int n=0;
	for (n = 0; n < strlen(fen); n++) {

	}*/

}
void printSpacers() {
	printf("* -");
	for (int i = 0; i < SIZE; i++) {
		printf("--");
	}
	printf(" *\n");
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

void printRow(char row[], int rowIdx) {
	printf("%d ", rowIdx);
	int i = 0;
	while (row[i]) {
		if (isdigit(row[i])) {
			int spaces = toDigit(row[i++]);
			for (int j = 0; j < spaces; j++) {
				printf("|%c", ' ');
			}
		}
		else {
			printf("|%c", row[i++]);
		}
	}
	printf("| %d\n", rowIdx);
}


void printBoard(char board[][SIZE]) {
	printColumns();
	printSpacers();
	for (int i = 0; i < SIZE; i++) {
		printf("%d ", SIZE - i);
		for (int j = 0; j < SIZE; j++) {
			printf("|%c", board[i][j]);
		}

		printf("| %d\n", SIZE - i);
	}

	printSpacers();
	printColumns();
}


//make func that gets Move move and prints all the fields
char findSrcPiece(char pgn[], int isWhiteTurn)
{
	if (isupper(pgn[0])) {
		if (isWhiteTurn) 
		{
			return pgn[0];
		}
		return tolower(pgn[0]);
	}
	else
	{
		if (isWhiteTurn)
		{
			return WHITE_PAWN;
		}
		return BLACK_PAWN;
	}

}
void specialCase(char pgn[],Move* m) {
	for (int i = 0; i < strlen(pgn[i]); i++)
	{
		if (pgn[i] == CAPTURE) {
			m->isCapture = 1;
		}
		if (pgn[i] == CHECK) {
			m->isCheck = 1;
		}
		if (pgn[i] == MATE) {
			m->isMate = 1;
		}
		if (pgn[i] == PROMOTION ) {
			m->isPromotion = 1;
		}
	}
}

void makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	Move move;
	int i;
	
	move.srcPiece = findSrcPiece(pgn,isWhiteTurn);
	move.isWhite = isWhiteTurn;
	for (int i = strlen(pgn)-1; i >0 ; i--)
	{
		if (isdigit(pgn[i])) {
			//check if dest row ==1
			if (move.destCol == 1) {
				//update src
			}
			else {
				move.destCol = 1;
				move.destRow = 1;
				move.iDest = pgn[i] - '0';
				move.jDest = pgn[i - 1] - '0';
			}
			break;
		}

	}


