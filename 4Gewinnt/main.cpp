/*
	Copyright (c) 2024 Mark Narain Enzinger

	MIT License (https://github.com/marknarain/4Gewinnt/blob/main/LICENSE)
*/

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <windows.h>

#define DIR_LEFT		0
#define DIR_RIGHT		1

#define ROWS			6
#define COLUMNS			7
#define PLAYER1_COLOR	9
#define PLAYER2_COLOR	12
#define WHITE			15

#define OK				16
#define COLUMN_FILLED	17
#define INVALID_MOVE	18

#define COIN_SYMBOL		"o"

using namespace std;

vector<vector<int>> board(COLUMNS, vector<int>(ROWS, 0));
vector<int> fall(COLUMNS);

int  dropCoin(int column, int player);
int  check4inRow();
void playerMove(int player);
void gotoxy(int column, int line);
void buildBoard();
void printBoard(int stX, int stY);
void clearBoard(int stX, int stY); 
void moveBoard(int distance, bool direction);
void clearLine(int line);
void updateFall(int x);
void error(int errorCode);
void coinDropAnimation();
int checkLine(int i, int j, int s1, int s2);

int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, WHITE);

	buildBoard();

	int player = 0;

	while (check4inRow() == 0)
	{
		SetConsoleTextAttribute(hConsole, WHITE);
		playerMove(player + 1);

		player = !player;
	}
	
	gotoxy(0, 9);	
	cout << "Player " << check4inRow() << " won the game!!" << std::string(80, ' ') << "\n";

	Sleep(500);

	moveBoard(2 * COLUMNS + 2, DIR_RIGHT);
	coinDropAnimation();
	board.resize(COLUMNS, vector<int>());

	moveBoard(2 * COLUMNS + 1, DIR_LEFT);

	gotoxy(0, 7);
	cout << string(2 * COLUMNS + 1, '-') << endl;

	SetConsoleTextAttribute(hConsole, WHITE);
}

/// <summary>
/// Updates the board array and animates dropping the coin
/// </summary>
/// <param name="column"> : The column in which the coin should be dropped</param>
/// <param name="player"> : Which player dropped the coin (because of color)</param>
/// <returns>Errorcode</returns>
int dropCoin(int column, int player)
{
	int row = ROWS;

	if (board[column - 1][0] != 0)
	{
		return COLUMN_FILLED;
	}

	// calculate, where to drop the coin
	while (board[column-1][row - 1] != 0)
	{
		row--;
	}
	board[column - 1][row - 1] = player;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	if (player == 1)
	{
		SetConsoleTextAttribute(hConsole, PLAYER1_COLOR);
	}
	else if (player == 2)
	{
		SetConsoleTextAttribute(hConsole, PLAYER2_COLOR);
	}

	// animate dropping the coin
	for (int i = 1; i < row; i++)
	{
		gotoxy(column + column - 1, i);
		cout << COIN_SYMBOL;
		Sleep(50);
		gotoxy(column + column - 1, i);
		cout << " ";
	}

	// place the coin at its final location
	gotoxy(column + column - 1, row);
	cout << COIN_SYMBOL;

	SetConsoleTextAttribute(hConsole, WHITE);

	return OK;
}

/// <summary>
/// Checks if 4 coins are in row either horizontally or vertically in all directions
/// </summary>
/// <returns>The player, who won or 0 if no 4 were found</returns>
int check4inRow()
{
	for (int i = 0; i < COLUMNS; i++)
	{
		for (int j = 0; j < ROWS; j++)
		{
			if (board[i][j] != 0)
			{
				if (i < 4 && j < 3 && checkLine(i, j, 1, 1))
				{
					return board[i][j];
				}
				if (i > 2 && j < 3 && checkLine(i, j, 1, -1))
				{
					return board[i][j];
				}
				if (i > 2 && j > 2 && checkLine(i, j, -1, -1))
				{
					return board[i][j];
				}
				if (i < 4 && j > 2 && checkLine(i, j, -1, 1))
				{
					return board[i][j];
				}
			}
		}
	}
	return 0;
}

int checkLine(int i, int j, int s1, int s2)
{
	if (board[i][j] == board[i][j + 1*s1] && board[i][j] == board[i][j + 2*s1] && board[i][j] == board[i][j + 3*s1] ||
		board[i][j] == board[i + 1*s2][j + 1*s1] && board[i][j] == board[i + 2*s2][j + 2*s1] && board[i][j] == board[i + 3*s2][j + 3*s1] ||
		board[i][j] == board[i + 1*s2][j] && board[i][j] == board[i + 2*s2][j] && board[i][j] == board[i + 3*s2][j])
	{
		return 1;
	}
	return 0;
}

/// <summary>
/// Updates the fall array with the new offset for printing
/// </summary>
/// <param name="x">The x location, where the array begins</param>
void updateFall(int x)
{
	// Fall 
	// (y - 1) / 2, because y is the location of the first pipes -> when the first pipe reaches x location 3 one field from the back has to start falling.
	for (int i = (x - 1) / 2; i > 0; i--)
	{
		if (fall[COLUMNS - i] < 20)
		{
			fall[COLUMNS - i]++;
		}
	}
}

