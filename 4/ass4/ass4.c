#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "../ass4.h"

/************************
NAME: Gali Seregin
ID:322060187
Course:01
Ass04
*************************/
typedef enum MoveType {
	PawnMove,
	NormalMove,
	CaptureMove,
	PromotionMove,
}MoveType;

typedef enum EndType {
	None,
	Chess,
	Mate
}EndType;

typedef enum { FALSE = 0, TRUE = 1 } boolean;

#define WHITE 1
#define BLACK 0

#define WHITE_PAWN_STARTING_ROW 1
#define BLACK_PAWN_STARTING_ROW (SIZE - WHITE_PAWN_STARTING_ROW - 1)

const char EMPTY = ' ';

const char NO_HINT = 'H';

const char DANGEROUS = 'D';
const char OK = 'K';

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

void interchange(char board[][SIZE]);
MoveType getMoveType(char pgn[]);

boolean tryAndMovePawn(char board[][SIZE], char pgn[], int isWhiteTurn);
boolean tryAndMoveNormal(char board[][SIZE], char pgn[], int isWhiteTurn);
boolean tryAndMovePromotion(char board[][SIZE], char pgn[], int isWhiteTurn);
boolean tryAndMoveCapture(char board[][SIZE], char pgn[], int isWhiteTurn);
void revertMove(char board[][SIZE], int srcRow, int srcCol, int dstRow, int dstCol, char srcPiece, char dstPiece);

boolean checkSourceObjectIsValid(char board[][SIZE], int i, int j, int isWhiteTurn);
boolean checkColumnAndRowValid(char row, char col);
boolean isCellOccupied(char board[][SIZE], int row, int col);
boolean isPawn(char piece);

void markDangerousSpotFromPawn(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int color);
void markDangerousSpotFromKnight(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col);
void markDangerousSpotFromQueen(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol);
void markDangerousSpotFromRook(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol);
void markDangerousSpotFromKing(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col);
void markDangerousSpotFromBishop(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol);
void markDangerousSpotFromBishopInOneDirection(char board[][SIZE], char dangerousCellsBoard[][SIZE], int rowDelta, int colDelta, int row, int col, int kingRow, int kingCol);

boolean handleCheckAndMateMove(char board[][SIZE], int srcRow, int srcCol, int dstRow, int dstCol, int opponentKingColor, EndType endType, char promotionPiece);
void markDangerousSpots(char board[][SIZE], char dangerousCellsBoard[][SIZE], int kingRow, int kingCol, int oppositeColor);
int getKingNumberOfOptionalMoves(int kingRow, int kingCol);
int getKingNumberOfDangerousAroundKing(char dangerousCellsBoard[][SIZE], int kingRow, int kingCol);
int getNumberOfCellsKingCanMove(char board[][SIZE], int kingColor);

boolean checkValidRookMove(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, boolean isCaptureMove);
boolean checkValidKnightMove(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol);
boolean checkAllEmptyCellsInRow(char board[][SIZE], int sourceRow, int sourceCol, int destCol, boolean includeDst);
boolean checkAllEmptyCellsInColumn(char board[][SIZE], int sourceCol, int sourceRow, int destRow, boolean includeDst);
boolean checkAllEmptyCellsInDiagonal(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, boolean includeDst);

void moveSrcToDest(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, const char to);

int toDigit(char c) {
	assert('0' <= c && c <= '9');
	return c - '0';
}

void createBoard(char board[][SIZE], char fen[])
{
	int j = 0;
	int n = 0;
	int m = 0;
	for (int i = 0; fen[i] != '\0'; i++)
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
}

