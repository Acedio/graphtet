//  []
//  [][]
//  []
//
// TETRIS
// FILENAME: "tet.h"
// JOSH SIMMONS
// LAST MODIFICATION : 12/14/2006 11:47:46 AM

// All the sweet SDL libraries. String is for filenames, stdlib for random, 
// iostream for error output, sstream as a workaround to convert ints to
// strings (why can't there just be a function in <string> for that, seriously...
// fstream for highscores
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
using namespace std; // Standard namespace woo.

// Use XMs or MP3s? XMs are MUCH smaller. The program is less than 300 kb, but then you add on MP3s and make
// it about 6 megs bigger. XMs are eensy weensy, though, and sound OK.
const bool XM = false;

const int SPEED = 700; //Starting speed (ms)
const int MINSPEED = 75; // Speed at level infinity (ms)

const int TILESIZE = 4; // Tetris uses 4*4 tiles to store blocks
const int BLOCKSIZE = 24; // in pixels
const int FIELDX = 10; // Our "field" is 10 blocks wide and 20 tall
const int FIELDY = 20;
const int BLOCKTYPES = 7; // 7 types of blocks: I, O, J, L, T, S, Z
const int CC = 0; //counter clockwise
const int CW = 1; //clockwise

const int SMBLOCKSIZE = 16; // small blocks (for next and hold) pixel size

// Standardized colors
const int BLOCKYELLOW = 1; //O
const int BLOCKCYAN = 2; //I
const int BLOCKBLUE = 3; //J
const int BLOCKORANGE = 4; //L
const int BLOCKMAGENTA = 5; //T
const int BLOCKRED = 6; //Z
const int BLOCKGREEN = 7; //S

const int SCREENW = 800; //800 pixels wide, 600 tall. Pretty standard.
const int SCREENH = 600;
const int SCREENBPP = 32; // 32 bits per pixel.

const int XOFFSET = 120; //where the field is in the screen x and y
const int YOFFSET = 60;

const int FONTSIZE = 42; //for score and stuff

int field[FIELDX][FIELDY]; //Our field.

//Tile class, with lame overloaded function
class Tile{
private:
	int xcoor, ycoor; //location
	int blocks[TILESIZE][TILESIZE]; //the 4*4 block array
	int STYLE; //color
public:
	Tile(); //contructah
	bool check(int cx, int cy); 
	bool rotate(bool DIR);
	void show();
	void apply();
	void disp(int x, int y);
	void reset();
	friend SDL_Surface* operator<<(SDL_Surface* surface, Tile tile); //BTW, this is a dumb way to use overloading.
};

// This is the score class, it's used for keeping all the highscores in.
class Scores{
private:
	string names[10];
	int scores[10];
	string fname; // name of the file we opened
public:
	Scores(string filename);
	void display();
	void checkScore(int score); //checkc current score against high scores
	void save();
	string getName(); // nifty initial getter
};

void clearfield(); //initializes/clears field
int checklines(int &lines); //any lines to be cleared?
void setclips(); //where are the blocks in the picture?
string intToStr(int i); //THEY NEED TO PUT THIS IN <string>.
void gameOver();

int rotocheck(int blocks[TILESIZE][TILESIZE], int xcoor, int ycoor); //Checks for rotation collisions

SDL_Surface *screen = NULL; //our screen for blitting to
SDL_Surface *block = NULL; //our picture!
SDL_Surface *smallblock = NULL; //small blocks
SDL_Surface *back = NULL; //background!
SDL_Surface *numbers = NULL; //Score!
SDL_Surface *title = NULL; //Title
SDL_Surface *cursor = NULL; //Title cursor

TTF_Font *font = NULL; //Our font

SDL_Color fontColor = { 255, 255, 255 }; //font color

SDL_Rect clip[BLOCKTYPES + 1]; //Used for holding the clip data. +1 because zero is not a block type, it's  a space
SDL_Rect smclip[BLOCKTYPES + 1];

Mix_Music *music = NULL; //our tunage

Mix_Chunk *effect = NULL; //our sound. dee doo.

SDL_Event event; //event structure so we can catch the X button activity and keyboard stuff

SDL_Surface *load_image( string filename); //returns a pointer to filename image and does all the optimization and color keying
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL); //prints surface to screen
bool init(); //opens up SDL, SDL_mixer, and SDL_ttf
void clean(); //closes everything and frees the surfaces