/// <summary>
/// Moves the board by a certain distance in a certain direction
/// </summary>
/// <param name="distance"> : Defines how far the board should be moved</param>
/// <param name="direction"> : Defines the direction, in which the board moves</param>
void moveBoard(int distance,bool direction)
{
	if (direction == DIR_RIGHT)
	{
		for (int i = 0; i < distance; i++)
		{
			clearBoard(i, 1);
			gotoxy(0, 7);
			cout << "\033[J";
			cout << string(2 * COLUMNS + 1, '-') << endl;
			updateFall(i + 1);
			printBoard(i, 2);
			Sleep(125);
		}
	}
	else if (direction == DIR_LEFT)
	{
		for (int i = distance; i >= 0; i--)
		{
			clearBoard(i, 1);
			gotoxy(0, 7);
			cout << "\033[J";
			cout << string(2 * COLUMNS + 1, '-') << endl;
			updateFall(i + 1);
			printBoard(i, 2);
			Sleep(125);
		}
	}
	
	gotoxy(0, 8);
	cout << "\033[J";

}

/// <summary>
/// Animates the coins falling down
/// </summary>
void coinDropAnimation()
{
	for (int i = 16; i < 35; i++)
	{
		clearBoard(2 * COLUMNS + 1, 1);
		gotoxy(0, 7);
		cout << "\033[J";
		cout << string(2 * COLUMNS + 1, '-') << endl;
		updateFall(2 * COLUMNS + 1 + 1);
		printBoard(2 * COLUMNS + 1, 2);
		Sleep(125);
	}
}

/// <summary>
/// Sets the cursor at a certain position in the terminal. The coordinates start in the top left corner.
/// </summary>
/// <param name="column"> : The x coordinate</param>
/// <param name="line"> : The y coordinate</param>
void gotoxy(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
}

/// <summary>
/// Prints the board with numbers and bottom line at the starting position
/// </summary>
void buildBoard()
{
	cout << " ";
	for (int i = 1; i <= COLUMNS; i++)
	{
		cout << i << " ";
	}
	cout << "\n";

	printBoard(0, 2);
	cout << string(2 * COLUMNS + 1, '-') << endl;
}

/// <summary>
/// Clears the line "line"
/// </summary>
/// <param name="line"> : The line to clear</param>
void clearLine(int line)
{
	gotoxy(0, line);
	cout << std::string(80, ' ');
	gotoxy(0, line);
}

/// <summary>
/// Clears a board at a certain location
/// </summary>
/// <param name="stX"> : ...defines the x coordinate of the boards left top corner</param>
/// <param name="stY"> : ...defines the y coordinate of the boards left top corner</param>
void clearBoard(int stX, int stY)
{
	for (int i = 0; i < ROWS; i++)
	{
		clearLine(stY+i);
		gotoxy(stX, stY + i);
	}
}

/// <summary>
/// Prints the board with its contents. 
/// </summary>
/// <param name="stX"> : ...defines the x coordinate of the boards left top corner</param>
/// <param name="stY"> : ...defines the y coordinate of the boards left top corner</param>
void printBoard(int stX, int stY)
{
	gotoxy(stX, stY-1);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, WHITE);

	for (int i = 0; i < ROWS; i++)
	{
		cout << "|";
		for (int j = 0; j < COLUMNS; j++)
		{
			// x direction is calculated by *2-1 because of the pipes. y direction is determined by how far the coins have fallen.
			gotoxy((j+1) * 2 - 1 + stX, i + 1 + fall[j]);
			
			if (fall[j] < 19)
			{
				switch (board[j][i])
				{
				case 0:
					cout << " ";
					break;
				case 1:
					SetConsoleTextAttribute(hConsole, PLAYER1_COLOR);
					cout << COIN_SYMBOL;
					SetConsoleTextAttribute(hConsole, WHITE);
					break;
				case 2:
					SetConsoleTextAttribute(hConsole, PLAYER2_COLOR);
					cout << COIN_SYMBOL;
					SetConsoleTextAttribute(hConsole, WHITE);
					break;
				}
			}

			gotoxy((j + 1) * 2 + stX, i + 1);
			cout << "|";
		}
		gotoxy(stX, stY + i);
	}
}

/// <summary>
/// Asks a player for input and calls the "dropCoin" function
/// </summary>
/// <param name="player"> : Which players turn it is</param>
void playerMove(int player)
{
	clearLine(9);

	string move;
	cout << "Player " << player << ", please enter your move : ";
	cin >> move;
	if (move < "1" || move > to_string(COLUMNS))
	{
		error(INVALID_MOVE);
		playerMove(player);
		return;
	}

	clearLine(10);

	int err = dropCoin(move[0] - '0', player);

	if (err == COLUMN_FILLED)
	{
		error(COLUMN_FILLED);
		playerMove(player);
		return;
	}
}

/// <summary>
/// Prints out errors based on the errorCode
/// </summary>
/// <param name="errorCode"></param>
void error(int errorCode)
{
	switch (errorCode)
	{
		case COLUMN_FILLED:
			cout << "This column is already filled!! Enter a different move:" << endl;
			break;
		case INVALID_MOVE:
			cout << "Invalid move. Please enter a number between 1 and 7." << endl;
			break;
	}
}