/********************************************************************************************************************
* Function name: interchange
* Input: char[][] - 2d array containing the table to interchange(valid values: any char of the chess-board)
* Output: None
* Function Operation: The function interchanges the given board
********************************************************************************************************************/
void interchange(char board[][SIZE])
{
	int lastRow = SIZE - 1;

	for (int j = 0; j <= lastRow / 2; j++) {
		for (int i = 0; i < SIZE; i++)
		{
			char t = board[j][i];
			board[j][i] = board[lastRow - j][i];
			board[lastRow - j][i] = t;
		}
	}
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

/********************************************************************************************************************
* Function name: isPawn
* Input: char piece(valid values - any piece of chess game)
* Output: boolean(TRUE=1, FALSE=0)
* Function Operation: The function checks if the given piece is a pawn, if yes - return TRUE(=1), else - returns
					  FALSE(=0)
********************************************************************************************************************/
boolean isPawn(char piece)
{
	return (piece != KING && piece != QUEEN && piece != BISHOP && piece != ROOK && piece != KNIGHT);
}


/********************************************************************************************************************
* Function name: getMoveType
* Input: char pgn[](valid values - any chess move)
* Output: MoveType(valid values - PromotionMove/CaptureMove/PawnMove/NormalMove)
* Function Operation: The function checks the type of the given move, and returns the type of it
********************************************************************************************************************/
MoveType getMoveType(char pgn[]) {
	char c;
	if (strchr(pgn, PROMOTION) != NULL)
		return PromotionMove;

	if (strchr(pgn, CAPTURE) != NULL)
		return CaptureMove;

	c = pgn[0];
	if (isPawn(c))
		return PawnMove;

	return NormalMove;
}

/********************************************************************************************************************
* Function name: moveSrcToDest
* Input: char board[][SIZE](valid values - any char from the board)
*        int sourceRow(valid values - 0 <= sourceRow <= (SIZE - 1)
*        int sourceCol(valid values - 0 <= sourceCol <= (SIZE - 1)
*        int destRow(valid values - 0 <= destRow <= (SIZE - 1)
*        int destCol(valid values - 0 <= destCol <= (SIZE - 1)
*        char to(valid values - any character representing a chess piece)
* Output: None
* Function Operation: The function moves a chess piece from one cell to another, puts EMPTY values to the source
*					  and moves it to the destination with the values of the to variable
********************************************************************************************************************/
void moveSrcToDest(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, const char to)
{
	board[sourceRow][sourceCol] = EMPTY;
	board[destRow][destCol] = to;
}

/********************************************************************************************************************
* Function name: revertMove
* Input: char board[][SIZE](valid values - any char from the board)
*        int sourceRow(valid values - 0 <= sourceRow <= (SIZE - 1)
*        int sourceCol(valid values - 0 <= sourceCol <= (SIZE - 1)
*        int destRow(valid values - 0 <= destRow <= (SIZE - 1)
*        int destCol(valid values - 0 <= destCol <= (SIZE - 1)
*        char srcPiece(valid values - any character representing a chess piece)
*        char dstPiece(valid values - any character representing a chess piece)
* Output: None
* Function Operation: The function swaps the piece from the destination cell to be the value of the srcPiece,
*                     and changes the source cell to be the value of the dstPiece
********************************************************************************************************************/
void revertMove(char board[][SIZE], int srcRow, int srcCol, int dstRow, int dstCol, char srcPiece, char dstPiece) {
	board[srcRow][srcCol] = srcPiece;
	board[dstRow][dstCol] = dstPiece;
}

/********************************************************************************************************************
* Function name: moveSrcToDest
* Input: char board[][SIZE](valid values - any char from the board)
*        int sourceRow(valid values - 0 <= sourceRow <= (SIZE - 1)
*        char pgn[](valid values - any chess move)
*        int isWhiteTurn(valid values - 1/0)
* Output: int(1/0)
* Function Operation: The function makes a chess move. if the move is valid, it makes it and returns 1,
*					  if not - it doesnt make the move and returns 0
********************************************************************************************************************/
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	boolean validMove;
	MoveType moveType;

	moveType = getMoveType(pgn);

	interchange(board);
	switch (moveType) {
	case PromotionMove:
		validMove = tryAndMovePromotion(board, pgn, isWhiteTurn);
		break;
	case CaptureMove:
		validMove = tryAndMoveCapture(board, pgn, isWhiteTurn);
		break;
	case PawnMove:
		validMove = tryAndMovePawn(board, pgn, isWhiteTurn);
		break;
	case NormalMove:
		validMove = tryAndMoveNormal(board, pgn, isWhiteTurn);
		break;
	default:
		validMove = FALSE;
		break;
	}
	interchange(board);
	return validMove;
}

/********************************************************************************************************************
* Function name: checkColumnAndRowValid
* Input: char col(valid values: 'a'-'h')
*        char row(valid values: '1'-'8')
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if the given col and row receives represent valid row and column value in
*                     a chess move. if it is - returns TRUE, otherwise - FALSE
********************************************************************************************************************/
boolean checkColumnAndRowValid(char col, char row)
{
	if (col < 'a' || col > 'h')
		return FALSE;
	if (row < '1' || row > '8')
		return FALSE;
	return TRUE;
}


/********************************************************************************************************************
* Function name: isCellOccupied
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int row(valid values: 0 <= row <= SIZE - 1
*         int col(valid values: 0 <= col <= SIZE - 1
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if there is a chess piece on board[row][col]. if yes - return TRUE
*					  otherwise - FALSE
********************************************************************************************************************/
boolean isCellOccupied(char board[][SIZE], int row, int col)
{
	return board[row][col] != EMPTY;
}


/********************************************************************************************************************
* Function name: checkAllEmptyCellsInRow
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int sourceRow(valid values: 0 <= sourceRow <= SIZE - 1
*         int sourceCol(valid values: 0 <= sourceCol <= SIZE - 1
*         int destCol(valid values: 0 <= destCol <= SIZE - 1
*		  boolean includeDst(TRUE/FALSE)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if on the given row(sourceRow), there are other pieces that reaches
*                     the destination(dstCol) with having only empty cells, if all the cells are empty, return TRUE
*					  else - returns FALSE
********************************************************************************************************************/
boolean checkAllEmptyCellsInRow(char board[][SIZE], int sourceRow, int sourceCol, int destCol, boolean includeDst)
{
	int j;
	if (destCol > sourceCol)
	{
		if (!includeDst)
			destCol -= 1;
		for (j = sourceCol + 1; j <= destCol; j++)
		{
			if (isCellOccupied(board, sourceRow, j))
				return FALSE;
		}
	}
	else
	{
		if (!includeDst)
			destCol += 1;
		for (j = sourceCol - 1; j >= destCol; j--)
		{
			if (isCellOccupied(board, sourceRow, j))
				return FALSE;
		}
	}
	return TRUE;
}

