//  []
//  [][]
//  []
//
// TETRIS
// JOSH SIMMONS
// LAST MODIFICATION : 11.30.06 @ 2:02PM


#include <iostream>
#include <stdlib.h>
using namespace std;

const int TILESIZE = 4;
const int FIELDX = 10;
const int FIELDY = 15;
const int BLOCKTYPES = 7;
const int CCW = 0;
const int CW = 1;

const int BLOCKRED = 1;
const int BLOCKBLUE = 2;
const int BLOCKGREEN = 3;
const int BLOCKYELLOW = 4;
const int BLOCKBLACK = 5;
const int BLOCKORANGE = 6;
const int BLOCKPURPLE = 7;

int field[FIELDX][FIELDY];

class Tile{
private:
	int xcoor, ycoor;
	int blocks[TILESIZE][TILESIZE];
	int STYLE;
public:
	Tile();
	bool check(int cx, int cy);
	bool rotate(bool DIR);
	void show();
	void apply();
};

void clearfield();
void checklines();

Tile::Tile()
{
	xcoor = (FIELDX - TILESIZE)/2;
	ycoor = 0;
	int block = rand()%BLOCKTYPES;
	switch(block)
	{
		case 0:
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKRED;
			break;
			
		case 1:
			blocks[2][0] = 1;
			blocks[2][1] = 1;
			blocks[2][2] = 1;
			blocks[2][3] = 1;
			STYLE = BLOCKBLUE;
			break;
		
		case 2:
			blocks[2][0] = 1;
			blocks[2][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKGREEN;
			break;
		
		case 3:
			blocks[1][0] = 1;
			blocks[1][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKYELLOW;
			break;
		
		case 4:
			blocks[0][2] = 1;
			blocks[1][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKBLACK;
			break;
		
		case 5:
			blocks[0][1] = 1;
			blocks[1][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKORANGE;		
			break;
		
		case 6:
			blocks[3][1] = 1;
			blocks[2][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKPURPLE;
			break;
	}
}

bool Tile::check(int cx, int cy)
{
	bool yep = 1; //if the block wont fit, yes
	
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] == 1)
			{
				if(field[xcoor + x + cx][ycoor + y + cy] != 0)
				{
					yep = 0;
				}
				if((xcoor + x) > FIELDX || (xcoor + x) < 0 || (ycoor + y) > FIELDY || (ycoor + y) < 0 )
				{
					yep = 0;
				}
			}
		}
	}
	
	if(yep)
	{
		xcoor += cx;
		ycoor += cy;
	}
	
	return yep;
}

bool Tile::rotate(bool DIR) //0 = ccw, 1 = cw
{
	int x, y;
	Tile checker;
	checker.xcoor = xcoor;
	checker.ycoor = ycoor;
	
	if(DIR = CCW)
	{
		for(y = 0; y < TILESIZE; y++)
		{
			for(x = 0; x < TILESIZE; x++)
			{
				checker.blocks[y][x] = blocks[x][y];
			}
		}
	}
	
	if(DIR = CW)
	{
		for(y = 0; y < TILESIZE; y++)
		{
			for(x = 0; x < TILESIZE; x++)
			{
				checker.blocks[TILESIZE - y][x] = blocks[x][y];
			}
		}
	}
	
	if(checker.check(0,0))
	{
		for(y = 0; y < TILESIZE; y++)
		{
			for(x = 0; x < TILESIZE; x++)
			{
				blocks[x][y] = checker.blocks[x][y];
			}
		}
		return 1;
	}
	return 0;
}

void Tile::show()
{
	for(int y = 0; y < FIELDY; y++)
	{
		for(int x = 0; x < FIELDX; x++)
			cout << field[x][y];
		cout << endl;
	}
}

void Tile::apply()
{
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] = 1)
			{
				field[xcoor + x][ycoor + y] = STYLE;
			}
		}
	}
}

void clearfield()
{
	for(int y = 0; y < FIELDY; y++)
	{
		for(int x = 0; x < FIELDX; x++)
		{
			field[x][y] = 0;
		}
	}
}

void checklines()
{
	int x, y, clear; //clear is used to keep track of the cleared rows
	bool nope; //if the field value is zero, nope = true
	
	for(y = 0; y < FIELDY; y++)
	{
		for(x = 0; x < FIELDX; x++)
		{
			if(!field[x][y])
				nope = true;
		}
		if(!nope)
		{
			for(clear = y; clear > 0; clear++)
			{
				for(x = 0; x < FIELDX; x++)
				{
					field[x][y] = field[x][y - 1];
				}
			}
		}
		nope = false;
	}
}
					

int main()
{
	bool quit = 0, falling;
	
	Tile *tile; //current tile
	
	clearfield();
	
	while(!quit)
	{
		tile = new Tile();
		falling = true;
		
		while(falling)
		{
			/* while()
			{
			}*/
			system("clear");
			tile->show();
			if(tile->check(0, -1))
				falling = false;
		}
		
	}
	return 0;
}
