//TETRIS IMITATION, GUIDELINE CONFORMING
//MADE BY TEODOR POMOHACI
//
//The goal of this program is to make a minimal tetris game that follows the 2009 Tetris Guideline, at least in the gameplay point of view.

//Includes
#include <iostream>
#include <windows.h>
#include <ctime>
#include <stdlib.h>
#include <conio.h> //might use it for input performance
using namespace std;

//Constants
int CELL_WIDTH = 2;
int O = 1,
    I = 2,
    T = 3,
    L = 4,
    J = 5,
    S = 6,
    Z = 7;
int DISPLAY_HEIGHT = 22;
int PADDING_TOP = 5;
int PADDING_LEFT = 20;
int rN = 0,
    rE = 1,
    rS = 2,
    rW = 3;
int USING_HOLD = 1;
char MINO_TEXTURE[2];
char GHOST_TEXTURE[2];
char TETRIMINO_TEXTURE[2];
char EMPTY_TEXTURE[2];
//Required variables
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD extra;
int buffer[41][11];
int prevbuffer[41][11]; //using the double buffering printing technique
struct Mino{
	int x, y;
};
struct Tetrimino{
	int type;
	Mino mino[4];
	int facing;
}tetrimino, prevtetrimino, ghosttetrimino, prevghosttetrimino;
int nextQueue[100];
int holdQueue;
int lose, lock;
int delta = 500;
int DASdelta = 133;
int DASPrevRight;
int DASPrevLeft;
int hold_check;
int linesCleared[40];
//Functions
void ShuffleTetriminos(int nextQueue[100], int a, int b){
	int r1, r2;
	int aux;
	for(int i=0;i<75;i++){
		r1 = rand()%7 + a;
		r2 = rand()%7 + a;
		aux = nextQueue[r1];
		nextQueue[r1] = nextQueue[r2];
		nextQueue[r2] = aux;
	}
}
void GenerateBag(int nextQueue[100]){
	if(nextQueue[0] == 0){
		for(int i=0;i<7;i++)
			nextQueue[i] = i+1; //Check the names of tetriminoes at Constants
		ShuffleTetriminos(nextQueue, 0, 6);
		for(int i=0;i<7;i++)
			nextQueue[i+7] = i+1;
		ShuffleTetriminos(nextQueue, 7, 13);
	}
	else if(nextQueue[7] == 0){
		for(int i=0;i<7;i++)
			nextQueue[i+7] = i+1;
		ShuffleTetriminos(nextQueue, 7, 13);
	}
}
void GenerateTetrimino(Tetrimino &tetrimino, int nextQueue[100], int &holdQueue, int isHold = 0){
	if(isHold){
		if(holdQueue == 0){
			holdQueue = tetrimino.type;
			GenerateTetrimino(tetrimino, nextQueue, holdQueue);
		}
		else{
			int aux = tetrimino.type;
			tetrimino.type = holdQueue;
			holdQueue = aux;
		}
	}
	else tetrimino.type = nextQueue[0];
	tetrimino.facing = rN;
	if(isHold == 0) for(int i=0;i<99;i++){
		nextQueue[i] = nextQueue[i+1];
	}
	//wanted to use switch, but cases cannot handle non-constant values
	if(tetrimino.type == O){
		tetrimino.mino[0].x = 5;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 6;tetrimino.mino[1].y = 22;
		tetrimino.mino[2].x = 5;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 6;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == I){
		tetrimino.mino[0].x = 4;tetrimino.mino[0].y = 21;
		tetrimino.mino[1].x = 5;tetrimino.mino[1].y = 21;
		tetrimino.mino[2].x = 6;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 7;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == T){
		tetrimino.mino[0].x = 5;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 4;tetrimino.mino[1].y = 21;
		tetrimino.mino[2].x = 5;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 6;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == L){
		tetrimino.mino[0].x = 6;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 4;tetrimino.mino[1].y = 21;
		tetrimino.mino[2].x = 5;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 6;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == J){
		tetrimino.mino[0].x = 4;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 4;tetrimino.mino[1].y = 21;
		tetrimino.mino[2].x = 5;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 6;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == S){
		tetrimino.mino[0].x = 5;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 6;tetrimino.mino[1].y = 22;
		tetrimino.mino[2].x = 4;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 5;tetrimino.mino[3].y = 21;
	}
	else if(tetrimino.type == Z){
		tetrimino.mino[0].x = 4;tetrimino.mino[0].y = 22;
		tetrimino.mino[1].x = 5;tetrimino.mino[1].y = 22;
		tetrimino.mino[2].x = 5;tetrimino.mino[2].y = 21;
		tetrimino.mino[3].x = 6;tetrimino.mino[3].y = 21;
	}
}
void DisplayNextQueue(int nextQueue[100]){
	for(int i=0;i<5;i++){
		if(nextQueue[i] == O){
			WriteConsoleOutputCharacter(h, "[][]    ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][]    ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == I){
			WriteConsoleOutputCharacter(h, "        ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][][][]", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == T){
			WriteConsoleOutputCharacter(h, "  []    ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][][]  ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == L){
			WriteConsoleOutputCharacter(h, "    []  ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][][]  ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == J){
			WriteConsoleOutputCharacter(h, "[]      ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][][]  ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == S){
			WriteConsoleOutputCharacter(h, "  [][]  ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "[][]    ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
		else if(nextQueue[i] == Z){
			WriteConsoleOutputCharacter(h, "[][]    ", 8, {15*2 + PADDING_LEFT, (3*i)+5 + PADDING_TOP}, &extra);
			WriteConsoleOutputCharacter(h, "  [][]  ", 8, {15*2 + PADDING_LEFT, (3*i)+6 + PADDING_TOP}, &extra);
		}
	}
}
void DisplayHoldQueue(int &holdQueue){
	if(holdQueue == O){
		WriteConsoleOutputCharacter(h, "[][]    ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][]    ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == I){
		WriteConsoleOutputCharacter(h, "        ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][][][]", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == T){
		WriteConsoleOutputCharacter(h, "  []    ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][][]  ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == L){
		WriteConsoleOutputCharacter(h, "    []  ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][][]  ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == J){
		WriteConsoleOutputCharacter(h, "[]      ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][][]  ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == S){
		WriteConsoleOutputCharacter(h, "  [][]  ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "[][]    ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
	else if(holdQueue == Z){
		WriteConsoleOutputCharacter(h, "[][]    ", 8, {-9 + PADDING_LEFT, 5 + PADDING_TOP}, &extra);
		WriteConsoleOutputCharacter(h, "  [][]  ", 8, {-9 + PADDING_LEFT, 6 + PADDING_TOP}, &extra);
	}
}
void FallTetrimino(Tetrimino &tetrimino, int buffer[41][11], int delta, int &previous, int &lock){
	if(clock()-previous > delta){
		previous = clock();
		bool check = 1;
		for(int i=0;i<4;i++)
			if(tetrimino.mino[i].y-1 == 0 || buffer[tetrimino.mino[i].y-1][tetrimino.mino[i].x] != 0)
				check = 0;
		if(check)
			for(int i=0;i<4;i++)
				tetrimino.mino[i].y--;
		//else lock = 0;
	}
}
void RefreshScreen(int buffer[41][11], int prevbuffer[41][11], Tetrimino tetrimino, Tetrimino prevtetrimino, Tetrimino ghosttetrimino, Tetrimino prevghosttetrimino){
	for(int i=1;i<=22;i++){
		for(int j=1;j<=10;j++){
			if(buffer[i][j] != prevbuffer[i][j]){
				if(buffer[i][j] == 0)
					WriteConsoleOutputCharacter(h, EMPTY_TEXTURE, 2, {j*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-i + PADDING_TOP}, &extra);
				else if(buffer[i][j] == 1)
					WriteConsoleOutputCharacter(h, MINO_TEXTURE, 2, {j*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-i + PADDING_TOP}, &extra);
			}
			prevbuffer[i][j] = buffer[i][j];
		}
	}
	bool check = 1;
	bool ghostcheck = 1;
	for(int k=0;k<4;k++){
		WriteConsoleOutputCharacter(h, TETRIMINO_TEXTURE, 2, {tetrimino.mino[k].x*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-tetrimino.mino[k].y + PADDING_TOP}, &extra);
		for(int l=0;l<4;l++)
			if(prevtetrimino.mino[k].y == tetrimino.mino[l].y && prevtetrimino.mino[k].x == tetrimino.mino[l].x)
				check = 0;
		if(check)
			WriteConsoleOutputCharacter(h, EMPTY_TEXTURE, 2, {prevtetrimino.mino[k].x*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-prevtetrimino.mino[k].y + PADDING_TOP}, &extra);
		else check = 1;

		if(tetrimino.mino[k].y != ghosttetrimino.mino[k].y && prevtetrimino.mino[k].y != ghosttetrimino.mino[k].y)
			WriteConsoleOutputCharacter(h, GHOST_TEXTURE, 2, {ghosttetrimino.mino[k].x*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-ghosttetrimino.mino[k].y + PADDING_TOP}, &extra);
		for(int l=0;l<4;l++)
			if(prevghosttetrimino.mino[k].y == ghosttetrimino.mino[l].y && prevghosttetrimino.mino[k].x == ghosttetrimino.mino[l].x)
				ghostcheck = 0;
		if(ghostcheck)
			WriteConsoleOutputCharacter(h, EMPTY_TEXTURE, 2, {prevghosttetrimino.mino[k].x*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-prevghosttetrimino.mino[k].y + PADDING_TOP}, &extra);
		else ghostcheck = 1;
	}
}
void Harddrop(Tetrimino &tetrimino, int buffer[41][11], int &lock){
	while(lock){
		bool check = 1;
		for(int i=0;i<4;i++)
			if(tetrimino.mino[i].y-1 == 0 || buffer[tetrimino.mino[i].y-1][tetrimino.mino[i].x] != 0)
				check = 0;
		if(check)
			for(int i=0;i<4;i++)
				tetrimino.mino[i].y--;
		else lock = 0;
	}
}
void Softdrop(Tetrimino &tetrimino, int buffer[41][11], int &lock){
	while(lock){
		bool check = 1;
		for(int i=0;i<4;i++)
			if(tetrimino.mino[i].y-1 == 0 || buffer[tetrimino.mino[i].y-1][tetrimino.mino[i].x] != 0)
				check = 0;
		if(check)
			for(int i=0;i<4;i++)
				tetrimino.mino[i].y--;
		else lock = 0;
	}
	lock = 1;
}
void MoveRight(Tetrimino &tetrimino, int buffer[41][11]){
	int check = 1;
	for(int i=0;i<4;i++)
		if(tetrimino.mino[i].x == 10 || buffer[tetrimino.mino[i].y][tetrimino.mino[i].x+1] == 1)
			check = 0;
	if(check)
		for(int i=0;i<4;i++)
			tetrimino.mino[i].x++;
}
void MoveLeft(Tetrimino &tetrimino, int buffer[41][11]){
	int check = 1;
	for(int i=0;i<4;i++)
		if(tetrimino.mino[i].x == 1 || buffer[tetrimino.mino[i].y][tetrimino.mino[i].x-1] == 1)
			check = 0;
	if(check)
		for(int i=0;i<4;i++)
			tetrimino.mino[i].x--;
}
void DASRight(Tetrimino &tetrimino, int buffer[41][11], int &DASPrevRight, int DASdelta){
	if(!GetAsyncKeyState('F'))
		DASPrevRight = clock();
	else if(clock() - DASPrevRight > DASdelta){
		int check = 1;
		for(int i=0;i<4;i++)
			if(tetrimino.mino[i].x == 10 || buffer[tetrimino.mino[i].y][tetrimino.mino[i].x+1] == 1)
				check = 0;
		if(check)
			for(int i=0;i<4;i++)
				tetrimino.mino[i].x++;
	}
}
void DASLeft(Tetrimino &tetrimino, int buffer[41][11], int &DASPrevLeft, int DASdelta){
	if(!GetAsyncKeyState('S'))
		DASPrevLeft = clock();
	else if(clock() - DASPrevLeft > DASdelta){
		int check = 1;
		for(int i=0;i<4;i++)
			if(tetrimino.mino[i].x == 1 || buffer[tetrimino.mino[i].y][tetrimino.mino[i].x-1] == 1)
				check = 0;
		if(check)
			for(int i=0;i<4;i++)
				tetrimino.mino[i].x--;
	}
}
bool Shift(Tetrimino &tetrimino, int buffer[41][11], Mino m[4], Mino offset){
	for(int i=0;i<4;i++){
		if(buffer[m[i].y+offset.y][m[i].x+offset.x]==1 || m[i].y+offset.y<=0 || m[i].x+offset.x<=0 || m[i].x+offset.x>10){
			return 0;
		}
	}
	for(int i=0;i<4;i++){
		tetrimino.mino[i].x = m[i].x+offset.x;
		tetrimino.mino[i].y = m[i].y+offset.y;
	}
	return 1;
}
void Rotate3x2NRight(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1,  1})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, { 0, -2})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1, -2})) tetrimino.facing = rE;
}
void Rotate3x2ERight(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, { 1, -1})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, { 0,  2})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, { 1,  2})) tetrimino.facing = rS;
}
void Rotate3x2SRight(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1,  1})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 0, -2})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1, -2})) tetrimino.facing = rW;
}
void Rotate3x2WRight(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, {-1, -1})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, { 0,  2})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, {-1,  2})) tetrimino.facing = rN;
}
/////////////////////////////////////////////////////
void Rotate3x2NLeft(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1,  1})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 0, -2})) tetrimino.facing = rW;
	else if(Shift(tetrimino, buffer, m, { 1, -2})) tetrimino.facing = rW;
}
void Rotate3x2ELeft(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, { 1, -1})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, { 0,  2})) tetrimino.facing = rN;
	else if(Shift(tetrimino, buffer, m, { 1,  2})) tetrimino.facing = rN;
}
void Rotate3x2SLeft(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1,  1})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, { 0, -2})) tetrimino.facing = rE;
	else if(Shift(tetrimino, buffer, m, {-1, -2})) tetrimino.facing = rE;
}
void Rotate3x2WLeft(Tetrimino &tetrimino, int buffer[41][11], Mino m[4]){
	     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, {-1, -1})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, { 0,  2})) tetrimino.facing = rS;
	else if(Shift(tetrimino, buffer, m, {-1,  2})) tetrimino.facing = rS;
}
void RotateTetriminoRight(Tetrimino tetrimino, Mino m[4], int center){
	for(int i=0;i<4;i++){
		if(i != center){
			if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y+1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x   && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y){
				m[i].x = tetrimino.mino[center].x  ; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x   && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y+1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y){
				m[i].x = tetrimino.mino[center].x  ; m[i].y = tetrimino.mino[center].y+1;
			}
		}
		else m[center] = tetrimino.mino[center];
	}
}
void RotateTetriminoLeft(Tetrimino tetrimino, Mino m[4], int center){
	for(int i=0;i<4;i++){
		if(i != center){
			if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y){
				m[i].x = tetrimino.mino[center].x  ; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x-1 && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y-1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x   && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y-1){
				m[i].x = tetrimino.mino[center].x+1; m[i].y = tetrimino.mino[center].y+1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y){
				m[i].x = tetrimino.mino[center].x; m[i].y = tetrimino.mino[center].y+1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x+1 && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y+1;
			}
			else if(tetrimino.mino[i].x==tetrimino.mino[center].x && tetrimino.mino[i].y==tetrimino.mino[center].y+1){
				m[i].x = tetrimino.mino[center].x-1; m[i].y = tetrimino.mino[center].y;
			}
		}
		else m[center] = tetrimino.mino[center];
	}
}
void RotateRight(Tetrimino &tetrimino, int buffer[41][11]){
	Mino m[4];
	if(tetrimino.type == I){
		if(tetrimino.facing == rN){
			m[0].x = tetrimino.mino[0].x+2; m[0].y = tetrimino.mino[0].y+1;
			m[1].x = tetrimino.mino[1].x+1; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x  ; m[2].y = tetrimino.mino[2].y-1;
			m[3].x = tetrimino.mino[3].x-1; m[3].y = tetrimino.mino[3].y-2;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2, -1})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1,  2})) tetrimino.facing = rE;
		}
		else if(tetrimino.facing == rE){
			m[0].x = tetrimino.mino[0].x+1; m[0].y = tetrimino.mino[0].y-2;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y-1;
			m[2].x = tetrimino.mino[2].x-1; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x-2; m[3].y = tetrimino.mino[3].y+1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-1,  2})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 2, -1})) tetrimino.facing = rS;
		}
		else if(tetrimino.facing == rS){
			m[0].x = tetrimino.mino[0].x-2; m[0].y = tetrimino.mino[0].y-1;
			m[1].x = tetrimino.mino[1].x-1; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x  ; m[2].y = tetrimino.mino[2].y+1;
			m[3].x = tetrimino.mino[3].x+1; m[3].y = tetrimino.mino[3].y+2;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2,  1})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1, -2})) tetrimino.facing = rW;
		}
		else if(tetrimino.facing == rW){
			m[0].x = tetrimino.mino[0].x-1; m[0].y = tetrimino.mino[0].y+2;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y+1;
			m[2].x = tetrimino.mino[2].x+1; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x+2; m[3].y = tetrimino.mino[3].y-1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 1, -2})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-2,  1})) tetrimino.facing = rN;
		}
	}
	else if(tetrimino.type == T || tetrimino.type == L || tetrimino.type == J || tetrimino.type == Z){
		RotateTetriminoRight(tetrimino, m, 2);
		     if(tetrimino.facing == rN) Rotate3x2NRight(tetrimino, buffer, m);
		else if(tetrimino.facing == rE) Rotate3x2ERight(tetrimino, buffer, m);
		else if(tetrimino.facing == rS) Rotate3x2SRight(tetrimino, buffer, m);
		else if(tetrimino.facing == rW) Rotate3x2WRight(tetrimino, buffer, m);
	}
	else if(tetrimino.type == S){
		RotateTetriminoRight(tetrimino, m, 3);
		     if(tetrimino.facing == rN) Rotate3x2NRight(tetrimino, buffer, m);
		else if(tetrimino.facing == rE) Rotate3x2ERight(tetrimino, buffer, m);
		else if(tetrimino.facing == rS) Rotate3x2SRight(tetrimino, buffer, m);
		else if(tetrimino.facing == rW) Rotate3x2WRight(tetrimino, buffer, m);
	}
}
void RotateLeft(Tetrimino &tetrimino, int buffer[41][11]){
	Mino m[4];
	if(tetrimino.type == I){
		if(tetrimino.facing == rN){
			m[0].x = tetrimino.mino[0].x+1; m[0].y = tetrimino.mino[0].y-2;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y-1;
			m[2].x = tetrimino.mino[2].x-1; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x-2; m[3].y = tetrimino.mino[3].y+1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1,  2})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2, -1})) tetrimino.facing = rW;
		}
		else if(tetrimino.facing == rW){
			m[0].x = tetrimino.mino[0].x+2; m[0].y = tetrimino.mino[0].y+1;
			m[1].x = tetrimino.mino[1].x+1; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x  ; m[2].y = tetrimino.mino[2].y-1;
			m[3].x = tetrimino.mino[3].x-1; m[3].y = tetrimino.mino[3].y-2;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-2, -1})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 1,  2})) tetrimino.facing = rS;
		}
		else if(tetrimino.facing == rS){
			m[0].x = tetrimino.mino[0].x-1; m[0].y = tetrimino.mino[0].y+2;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y+1;
			m[2].x = tetrimino.mino[2].x+1; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x+2; m[3].y = tetrimino.mino[3].y-1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1, -2})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2,  1})) tetrimino.facing = rE;
		}
		else if(tetrimino.facing == rE){
			m[0].x = tetrimino.mino[0].x-2; m[0].y = tetrimino.mino[0].y-1;
			m[1].x = tetrimino.mino[1].x-1; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x  ; m[2].y = tetrimino.mino[2].y+1;
			m[3].x = tetrimino.mino[3].x+1; m[3].y = tetrimino.mino[3].y+2;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 2,  1})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-1, -2})) tetrimino.facing = rN;
		}
	}
	else if(tetrimino.type == T || tetrimino.type == L || tetrimino.type == J || tetrimino.type == Z){
		RotateTetriminoLeft(tetrimino, m, 2);
		     if(tetrimino.facing == rN) Rotate3x2NLeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rE) Rotate3x2ELeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rS) Rotate3x2SLeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rW) Rotate3x2WLeft(tetrimino, buffer, m);
	}
	else if(tetrimino.type == S){
		RotateTetriminoLeft(tetrimino, m, 3);
		     if(tetrimino.facing == rN) Rotate3x2NLeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rE) Rotate3x2ELeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rS) Rotate3x2SLeft(tetrimino, buffer, m);
		else if(tetrimino.facing == rW) Rotate3x2WLeft(tetrimino, buffer, m);
	}
}
int LineClear(int buffer[41][11]){
	int check = 1;
	for(int i=1;i<=40;i++){
		for(int j=1;j<=10;j++){
			if(buffer[i][j] == 0) check = 0;
		}
		if(check){
			for(int k=0;k<=10;k++){
				buffer[i][k] = 0;
			}
			for(int k=i;k<=40;k++){
				for(int j=1;j<=10;j++){
					buffer[k][j] = buffer[k+1][j];
				}
			}
			i--;
		}
		check = 1;
	}
}
//Main function
int main(int argc, char ** argv){
	// MINO_TEXTURE[0] = (char)219; MINO_TEXTURE[1] = (char)219;
	// GHOST_TEXTURE[0] = (char)177; GHOST_TEXTURE[1] = (char)177;
	// TETRIMINO_TEXTURE[0] = (char)178; TETRIMINO_TEXTURE[1] = (char)178;
	// EMPTY_TEXTURE[0] = (char)32; EMPTY_TEXTURE[1] = (char)32;	
	EMPTY_TEXTURE[0] = '.'; EMPTY_TEXTURE[1] = ' ';
	MINO_TEXTURE[0] = '['; MINO_TEXTURE[1] = ']';
	GHOST_TEXTURE[0] = '('; GHOST_TEXTURE[1] = ')';
	TETRIMINO_TEXTURE[0] = '{'; TETRIMINO_TEXTURE[1] = '}';
	prevtetrimino = tetrimino;
	srand(time(NULL)); //for set seed
	for(int i=40;i>=1;i--){
		for(int j=1;j<=10;j++){
			prevbuffer[i][j] = 69; //haha funny number
		}
	}
	lose = 1;
	int prev = 0;
	char ch;
	system("cls");
	while(lose){
		lock = 1;
		GenerateBag(nextQueue);
		GenerateTetrimino(tetrimino, nextQueue, holdQueue);
		DisplayNextQueue(nextQueue);
		DisplayHoldQueue(holdQueue);
		FallTetrimino(tetrimino, buffer, delta, prev, lock);
		while(lock){
			prevtetrimino = tetrimino;
			FallTetrimino(tetrimino, buffer, delta, prev, lock);
			if(kbhit()){
				if(ch == 'e') hold_check = 0;
				else hold_check = 1;
				ch = getch();
				if(ch == ' ')
					Harddrop(tetrimino, buffer, lock);
				else if(ch == 'd')
					Softdrop(tetrimino, buffer, lock);
				else if(ch == 'f')
					MoveRight(tetrimino, buffer);
				else if(ch == 's')
					MoveLeft(tetrimino, buffer);
				else if(ch == 'k')
					RotateRight(tetrimino, buffer);
				else if(ch == 'j')
					RotateLeft(tetrimino, buffer);
				else if(ch == 'e' && hold_check){
					GenerateTetrimino(tetrimino, nextQueue, holdQueue, USING_HOLD);
					DisplayNextQueue(nextQueue);
					DisplayHoldQueue(holdQueue);
					hold_check = 0;
				}
			}
			DASRight(tetrimino, buffer, DASPrevRight, DASdelta);
			DASLeft(tetrimino, buffer, DASPrevLeft, DASdelta);
			ghosttetrimino = tetrimino;
			prevghosttetrimino = prevtetrimino;
			int ghostcheck = 1;
			int prevghostcheck = 1;
			while(ghostcheck || prevghostcheck){
				for(int i=0;i<4;i++){
					if(buffer[ghosttetrimino.mino[i].y-1][ghosttetrimino.mino[i].x] == 1 || ghosttetrimino.mino[i].y-1 == 0)
						ghostcheck = 0;
				}
				for(int i=0;i<4;i++){
					if(buffer[prevghosttetrimino.mino[i].y-1][prevghosttetrimino.mino[i].x] == 1 || prevghosttetrimino.mino[i].y-1 == 0)
						prevghostcheck = 0;
				}
				if(ghostcheck){
					for(int i=0;i<4;i++){
						ghosttetrimino.mino[i].y--;
					}
				}
				if(prevghostcheck){
					for(int i=0;i<4;i++){
						prevghosttetrimino.mino[i].y--;
					}
				}
			}
			RefreshScreen(buffer, prevbuffer, tetrimino, prevtetrimino, ghosttetrimino, prevghosttetrimino);
		}
		for(int i=0;i<4;i++){
			buffer[tetrimino.mino[i].y][tetrimino.mino[i].x] = 1;
		}
		LineClear(buffer);
		for(int i=0;i<4;i++){
			if(buffer[tetrimino.mino[i].y][tetrimino.mino[i].x] == 0){
				WriteConsoleOutputCharacter(h, EMPTY_TEXTURE, 2, {tetrimino.mino[i].x*CELL_WIDTH + PADDING_LEFT, DISPLAY_HEIGHT-tetrimino.mino[i].y + PADDING_TOP}, &extra);
			}
		}
	}
	return 0;
}