/********************************************************************************************************************
* Function name: checkAllEmptyCellsInColumn
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int sourceCol(valid values: 0 <= sourceCol <= SIZE - 1
*         int sourceRow(valid values: 0 <= sourceRow <= SIZE - 1
*         int destRow(valid values: 0 <= destRow <= SIZE - 1
*		  boolean includeDst(TRUE/FALSE)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if on the given col(sourceCol), there are other pieces that reaches
*                     the destination(destRow) with having only empty cells, if all the cells are empty, return TRUE
*					  else - returns FALSE
********************************************************************************************************************/
boolean checkAllEmptyCellsInColumn(char board[][SIZE], int sourceCol, int sourceRow, int destRow, boolean includeDst)
{
	int i;
	if (destRow > sourceRow)
	{
		if (!includeDst)
			destRow -= 1;
		for (i = sourceRow + 1; i <= destRow; i++)
		{
			if (isCellOccupied(board, i, sourceCol))
				return FALSE;
		}
	}
	else
	{
		if (!includeDst)
			sourceRow += 1;
		for (i = destRow - 1; i >= sourceRow; i--)
		{
			if (isCellOccupied(board, i, sourceCol))
				return FALSE;
		}
	}
	return TRUE;
}

/********************************************************************************************************************
* Function name: checkAllEmptyCellsInDiagonal
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int sourceCol(valid values: 0 <= sourceCol <= SIZE - 1
*         int sourceRow(valid values: 0 <= sourceRow <= SIZE - 1
*         int destRow(valid values: 0 <= destRow <= SIZE - 1
*         int destCol(valid values: 0 <= destCol <= SIZE - 1
*		  boolean includeDst(TRUE/FALSE)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if from the given source(board[sourceRow][sourceCol], all the cells are
*					  empty until the destination cell(board[destRow][destCol]), if yes - returns TRUE,
*					  otherwise - FALSE.
********************************************************************************************************************/
boolean checkAllEmptyCellsInDiagonal(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, boolean includeDst)
{
	int len, i;
	len = abs(destCol - sourceCol);
	if (abs(destCol - sourceCol) != abs(destRow - sourceRow))
		return FALSE;

	if ((len == 1) && isCellOccupied(board, destRow, destCol))
		return TRUE;

	if (!includeDst)
		len -= 1;

	if (sourceRow < destRow && sourceCol < destCol)
	{
		for (i = 1; i <= len; i++)
		{
			if (isCellOccupied(board, sourceRow + i, sourceCol + i))
				return FALSE;
		}
	}
	else if (sourceRow < destRow && sourceCol > destCol)
	{
		for (i = 1; i <= len; i++)
		{
			if (isCellOccupied(board, sourceRow + i, sourceCol - i))
				return FALSE;
		}
	}
	else if (sourceRow > destRow&& sourceCol < destCol)
	{
		for (i = 1; i <= len; i++)
		{
			if (isCellOccupied(board, sourceRow - i, sourceCol + i))
				return FALSE;
		}
	}
	else if (sourceRow > destRow&& sourceCol > destCol)
	{
		for (i = 1; i <= len; i++)
		{
			if (isCellOccupied(board, sourceRow - i, sourceCol - i))
				return FALSE;
		}
	}
	return TRUE;
}

