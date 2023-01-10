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
char MINO_TEXTURE[] = "[]";
int CELL_WIDTH = 2;
int O = 1,
    I = 2,
    T = 3,
    L = 4,
    J = 5,
    S = 6,
    Z = 7;
int DISPLAY_HEIGHT = 40;
int rN = 0,
    rE = 1,
    rS = 2,
    rW = 3;
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
}tetrimino, prevtetrimino;
int nextQueue[100];
int lose, lock;
int delta = 500;
int DASdelta = 133;
int DASPrevRight;
int DASPrevLeft;
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
void GenerateTetrimino(Tetrimino &tetrimino, int nextQueue[100]){
	tetrimino.type = nextQueue[0];
	for(int i=0;i<99;i++){
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
		else lock = 0;
	}
}
void RefreshScreen(int buffer[41][11], int prevbuffer[41][11], Tetrimino tetrimino, Tetrimino prevtetrimino){
	for(int i=1;i<=40;i++){
		for(int j=1;j<=10;j++){
			if(buffer[i][j] != prevbuffer[i][j]){
				if(buffer[i][j] == 0)
					WriteConsoleOutputCharacter(h, ". ", 2, {j*CELL_WIDTH, DISPLAY_HEIGHT-i}, &extra);
				else if(buffer[i][j] == 1)
					WriteConsoleOutputCharacter(h, "[]", 2, {j*CELL_WIDTH, DISPLAY_HEIGHT-i}, &extra);
			}
			prevbuffer[i][j] = buffer[i][j];
		}
	}
	bool check = 1;
	for(int k=0;k<4;k++){
		WriteConsoleOutputCharacter(h, "[]", 2, {tetrimino.mino[k].x*CELL_WIDTH, DISPLAY_HEIGHT-tetrimino.mino[k].y}, &extra);
		for(int l=0;l<4;l++)
			if(prevtetrimino.mino[k].y == tetrimino.mino[l].y && prevtetrimino.mino[k].x == tetrimino.mino[l].x)
				check = 0;
		if(check)
			WriteConsoleOutputCharacter(h, ". ", 2, {prevtetrimino.mino[k].x*CELL_WIDTH, DISPLAY_HEIGHT-prevtetrimino.mino[k].y}, &extra);
		else check = 1;
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
		if(buffer[m[i].y+offset.y][m[i].x+offset.x] == 1){
			return 0;
		}
	}
	cout << "here";
	for(int i=0;i<4;i++)
		tetrimino.mino[i] = m[i];
	return 1;
}
void RotateRight(Tetrimino &tetrimino, int buffer[41][11]){
	Mino m[4];
	if(tetrimino.type = I){
		if(tetrimino.facing == rN){
			m[0].x = tetrimino.mino[0].x+1; m[0].y = tetrimino.mino[0].y+1;
			m[1].x = tetrimino.mino[1].x+1; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x+1; m[2].y = tetrimino.mino[2].y-1;
			m[3].x = tetrimino.mino[3].x+1; m[3].y = tetrimino.mino[3].y-2;
            //cout << "here";
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, {-2, -1})) tetrimino.facing = rE;
			else if(Shift(tetrimino, buffer, m, { 1,  2})) tetrimino.facing = rE;
		}
		else if(tetrimino.facing == rE){
			m[0].x = tetrimino.mino[0].x-2; m[0].y = tetrimino.mino[0].y-2;
			m[1].x = tetrimino.mino[1].x-1; m[1].y = tetrimino.mino[1].y-1;
			m[2].x = tetrimino.mino[2].x  ; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x+1; m[3].y = tetrimino.mino[3].y+1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, {-1,  2})) tetrimino.facing = rS;
			else if(Shift(tetrimino, buffer, m, { 2, -1})) tetrimino.facing = rS;
		}
		else if(tetrimino.facing == rS){
			m[0].x = tetrimino.mino[0].x+1; m[0].y = tetrimino.mino[0].y+2;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y+1;
			m[2].x = tetrimino.mino[2].x-1; m[2].y = tetrimino.mino[2].y;
			m[3].x = tetrimino.mino[3].x-2; m[3].y = tetrimino.mino[3].y-1;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1,  0})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, { 2,  1})) tetrimino.facing = rW;
			else if(Shift(tetrimino, buffer, m, {-1, -2})) tetrimino.facing = rW;
		}
		else if(tetrimino.facing == rW){
			m[0].x = tetrimino.mino[0].x-1; m[0].y = tetrimino.mino[0].y-1;
			m[1].x = tetrimino.mino[1].x  ; m[1].y = tetrimino.mino[1].y;
			m[2].x = tetrimino.mino[2].x+1; m[2].y = tetrimino.mino[2].y+1;
			m[3].x = tetrimino.mino[3].x+2; m[3].y = tetrimino.mino[3].y+2;
			     if(Shift(tetrimino, buffer, m, { 0,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 1,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-2,  0})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, { 1, -2})) tetrimino.facing = rN;
			else if(Shift(tetrimino, buffer, m, {-2,  1})) tetrimino.facing = rN;
		}
	}
}
//Main function
int main(int argc, char ** argv){
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
		GenerateTetrimino(tetrimino, nextQueue);

		// ch = getch();
		// if(ch == 'f') break;
		while(lock){
			prevtetrimino = tetrimino;
			FallTetrimino(tetrimino, buffer, delta, prev, lock);
			if(kbhit()){
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
					// RotateTetrimino();
					//NOTE: Might use GetAsyncKeyState only for das, and getch for anything else
			}
			DASRight(tetrimino, buffer, DASPrevRight, DASdelta);
			DASLeft(tetrimino, buffer, DASPrevLeft, DASdelta);
			RefreshScreen(buffer, prevbuffer, tetrimino, prevtetrimino);
		}
		for(int i=0;i<4;i++){
			buffer[tetrimino.mino[i].y][tetrimino.mino[i].x] = 1;
		}
	}
	return 0;
}
