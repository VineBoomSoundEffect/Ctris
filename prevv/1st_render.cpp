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
}tetrimino;
int nextQueue[100];
int lose, lock;
int delta = 500;

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
		for(int i=0;i<4;i++){
			if(tetrimino.mino[i].y-1 == 0 || buffer[tetrimino.mino[i].y-1][tetrimino.mino[i].x] != 0)
				check = 0;
		}
		if(check){
			for(int i=0;i<4;i++){
				//cout << tetrimino.mino[i].x << " " << tetrimino.mino[i].y << "\n";
				tetrimino.mino[i].y--;
			}
			//cout << "\n";
		}
		else{
			lock = 0;
		}
		//cout << "check: " << check << "\n";
	}
}
void RefreshScreen(int buffer[41][11], int prevbuffer[41][11], Tetrimino tetrimino){
	int mirror = 40;
	Tetrimino prevtetrimino = tetrimino;
	bool check = 1;
	for(int i=0;i<4;i++){
		prevtetrimino.mino[i].y++;
	}
	for(int i=1;i<=40;i++){
		mirror--;
		for(int j=1;j<=10;j++){
			if(buffer[i][j] != prevbuffer[i][j]){
				if(buffer[i][j] == 0)
					WriteConsoleOutputCharacter(h, ". ", 2, {j*CELL_WIDTH, mirror}, &extra);
				else if(buffer[i][j] == 1)
					WriteConsoleOutputCharacter(h, "[]", 2, {j*CELL_WIDTH, mirror}, &extra);
			}
			for(int k=0;k<4;k++){
				if(tetrimino.mino[k].x==j && tetrimino.mino[k].y==i){
					WriteConsoleOutputCharacter(h, "[]", 2, {j*CELL_WIDTH, mirror}, &extra);
					for(int l=0;l<4;l++)
						if(tetrimino.mino[k].y+1 == tetrimino.mino[l].y && tetrimino.mino[k].x == tetrimino.mino[l].x)
							check = 0;
				if(check)
					WriteConsoleOutputCharacter(h, ". ", 2, {j*CELL_WIDTH, mirror-1}, &extra);
				else check = 1;
				}
			}
			prevbuffer[i][j] = buffer[i][j];
		}
	}
}

//Main function
int main(int argc, char ** argv){
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

		for(int i=0;i<14;i++){
			cout << nextQueue[i] << " ";
			if(i == 6) cout << "\t";
		}
		cout << "\n";
		ch = getch();
		if(ch == 'f') break;
		cout << tetrimino.type;
		while(lock){
			FallTetrimino(tetrimino, buffer, delta, prev, lock);
		// if(kbhit()){
			// 	getch();
			// 	RotateTetrimino();
			// 	MoveTetrimino();
			//  SoftdropTetrimino();
			//  HarddropTetrimino();
			// 	//NOTE: Might use GetAsyncKeyState only for das, and getch for anything else
		// }
			RefreshScreen(buffer, prevbuffer, tetrimino);
		}
		for(int i=0;i<4;i++){
			buffer[tetrimino.mino[i].y][tetrimino.mino[i].x] = 1;
		}
	}
	return 0;
}