/********************************************************************************************************************
* Function name: checkValidKnightMove
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int sourceRow(valid values: 0 <= sourceRow <= SIZE - 1
*         int sourceCol(valid values: 0 <= sourceCol <= SIZE - 1
*         int destRow(valid values: 0 <= destRow <= SIZE - 1
*         int destCol(valid values: 0 <= destCol <= SIZE - 1
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if from the given board[sourceRow][sourceCol] cell, the knight can
*					  reach the destination(board[destRow][destCol], if yes-  return TRUE, otherwise - FALSE
********************************************************************************************************************/
boolean checkValidKnightMove(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol)
{
	int i, moveRow, moveCol;
	int rowOffsets[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
	int colOffsets[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

	for (i = 0; i < 8; i++)
	{
		moveRow = sourceRow + rowOffsets[i];
		moveCol = sourceCol + colOffsets[i];
		if (moveRow < 0 || moveRow >= SIZE || moveCol < 0 || moveCol >= SIZE)
			continue;
		if (moveRow == destRow && moveCol == destCol)
			return TRUE;
	}
	return FALSE;
};


/********************************************************************************************************************
* Function name: checkSourceObjectIsValid
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         int i(valid values: 0 <= i <= SIZE - 1
*         int j(valid values: 0 <= j <= SIZE - 1
*         int isWhiteTurn(valid values: 0 or 1)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if board[i][j] is a valid chess piece for the given turn(isWhiteTurn).
*                     if it is - returns TRUE, otherwise - FALSE
********************************************************************************************************************/
boolean checkSourceObjectIsValid(char board[][SIZE], int i, int j, int isWhiteTurn)
{
	char object;

	object = board[i][j];
	if (isWhiteTurn == WHITE)
		return (object == WHITE_BISHOP || object == WHITE_KING || object == WHITE_KNIGHT || object == WHITE_QUEEN || object == WHITE_ROOK || object == WHITE_PAWN);
	else if (isWhiteTurn == BLACK)
		return (object == BLACK_BISHOP || object == BLACK_KING || object == BLACK_KNIGHT || object == BLACK_QUEEN || object == BLACK_ROOK || object == BLACK_PAWN);
}


/********************************************************************************************************************
* Function name: tryAndMoveNormal
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         char pgn[](valid values: a normal chess move)
*         int isWhiteTurn(valid values: 0 or 1)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function receives a chess move that is considered normal(without x or =), parses it,
*					  validates it and makes the move if it is valid(considering check/mate if needed).
*					  if the move was made - return TRUE, otherwise - FALSE
********************************************************************************************************************/
boolean tryAndMoveNormal(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	int destRow, destCol, i, j, pgnLength, startRow = 0, endRow = SIZE - 1, startCol = 0, endCol = SIZE - 1;
	int destI, destJ;
	char destRowChar, destColChar, movingObject, upperMovingObject, hintChar, hintRow = NO_HINT, hintCol = NO_HINT;
	boolean found;
	EndType endType;

	pgnLength = strlen(pgn);
	endType = None;

	if (pgn[pgnLength - 1] == '#')
		endType = Mate;
	else if (pgn[pgnLength - 1] == '+')
		endType = Chess;

	if (endType != None)
	{
		pgnLength -= 1;
	}

	movingObject = pgn[0];
	upperMovingObject = toupper(movingObject);
	destColChar = pgn[pgnLength - 2];
	destRowChar = pgn[pgnLength - 1];

	destCol = destColChar - 'a';
	destRow = destRowChar - '1';

	if (!checkColumnAndRowValid(destColChar, destRowChar))
		return FALSE;

	if (isCellOccupied(board, destRow, destCol))
		return FALSE;

	if (pgnLength > 3)
	{
		hintChar = pgn[1];
		if (hintChar >= 'a' && hintChar <= 'h')
			hintCol = hintChar;
		else if (hintChar >= '1' && hintChar <= '8')
			hintRow = hintChar;

		if (pgnLength == 5)
		{
			hintChar = pgn[2];
			if (hintChar >= 'a' && hintChar <= 'h')
				hintCol = hintChar;
			else if (hintChar >= '1' && hintChar <= '8')
				hintRow = hintChar;
		}

		if (hintCol != NO_HINT)
		{
			startCol = hintCol - 'a';
			endCol = startCol;
		}

		if (hintRow != NO_HINT)
		{
			startRow = hintRow - '0';
			endRow = startRow;
		}
	}

	found = FALSE;
	for (i = startRow; (i <= endRow) && !found; i++)
	{
		for (j = startCol; (j <= endCol) && !found; j++)
		{
			if (isCellOccupied(board, i, j))
			{
				if ((board[i][j] == ((isWhiteTurn) ? WHITE_BISHOP : BLACK_BISHOP)) && upperMovingObject == BISHOP)
				{
					found = checkAllEmptyCellsInDiagonal(board, i, j, destRow, destCol, TRUE);
				}
				else if ((board[i][j] == ((isWhiteTurn) ? WHITE_KING : BLACK_KING)) && upperMovingObject == KING)
				{
					found = (abs(destCol - j) == 1 || abs(destRow - i) == 1);
				}
				else if ((board[i][j] == ((isWhiteTurn) ? WHITE_KNIGHT : BLACK_KNIGHT)) && upperMovingObject == KNIGHT)
				{
					found = checkValidKnightMove(board, i, j, destRow, destCol);
				}
				else if ((board[i][j] == ((isWhiteTurn) ? WHITE_QUEEN : BLACK_QUEEN)) && upperMovingObject == QUEEN)
				{
					found = checkAllEmptyCellsInDiagonal(board, i, j, destRow, destCol, TRUE) ||
						checkValidRookMove(board, i, j, destRow, destCol, FALSE);
				}
				else if ((board[i][j] == ((isWhiteTurn) ? WHITE_ROOK : BLACK_ROOK)) && upperMovingObject == ROOK)
				{
					found = checkValidRookMove(board, i, j, destRow, destCol, FALSE);
				}
				if (found)
				{
					destI = i;
					destJ = j;
				}
			}
		}
	}

	if (!found)
		return FALSE;
	return handleCheckAndMateMove(board, destI, destJ, destRow, destCol,
		isWhiteTurn, endType, EMPTY);
}

/********************************************************************************************************************
* Function name: checkValidRookMove
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         char pgn[](valid values: a normal chess move)
*         int sourceRow(valid values: 0 <= sourceRow <= SIZE - 1
*         int sourceCol(valid values: 0 <= sourceCol <= SIZE - 1
*         int destRow(valid values: 0 <= destRow <= SIZE - 1
*         int destCol(valid values: 0 <= destCol <= SIZE - 1
*		  boolean isCaptureMove(valid values: TRUE/FALSE)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function checks if the rook move from the board[sourceRow][sorceCol] to the
*					  board[destRow][destCol] is a valid move. if yes - return TRUE. otherwise - FALSE
********************************************************************************************************************/
boolean checkValidRookMove(char board[][SIZE], int sourceRow, int sourceCol, int destRow, int destCol, boolean isCaptureMove)
{
	int i, dx, dy;

	if (sourceRow == destRow && sourceCol == destCol)
		return FALSE;

	if (isCellOccupied(board, destRow, destCol) && !isCaptureMove)
		return FALSE;

	if (sourceRow == destRow) {
		dx = (sourceCol < destCol) ? 1 : -1;

		for (i = sourceCol + dx; i != destCol; i += dx)
			if (isCellOccupied(board, sourceRow, i))
				return FALSE;

	}
	else if (sourceCol == destCol) {
		dy = (sourceRow < destRow) ? 1 : -1;

		for (i = sourceRow + dy; i != destRow; i += dy)
			if (isCellOccupied(board, i, sourceCol))
				return FALSE;
	}
	else {
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************************************
* Function name: tryAndMoveCapture
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         char pgn[](valid values: a normal chess move)
*		  boolean isWhiteTurn(valid values: 0/1)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function attempts to make a capture move, based on the move received. the function
*					  validates the move and if it is valid - return TRUE, otherwise - FALSE.
********************************************************************************************************************/
boolean tryAndMoveCapture(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	int destRow, destCol, pgnLength, i, j, startRow = 0, endRow = SIZE - 1, startCol = 0, endCol = SIZE - 1, srcRow;
	int dstI, dstJ;
	char destRowChar, destColChar, movingObject, upperMovingObject, hintChar, hintRow = NO_HINT, hintCol = NO_HINT;
	boolean found;
	EndType endType;

	pgnLength = strlen(pgn);
	endType = None;
	if (pgn[pgnLength - 1] == '#')
		endType = Mate;
	if (pgn[pgnLength - 1] == '+')
		endType = Chess;

	if (endType != None)
	{
		pgnLength -= 1;
	}

	movingObject = pgn[0];
	upperMovingObject = toupper(movingObject);
	destColChar = pgn[pgnLength - 2];
	destRowChar = pgn[pgnLength - 1];

	destCol = destColChar - 'a';
	destRow = destRowChar - '1';

	if (!checkColumnAndRowValid(destColChar, destRowChar))
		return FALSE;

	if (pgnLength == 5)
	{
		hintChar = pgn[1];
		if (hintChar >= 'a' && hintChar <= 'h')
			hintCol = hintChar;
		else if (hintChar >= '1' && hintChar <= '8')
			hintRow = hintChar;
	}

	if (pgnLength == 6)
	{
		hintChar = pgn[2];
		if (hintChar >= 'a' && hintChar <= 'h')
			hintCol = hintChar;
		else if (hintChar >= '1' && hintChar <= '8')
			hintRow = hintChar;
	}

	if (hintCol != NO_HINT)
	{
		startCol = hintCol - 'a';
		endCol = startCol;
	}

	if (hintRow != NO_HINT)
	{
		startRow = hintRow - '1';
		endRow = startRow;
	}

	found = FALSE;
	if (movingObject >= 'a' && movingObject <= 'h')
	{
		dstI = (isWhiteTurn == WHITE) ? (destRow - 1) : (destRow + 1);
		dstJ = movingObject - 'a';
		found = TRUE;
	}
	else
	{
		for (i = startRow; (i <= endRow) && !found; i++)
		{
			for (j = startCol; (j <= endCol) && !found; j++)
			{
				if (isCellOccupied(board, i, j))
				{
					if ((board[i][j] == ((isWhiteTurn) ? WHITE_BISHOP : BLACK_BISHOP)) && upperMovingObject == BISHOP)
					{
						found = checkAllEmptyCellsInDiagonal(board, i, j, destRow, destCol, FALSE);
					}
					else if ((board[i][j] == ((isWhiteTurn) ? WHITE_KING : BLACK_KING)) && upperMovingObject == KING)
					{
						found = (abs(destCol - j) == 1 || abs(destRow - i) == 1);
					}
					else if ((board[i][j] == ((isWhiteTurn) ? WHITE_KNIGHT : BLACK_KNIGHT)) && upperMovingObject == KNIGHT)
					{
						found = checkValidKnightMove(board, i, j, destRow, destCol);
					}
					else if ((board[i][j] == ((isWhiteTurn) ? WHITE_QUEEN : BLACK_QUEEN)) && upperMovingObject == QUEEN)
					{
						found = (checkAllEmptyCellsInDiagonal(board, i, j, destRow, destCol, FALSE) ||
							checkValidRookMove(board, i, j, destRow, destCol, TRUE));
					}
					else if ((board[i][j] == ((isWhiteTurn) ? WHITE_ROOK : BLACK_ROOK)) && upperMovingObject == ROOK)
					{
						found = checkValidRookMove(board, i, j, destRow, destCol, TRUE);
					}
					if (found)
					{
						dstI = i;
						dstJ = j;
					}
				}
			}
		}
	}
	if (!found)
		return FALSE;

	i = dstI;
	j = dstJ;

	if (!found)
		return FALSE;

	return handleCheckAndMateMove(board, i, j, destRow, destCol,
		isWhiteTurn, endType, EMPTY);
}


/********************************************************************************************************************
* Function name: markDangerousSpotFromPawn
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
*		  int color(valid values: WHITE/BLACK)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a pawn)
********************************************************************************************************************/
void markDangerousSpotFromPawn(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int color) {
	int checkedRow, checkedCol;
	if (color == WHITE)
	{
		if (row == SIZE - 1)
			return;
		checkedRow = row + 1;
	}
	else if (color == BLACK)
	{
		if (row == 0)
			return;
		checkedRow = row - 1;
	}

	checkedCol = col - 1;
	if (checkedCol > 0)
	{
		if (!isCellOccupied(board, checkedRow, checkedCol))
			dangerousCellsBoard[checkedRow][checkedCol] = DANGEROUS;
	}
	checkedCol = col + 1;
	if (checkedCol < SIZE)
	{
		if (!isCellOccupied(board, checkedRow, checkedCol))
			dangerousCellsBoard[checkedRow][checkedCol] = DANGEROUS;
	}
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromKnight
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a pawn)
********************************************************************************************************************/
void markDangerousSpotFromKnight(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col)
{
	int i, moveRow, moveCol;

	int rowOffsets[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
	int colOffsets[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

	for (i = 0; i < 8; i++)
	{
		moveRow = row + rowOffsets[i];
		moveCol = col + colOffsets[i];

		if (moveRow < 0 || moveRow > SIZE - 1 || moveCol < 0 || moveCol > SIZE - 1)
			continue;

		if (!isCellOccupied(board, moveRow, moveCol) || (toupper(board[moveRow][moveCol]) == KING))
			dangerousCellsBoard[moveRow][moveCol] = DANGEROUS;
	}
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromKnight
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
*         int kingRow(valid values: 0 <= kingRow <= SIZE - 1)
*         int kingCol(valid values: 0 <= kingCol <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a Rook)
********************************************************************************************************************/
void markDangerousSpotFromRook(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol) {
	int i, j;
	boolean found;

	if (abs(row - kingRow) == 1)
	{
		if (col == kingCol)
			dangerousCellsBoard[row][col] = DANGEROUS;
	}
	if (abs(col - kingCol) == 1)
	{
		if (row == kingRow)
			dangerousCellsBoard[row][col] = DANGEROUS;
	}
	if (col + 1 <= SIZE - 1)
	{
		for (j = col + 1; (j < SIZE); j++)
		{

			if (isCellOccupied(board, row, j) && toupper(board[row][j]) != KING)
				break;
			dangerousCellsBoard[row][j] = DANGEROUS;
		}
	}

	found = FALSE;
	if (col - 1 >= 0)
	{
		for (j = col - 1; (j >= 0); j--)
		{
			if (isCellOccupied(board, row, j) && toupper(board[row][j]) != KING)
				break;
			dangerousCellsBoard[row][j] = DANGEROUS;
		}
	}

	found = FALSE;
	if (row - 1 >= 0)
	{
		for (i = row - 1; (i >= 0); i--)
		{
			if (isCellOccupied(board, i, col) && toupper(board[i][col]) != KING)
				break;
			dangerousCellsBoard[i][col] = DANGEROUS;
		}
	}

	if (row + 1 <= SIZE - 1)
	{
		for (i = row + 1; (i < SIZE); i++)
		{
			if (isCellOccupied(board, i, col) && toupper(board[i][col]) != KING)
				break;
			dangerousCellsBoard[i][col] = DANGEROUS;
		}
	}
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromKing
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a enemy King)
********************************************************************************************************************/
void markDangerousSpotFromKing(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col) {
	int i, nextRow, nextCol;
	int rowOffsets[9] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
	int colOffset[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	for (i = 0; i < 9; i++)
	{
		nextRow = row + rowOffsets[i];
		nextCol = col + colOffset[i];

		if (nextRow < 0 || nextRow >= SIZE || nextCol < 0 || nextCol >= SIZE)
			continue;

		if (!isCellOccupied(board, nextRow, nextCol))
			dangerousCellsBoard[nextRow][nextCol] = DANGEROUS;
	}
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromBishopInOneDirection
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int rowDelta(valid values: -1 / 1)
*         int colDelta(valid values: -1 / 1)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
*         int kingRow (valid values: 0 <= kingRow <= SIZE - 1)
*         int kingCol (valid values: 0 <= kingCol <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a a bishop, to a given direction)
********************************************************************************************************************/
void markDangerousSpotFromBishopInOneDirection(char board[][SIZE], char dangerousCellsBoard[][SIZE], int rowDelta, int colDelta, int row, int col, int kingRow, int kingCol)
{
	int i, newRow, newColumn;
	dangerousCellsBoard[row][col] = DANGEROUS;
	if (abs(kingRow - row) == abs(kingCol - col))
		dangerousCellsBoard[row][col] = DANGEROUS;
	for (i = 1; i < SIZE; i++)
	{
		int newRow = row + rowDelta * i;
		int newColumn = col + colDelta * i;
		if (newRow < 0 || newRow > SIZE - 1 || newColumn < 0 || newColumn > SIZE - 1)
		{
			break;
		}
		if (isCellOccupied(board, newRow, newColumn) && toupper(board[newRow][newColumn]) != KING)
			break;
		dangerousCellsBoard[newRow][newColumn] = DANGEROUS;
	}
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromBishop
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
*         int kingRow (valid values: 0 <= kingRow <= SIZE - 1)
*         int kingCol (valid values: 0 <= kingCol <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of a a bishop, to a all diagonals)
********************************************************************************************************************/
void markDangerousSpotFromBishop(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol) {
	markDangerousSpotFromBishopInOneDirection(board, dangerousCellsBoard, -1, -1, row, col, kingRow, kingCol);
	markDangerousSpotFromBishopInOneDirection(board, dangerousCellsBoard, 1, -1, row, col, kingRow, kingCol);
	markDangerousSpotFromBishopInOneDirection(board, dangerousCellsBoard, 1, 1, row, col, kingRow, kingCol);
	markDangerousSpotFromBishopInOneDirection(board, dangerousCellsBoard, -1, 1, row, col, kingRow, kingCol);
}

/********************************************************************************************************************
* Function name: markDangerousSpotFromQueen
* Input:  char board[][SIZE](valid values: 2d array of any chess pieces)
*         char dangerousCellsBoard[][SIZE]((valid values: 2d array)
*         int row(valid values: 0 <= row <= SIZE - 1)
*         int col(valid values: 0 <= col <= SIZE - 1)
*         int kingRow (valid values: 0 <= kingRow <= SIZE - 1)
*         int kingCol (valid values: 0 <= kingCol <= SIZE - 1)
* Output: None
* Function Operation: The function marks the dangerous cells on dangerousCellsBoard that are dangerous for the king
*                     with the given color(from the perspective of queen)
********************************************************************************************************************/
void markDangerousSpotFromQueen(char board[][SIZE], char dangerousCellsBoard[][SIZE], int row, int col, int kingRow, int kingCol) {
	markDangerousSpotFromBishop(board, dangerousCellsBoard, row, col, kingRow, kingCol);
	markDangerousSpotFromRook(board, dangerousCellsBoard, row, col, kingRow, kingCol);
}

/********************************************************************************************************************
* Function name: tryAndMovePromotion
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         char pgn[](valid values: a normal chess move)
*		  boolean isWhiteTurn(valid values: 0/1)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function attempts to make a Promotion move, based on the move received. the function
*					  validates the move and if it is valid - return TRUE, otherwise - FALSE.
********************************************************************************************************************/
boolean tryAndMovePromotion(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	int dstRow, dstCol, srcRow, pgnLength;
	EndType endType;
	char toObject;

	if (!checkColumnAndRowValid(pgn[0], pgn[1]))
		return FALSE;
	endType = None;
	pgnLength = strlen(pgn);

	if (pgn[pgnLength - 1] == '#')
		endType = Mate;
	else if (pgn[pgnLength - 1] == '+')
		endType = Chess;

	dstCol = pgn[0] - 'a';
	dstRow = pgn[1] - '1';
	toObject = (isWhiteTurn == WHITE) ? toupper(pgn[3]) : tolower(pgn[3]);

	if (isCellOccupied(board, dstRow, dstCol))
		return FALSE;

	srcRow = (isWhiteTurn == WHITE) ? (dstRow - 1) : (dstRow + 1);

	if (isWhiteTurn == WHITE)
	{
		if (dstRow != SIZE - 1)
			return FALSE;

		if (board[srcRow][dstCol] != WHITE_PAWN)
			return FALSE;
	}
	else if (isWhiteTurn == BLACK)
	{
		if (dstRow != 0)
			return FALSE;

		if (board[srcRow][dstCol] != BLACK_PAWN)
			return FALSE;
	}

	return handleCheckAndMateMove(board, srcRow, dstCol, dstRow, dstCol,
		isWhiteTurn, endType, toObject);
}

/********************************************************************************************************************
* Function name: tryAndMovePawn
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*         char pgn[](valid values: a normal chess move)
*		  boolean isWhiteTurn(valid values: 0/1)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function attempts to make a Pawn move, based on the move received. the function
*					  validates the move and if it is valid - return TRUE, otherwise - FALSE.
********************************************************************************************************************/
boolean tryAndMovePawn(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	int row, validDistance, destCol, destRow;
	boolean found;
	EndType endType;

	if (!checkColumnAndRowValid(pgn[0], pgn[1]))
		return FALSE;

	endType = None;
	destCol = pgn[0] - 'a';
	destRow = pgn[1] - '1';

	if (strlen(pgn) > 2)
		endType = (pgn[2] == '#') ? Mate : Chess;

	if (isCellOccupied(board, destRow, destCol))
		return FALSE;

	int foundRow;
	if (isWhiteTurn == WHITE)
	{
		found = FALSE;
		validDistance = (destRow - 2 == WHITE_PAWN_STARTING_ROW) ? 2 : 1;
		for (row = destRow; (row >= destRow - validDistance) && !found; row--)
		{
			if ((isCellOccupied(board, row, destCol)) && board[row][destCol] == WHITE_PAWN)
			{
				found = TRUE;
				foundRow = row;
			}
		}
	}
	else
	{
		found = FALSE;
		validDistance = (destRow + 2 == BLACK_PAWN_STARTING_ROW) ? 2 : 1;
		for (row = destRow; (row <= destRow + validDistance) && !found; row++)
		{
			if ((isCellOccupied(board, row, destCol)) && board[row][destCol] == BLACK_PAWN)
			{
				found = TRUE;
				foundRow = row;
			}
		}
	}
	if (!found)
		return FALSE;

	return handleCheckAndMateMove(board, foundRow, destCol, destRow, destCol,
		isWhiteTurn, endType, EMPTY);
}

/********************************************************************************************************************
* Function name: getKingNumberOfOptionalMoves
* Input: int kingRow(valid values: 0 <= kingRow <= SIZE - 1)
*        int kingCol(valid values: 0 <= kingRow <= SIZE - 1)
* Output: int(0 - 8)
* Function Operation: The function returns the number of cells the king in the given postion can advance to
********************************************************************************************************************/
int getKingNumberOfOptionalMoves(int kingRow, int kingCol)
{
	int i, moveRow, moveCol, optionalMoves = 0;
	int rowOffsets[9] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
	int colOffsets[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	for (i = 0; i < 9; i++)
	{
		moveRow = kingRow + rowOffsets[i];
		moveCol = kingCol + colOffsets[i];

		if (moveRow < 0 || moveRow > SIZE - 1 || moveCol < 0 || moveCol > SIZE - 1)
			continue;
		optionalMoves++;
	}
	return optionalMoves;
}

/********************************************************************************************************************
* Function name: getKingNumberOfDangerousAroundKing
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*        char dangerousCellsBoard[](valid values: 2d array of char)
*        int kingRow(valid values: 0 <= kingRow <= SIZE - 1)
*        int kingCol(valid values: 0 <= kingRow <= SIZE - 1)
* Output: int(0-8)
* Function Operation: The function returns the number of dangerous cells around a given king position.
********************************************************************************************************************/
int getKingNumberOfDangerousAroundKing(char dangerousCellsBoard[SIZE][SIZE], int kingRow, int kingCol)
{
	int i, moveRow, moveCol, dangerousSpots = 0;
	int rowOffsets[9] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
	int colOffsets[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	for (i = 0; i < 9; i++)
	{
		moveRow = kingRow + rowOffsets[i];
		moveCol = kingCol + colOffsets[i];

		if (moveRow < 0 || moveRow > SIZE - 1 || moveCol < 0 || moveCol > SIZE - 1)
			continue;
		if (dangerousCellsBoard[moveRow][moveCol] == DANGEROUS)
			dangerousSpots++;
	}
	return dangerousSpots;
}

/********************************************************************************************************************
* Function name: markDangerousSpots
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*        char dangerousCellsBoard[](valid values: 2d array of char)
*        int kingRow(valid values: 0 <= kingRow <= SIZE - 1)
*        int kingCol(valid values: 0 <= kingRow <= SIZE - 1)
*	     int oppositeColor(valid values: WHITE/TRUE)
* Output: None
* Function Operation: The function marks on dangerousCellBoard 2d array with the value of DANGEROUS, all the cells
*                     that are dangerous for the king given in the given position(kingRow, kingCol)
********************************************************************************************************************/
void markDangerousSpots(char board[][SIZE], char dangerousCellsBoard[SIZE][SIZE], int kingRow, int kingCol, int oppositeColor)
{
	int i, j;
	char piece;
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			piece = toupper(board[i][j]);
			if (checkSourceObjectIsValid(board, i, j, oppositeColor))
			{
				if (piece == PAWN)
				{
					markDangerousSpotFromPawn(board, dangerousCellsBoard, i, j, oppositeColor);
				}
				else if (piece == KNIGHT)
				{
					markDangerousSpotFromKnight(board, dangerousCellsBoard, i, j);
				}
				else if (piece == QUEEN)
				{
					markDangerousSpotFromQueen(board, dangerousCellsBoard, i, j, kingRow, kingCol);
				}
				else if (piece == BISHOP)
				{
					markDangerousSpotFromBishop(board, dangerousCellsBoard, i, j, kingRow, kingCol);
				}
				else if (piece == ROOK)
				{
					markDangerousSpotFromRook(board, dangerousCellsBoard, i, j, kingRow, kingCol);
				}
				else if (piece == KING)
				{
					markDangerousSpotFromKing(board, dangerousCellsBoard, i, j);
				}
			}
		}
	}
}

/********************************************************************************************************************
* Function name: handleCheckAndMateMove
* Input: char board[][SIZE](valid values: 2d array of any chess pieces)
*        int srcRow(valid values: 0 <= srcRow <= SIZE - 1)
*        int srcCol(valid values: 0 <= srcCol <= SIZE - 1)
*        int dstRow(valid values: 0 <= dstRow <= SIZE - 1)
*        int dstCol(valid values: 0 <= dstCol <= SIZE - 1)
*	     int attackingColor(valid values: WHITE/TRUE)
*	     EndType endType(valid values: Mate/Check/None)
*        char promotion(valid values: any char)
* Output: boolean(TRUE/FALSE)
* Function Operation: The function makes a chess move from board[srcRow][srcCol] to board[dstRow][dstCol], for the
*					  given attacking color, validating if the move has chess/mate(or none), and returns accordingly
*					  if the move was valid. if it was - return TRUE, otherwise - FALSE
********************************************************************************************************************/
boolean handleCheckAndMateMove(char board[][SIZE], int srcRow, int srcCol, int dstRow, int dstCol, int attackingColor, EndType endType, char promotionPiece)
{
	int kingRow, kingCol, i, j, oppositeColor, availableKingMoves, kingOptionalMovesCounter = 0, kingNumberOfCellsDangerous = 0;
	boolean isCheck, isMate;
	char dangerousCellsBoard[SIZE][SIZE];
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			dangerousCellsBoard[i][j] = OK;
			if (board[i][j] == ((attackingColor == BLACK) ? WHITE_KING : BLACK_KING))
			{
				kingRow = i;
				kingCol = j;
			}
		}
	}
	moveSrcToDest(board, srcRow, srcCol, dstRow, dstCol, ((promotionPiece == EMPTY) ? board[srcRow][srcCol] : promotionPiece));
	markDangerousSpots(board, dangerousCellsBoard, kingRow, kingCol, attackingColor);

	kingOptionalMovesCounter = getKingNumberOfOptionalMoves(kingRow, kingCol);
	kingNumberOfCellsDangerous = getKingNumberOfDangerousAroundKing(dangerousCellsBoard, kingRow, kingCol);

	availableKingMoves = kingOptionalMovesCounter - kingNumberOfCellsDangerous;

	isCheck = ((dangerousCellsBoard[kingRow][kingCol] == DANGEROUS) && (availableKingMoves > 0));
	isMate = (((dangerousCellsBoard[kingRow][kingCol] != DANGEROUS) && (availableKingMoves <= 1)) ||
		((dangerousCellsBoard[kingRow][kingCol] == DANGEROUS) && (availableKingMoves == 0)));

	if (((endType == Chess) && (isMate || !isCheck)) ||
		((endType == None) && (isCheck || isMate)) ||
		((endType == Mate) && (!isMate || isCheck)))
	{
		revertMove(board, srcRow, srcCol, dstRow, dstCol, board[dstRow][dstCol], board[srcRow][srcCol]);
		return FALSE;
	}
	return TRUE;
}