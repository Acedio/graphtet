//  []
//  [][]
//  []
//
// TETRIS
// FILENAME: "tet.cpp"
// JOSH SIMMONS
// LAST MODIFICATION : 12/14/2006 11:47:46 AM
#include "tet.h"

//returns our loaded image, colorkeyed, optomized, and all
SDL_Surface *load_image( string filename)
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	
	loadedImage = IMG_Load( filename.c_str() ); //I wish I would have known about the STR.c_str() function earlier =/
	
	if( loadedImage != NULL) //this will optimize the image for the format we want. Basically just changes color depth
	{
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
		
		if(optimizedImage != NULL)
		{
			Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ); //color keeeey. makes all the 0x00FFFF values transparent. nifty.
			
			SDL_SetColorKey( optimizedImage, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey );
		}
	}
	
	return optimizedImage;
}

//takes a given surface to blit to the screen
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
	SDL_Rect offset;
	
	offset.x = x; //sets where the image will appear on screen
	offset.y = y;
	
	SDL_BlitSurface( source, clip, destination, &offset ); //yay pointers =D
}

bool init() // initializes all SDL stuff
{
	//initializes all the subsytems (video, sound and all that)
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false; //if it fails
	}
	
	//sets our main surface to the desired width, height, bpp, and tells it to write in system memory rather than video memory
	screen = SDL_SetVideoMode( SCREENW, SCREENH, SCREENBPP, SDL_SWSURFACE );
	
	if( screen == NULL )
	{
		return false; //more error checking
	}
	
	//Get the font engine up and running (scores)
	if(TTF_Init() == -1)
	{
		return false;
	}
	
	//init the rave mixa
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		cout << "Mix_OpenAudio: " << Mix_GetError() << endl;
	}
	
	//sets window name to \/ that and says we don't want no shtinkin' icon
	SDL_WM_SetCaption( "TETЯIS", NULL );
	
	//if all is well in the world
	return true;
}

void setclips() //this will show where the different colored blocks are in blocks.png
{
	clip[0].x = 0;
	clip[0].y = 0;
	clip[0].w = BLOCKSIZE;
	clip[0].h = BLOCKSIZE;
	
	for(int y = 1; y <= BLOCKTYPES; y++)
	{
		clip[y].x = (y - 1) * BLOCKSIZE;
		clip[y].y = 0;
		clip[y].w = BLOCKSIZE;
		clip[y].h = BLOCKSIZE;
	}
	
	smclip[0].x = 0; //and where smallblocks are in smallblocks.png
	smclip[0].y = 0;
	smclip[0].w = SMBLOCKSIZE;
	smclip[0].h = SMBLOCKSIZE;
	
	for(int y = 1; y <= BLOCKTYPES; y++)
	{
		smclip[y].x = (y - 1) * SMBLOCKSIZE;
		smclip[y].y = 0;
		smclip[y].w = SMBLOCKSIZE;
		smclip[y].h = SMBLOCKSIZE;
	}
}

void clean() //closes up everything so our OS doesn't whine.
{
	SDL_FreeSurface(block); //frees pictures
	SDL_FreeSurface(numbers);
	SDL_FreeSurface(smallblock);
	SDL_FreeSurface(back);
	SDL_FreeSurface(title);
	
	Mix_FreeMusic(music);
	
	Mix_FreeChunk(effect);
	
	TTF_CloseFont(font);
	
	TTF_Quit();
	
	Mix_CloseAudio(); //Closes mixer
	
	SDL_Quit(); //frees screen and everything else
}

//constructor for tiles. pseudo random.
//by the way...
/*
O
XX
XX

I
XXXX

J
 X
 X
XX

L
X
X
XX

T
XXX
 X

S
 XX
XX

Z
XX
 XX
*/
//Good?
Tile::Tile()
{
	xcoor = (FIELDX - TILESIZE)/2;
	ycoor = 0;

	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			blocks[x][y] = 0;
		}
	}

	int block = rand()%BLOCKTYPES;
	switch(block)
	{
		case 0: //O
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			blocks[1][2] = 1;
			blocks[2][2] = 1;
			STYLE = BLOCKYELLOW;
			break;
			
		case 1: //I
			blocks[2][0] = 1;
			blocks[2][1] = 1;
			blocks[2][2] = 1;
			blocks[2][3] = 1;
			STYLE = BLOCKCYAN;
			break;
		
		case 2: //J
			blocks[0][0] = 1;
			blocks[0][1] = 1;
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			STYLE = BLOCKBLUE;
			break;
		
		case 3: //L
			blocks[2][0] = 1;
			blocks[0][1] = 1;
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			STYLE = BLOCKORANGE;
			break;
		
		case 4: //T
			blocks[1][0] = 1;
			blocks[0][1] = 1;
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			STYLE = BLOCKMAGENTA;
			break;
		
		case 5: //Z
			blocks[0][0] = 1;
			blocks[1][0] = 1;
			blocks[1][1] = 1;
			blocks[2][1] = 1;
			STYLE = BLOCKRED;		
			break;
		
		case 6: //S
			blocks[1][0] = 1;
			blocks[2][0] = 1;
			blocks[0][1] = 1;
			blocks[1][1] = 1;
			STYLE = BLOCKGREEN;
			break;
	}
}

//checks if there are blocks occupying a certain point in relation to the tile and moves it if not.
bool Tile::check(int cx, int cy)
{	
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] != 0)
			{
				if(field[xcoor + x + cx][ycoor + y + cy] != 0)
				{
					return 0;
				}
				if((ycoor + y + cy) >= FIELDY )
				{
					return 0; //Ran into ground. Stop falling.
				}
				if((xcoor + x + cx) >= FIELDX || (xcoor + x + cx) < 0 )
				{
					return 1; //It ran into a wall.
				}
			}
		}
	}
	
	xcoor += cx;
	ycoor += cy;
	
	return 1;
}

//rotates the tile, then checks it
bool Tile::rotate(bool DIR) //CC = counterclockwise, CW = clockwise
{
	int x, y;
	int checkx, checky, checkblocks[TILESIZE][TILESIZE];
	bool roto = 0;
	checkx = xcoor;
	checky = ycoor;
	
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			checkblocks[x][y] = 0;
		}
	}
	
	if(STYLE > 2)
	{
		roto = 1;
	}
	
	if(DIR == CC)
	{
		for(y = 0; y < TILESIZE - roto; y++)
		{
			for(x = 0; x < TILESIZE - roto; x++)
			{
				checkblocks[x][y] = blocks[TILESIZE-roto-y-1][x]; //What's with the -1? The thing is, the for loop goes from 0..3 so the TILESIZE - x will go from 4..1. Since we want 3..0, we -1 more.
			}
		}
	}
	
	if(DIR == CW)
	{
		for(y = 0; y < TILESIZE - roto; y++)
		{
			for(x = 0; x < TILESIZE - roto; x++)
			{
				checkblocks[x][y] = blocks[y][TILESIZE-roto-x-1]; //See above.
			}
		}
	}
	
	if(rotocheck(checkblocks, xcoor, ycoor))
	{
		for(y = 0; y < TILESIZE; y++)
		{
			for(x = 0; x < TILESIZE; x++)
			{
				blocks[x][y] = checkblocks[x][y];
			}
		}
		return 1;
	}
	return 0;
}

//displays the current tile and the other tiles on screen
void Tile::show()
{
	int x, y;
	for(y = 0; y < FIELDY; y++)
	{
		for(x = 0; x < FIELDX; x++)
		{
			if(field[x][y] != 0)
			{
				apply_surface( x*BLOCKSIZE + XOFFSET, y*BLOCKSIZE + YOFFSET, block, screen, &clip[field[x][y]] ); //calls our apply surface function for the field
			}
		}
	}
	for(y = 0; y < TILESIZE; y++)
	{
		for(x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] != 0)
			{
				apply_surface( xcoor*BLOCKSIZE+x*BLOCKSIZE + XOFFSET, ycoor*BLOCKSIZE+y*BLOCKSIZE + YOFFSET, block, screen, &clip[STYLE] ); //and for the current tile
			}
		}
	}
}

//stupid overloading requirement deal. same as above.
SDL_Surface* operator<<(SDL_Surface* surface, Tile tile)
{
	int x, y;
	for(y = 0; y < FIELDY; y++)
	{
		for(x = 0; x < FIELDX; x++)
		{
			if(field[x][y] != 0)
			{
				apply_surface( x*BLOCKSIZE + XOFFSET, y*BLOCKSIZE + YOFFSET, block, surface, &clip[field[x][y]] );
			}
		}
	}
	for(y = 0; y < TILESIZE; y++)
	{
		for(x = 0; x < TILESIZE; x++)
		{
			if(tile.blocks[x][y] != 0)
			{
				apply_surface( tile.xcoor*BLOCKSIZE+x*BLOCKSIZE + XOFFSET, tile.ycoor*BLOCKSIZE+y*BLOCKSIZE + YOFFSET, block, screen, &clip[tile.STYLE] );
			}
		}
	}
	
	return surface;
}

//When the tile can fall no more, apply it.
void Tile::apply()
{
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] != 0)
			{
				field[xcoor + x][ycoor + y] = STYLE;
			}
		}
	}
}

//puts the tile back at the top of the screen. used when you hold things.
void Tile::reset()
{
	xcoor = (FIELDX - TILESIZE)/2;
	ycoor = 0;
}

//for displaying small tiles on the screen for next and hold
void Tile::disp(int x, int y)
{
	for(int i = 0; i < TILESIZE; i++)
	{
		for(int j = 0; j < TILESIZE; j++)
		{
			if(blocks[j][i] != 0)
			{
				apply_surface( x + SMBLOCKSIZE*j, y + SMBLOCKSIZE*i, smallblock, screen, &smclip[STYLE] );
			}
		}
	}
}

//you pass the lines that have been cleared by reference (used for levels), then return the
//score you get for the number of lines you cleared (standardized)
int checklines(int &lines)
{
	int x, y, clear, cleared = 0; //clear is used to keep track of the cleared rows
	bool nope; //if the field value is zero, nope = true
	
	for(y = 0; y < FIELDY; y++)
	{
		for(x = 0; x < FIELDX; x++)
		{
			if(field[x][y] == 0)
				nope = true;
		}
		if(!nope)
		{
			for(clear = y; clear > 0; clear--)
			{
				for(x = 0; x < FIELDX; x++)
				{
					field[x][clear] = field[x][clear - 1];
				}
			}
			cleared++;
		}
		nope = false;
	}
	lines += cleared;
	switch(cleared) // Standard scores from way back when!
	{
		case 1: // 1 line
			return 40;
		case 2: // 2 lines
			return 100;
		case 3: // 3 lines
			return 300;
		case 4: // 4 lines
			return 1200;
		default: // 0 lines! go you!
			return 0;
	}
}

//we use it for checking rotation, but you could really use it for checking any block. returns true if it'd work.
int rotocheck(int blocks[TILESIZE][TILESIZE], int xcoor, int ycoor)
{	
	for(int y = 0; y < TILESIZE; y++)
	{
		for(int x = 0; x < TILESIZE; x++)
		{
			if(blocks[x][y] != 0)
			{
				if(field[xcoor + x][ycoor + y] != 0)
				{
					return 0;
				}
				if((xcoor + x) >= FIELDX || (xcoor + x) < 0 || (ycoor + y) >= FIELDY  )
				{
					return 0;
				}
			}
		}
	}
	
	return 1;
}



void gameOver() // Tiles stacking up, music fading out, etc
{
	Mix_FadeOutMusic(1000);
	
	for(int y = FIELDY - 1; y >= 0; y--)
	{
		for(int x = 0; x < FIELDX; x++)
		{
			apply_surface( x*BLOCKSIZE + XOFFSET, y*BLOCKSIZE + YOFFSET, block, screen, &clip[(((FIELDY-y)*FIELDX+x)%7+1)] );
			SDL_Flip( screen );
			SDL_Delay(2);
		}
	}
	
	SDL_Delay(1000);
}

Scores::Scores(string filename) // Constructor. Opens the score file and reads in the scores/names. If a file doesn't exist, it just creates a 'blank' one.
{
	fname = filename;
	ifstream file;
	file.open(fname.c_str());
	if(file.fail())
	{
		for(int i = 0; i < 10; i++)
		{
		names[i] = "NUL";
		scores[i] = 0;
		}
	}
	else
	{
		for(int i = 0; i < 10; i++)
		{
			file >> names[i];
			file >> scores[i];
		}
		file.close();
	}
}

void Scores::display() // Displays all the high scores with cool little scrolling tiles ;D
{
	bool quit = false;
	
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
	
	numbers = TTF_RenderText_Blended(font, "High Scores!", fontColor);
	apply_surface(280, 35, numbers, screen);
	
	for(int i = 0; i < 10; i++)
	{
		numbers = TTF_RenderText_Blended(font, names[i].c_str(), fontColor);
		apply_surface(280, i*50 + 85, numbers, screen);
		numbers = TTF_RenderText_Blended(font, intToStr(scores[i]).c_str(), fontColor);
		apply_surface(360, i*50 + 85, numbers, screen);
	}
	
	while(!quit)
	{
		for(int i = 0; i < SCREENW+SMBLOCKSIZE*BLOCKTYPES; i+=SMBLOCKSIZE*BLOCKTYPES)
		{
			apply_surface(i+(SDL_GetTicks()/100)%(SMBLOCKSIZE*BLOCKTYPES)-(SMBLOCKSIZE*BLOCKTYPES),0,smallblock,screen);
			apply_surface(i-(SDL_GetTicks()/100)%(SMBLOCKSIZE*BLOCKTYPES),SCREENH-SMBLOCKSIZE,smallblock,screen);
		}
		SDL_Flip(screen);
		SDL_Delay(1);
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
			{
				exit(0);
			}
			else if( event.type == SDL_KEYDOWN )
			{
				quit = true;
			}
		}
	}
}

void Scores::checkScore(int score) // Checks our current score against the high scores and moves accordingly
{
	for(int i = 0; i < 10; i++)
	{
		if(scores[i] < score)
		{
			for(int j = 8; j >= i; j--)
			{
				scores[j+1] = scores[j];
			}
			scores[i] = score;
			names[i] = getName(); // Gets the initials
			i = 10;
		}
	}
}

void Scores::save() // commits scores to memory
{				
	ofstream file;
	file.open(fname.c_str()); // uses the same filename we used to open it
	for(int i = 0; i < 10; i++)
	{
		file << names[i] << " ";
		file << scores[i] << endl;
	}
	file.close();
}

string Scores::getName() // Nifty input getter.
{
	string str = "NAM";
	bool done = false;
	SDL_EnableUNICODE( SDL_ENABLE ); // SDL doesn't have it's keyboard stuff in order to begin with (ABC..Z), so we have to enable unicode to straighten em out.
	while(!done)
	{
		while( SDL_PollEvent( &event ) )
    	{
    		if( event.type == SDL_QUIT )
    		{
    			exit(0);
    		}
    		else if( event.type == SDL_KEYDOWN )
    		{
    			if( str.length() < 3 )
    			{
    				if( ( event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9' ) )
    				{
    					str += (char)event.key.keysym.unicode;
    				}
    				else if( ( event.key.keysym.unicode >= (Uint16)'A' ) && ( event.key.keysym.unicode <= (Uint16)'Z' ) )
    				{
    					str += (char)event.key.keysym.unicode;
    				}
    				else if( ( event.key.keysym.unicode >= (Uint16)'a' ) && ( event.key.keysym.unicode <= (Uint16)'z' ) )
   					{
  						str += (char)event.key.keysym.unicode + (Uint16)'A' - (Uint16)'a';
   					}
				}
				if( ( event.key.keysym.sym == SDLK_BACKSPACE ) && ( str.length() != 0 ) )
				{
					//Remove a character from the end
					str.erase( str.length() - 1 );
				}
				if( (event.key.keysym.sym == SDLK_RETURN ) )
				{
					done = true;
				}
			}
		}
		SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
		numbers = TTF_RenderText_Blended(font, "New High Score!", fontColor);
		apply_surface( ( SCREENW - numbers->w ) / 2, (( SCREENH - numbers->h ) / 2 - 50), numbers, screen );
		numbers = TTF_RenderText_Blended(font, str.c_str(), fontColor);
		if( numbers != NULL )
    	{
        	//Show the name
        	apply_surface( ( SCREENW - numbers->w ) / 2, ( SCREENH - numbers->h ) / 2, numbers, screen );
    	}
    	SDL_Flip( screen );
	}
	SDL_EnableUNICODE( SDL_DISABLE ); // It uses a lot of overhead supposedly so we have to close it out, too
	return str;
}	

//the massive main function
int main(int argc, char* args[]) // Gets command line input
{
	bool quit = false, playing = true, falling = true, held = false, paused = false, cheating = false; // All our miscellaneous bools to keep track of game states.
	
	int level = 0, lines = 0, score = 0, choice = 0, cheatseq = 0;
	
	Scores scores("gfx"); // Opens the uber camoflaged gfx file for getting highscores. Tricksy, eh?
	
	Uint32 start = 0; // A REALLY big int for keeping track of time
	
	srand(SDL_GetTicks()); // seeding the random... seed
	
	if( init() == false ) // Initialize SDL
	{
		cout << "Init fail" << endl;
		return 1; //ERR !!!!
	}
	
	block = load_image( "blocks.png" ); // load blocks
	
	if(block == NULL) 
	{
		cout << "Error loading blocks.png" << endl;
		return 1; //ERR!
	}
	
	back = load_image( "back.png" ); // load background
	
	if(back == NULL)
	{
		cout << "Error loading back.png" << endl;
		return 1; //ERR!
	}
	
	smallblock = load_image( "smallblocks.png" ); // small blocks for next and hold
	
	if(smallblock == NULL)
	{
		cout << "Error loading smallblocks.png" << endl;
		return 1; //ERR!
	}
	
	title = load_image( "title.png" ); // title
	
	if(title == NULL)
	{
		cout << "Error loading title.png" << endl;
		return 1; //ERR!
	}
	
	cursor = load_image( "cursor.png" ); // cursor in menu
	
	if(cursor == NULL)
	{
		cout << "Error loading cursor.png" << endl;
		return 1; //ERR!
	}
	
	font = TTF_OpenFont("ProggyClean.ttf", FONTSIZE); // our font
	
	if(font == NULL)
	{
		cout << "Error loading ProggyClean.ttf" << endl;
		return 1; //Yup. Didn't load.
	}
	
	effect = Mix_LoadWAV( "pause.wav" ); // dee doo sound
	
	if(effect == NULL)
	{
		cout << "Mix_LoadWAV: " << Mix_GetError() << endl;
	}
	
	while(playing) // while the user hasn't quit
	{
		score = 0;
		quit = false;
		Mix_FadeOutMusic(100); // fades out the music (if playing) for menu.
		
		Mix_FreeMusic(music); // gets rid of any music we might have loaded
		
		if(XM == true) // load title music
		{
			music = Mix_LoadMUS("title.xm");
		}
		else
		{
			music = Mix_LoadMUS("title.mp3");
		}
		
		if(!music)
		{
	   		cout << "Mix_LoadMUS(\"title.mp3\"): %s\n" << Mix_GetError() << endl;
	   		// music didn't load...
		}
		
		Mix_PlayMusic(music, -1); // play it til the user can't stand it no mo'.

		for(int i = 600; i>=100; i--) // slowly bring up the title
		{
			while( SDL_PollEvent( &event ) ) // gets any recent events (mouse, keyboard, whatev)
			{
				if( event.type == SDL_QUIT ) // X button
				{
					i = 100;
					playing = false;
					quit = true;
				}
				else if( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) //escape
				{
					i = 100; // brings the title screen instantly
				}
			}
			SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) ); // fill screen black
			apply_surface( 200, i, title, screen ); // apply title
			if( SDL_Flip( screen ) == -1 ) // hand *screen to video card
			{
				return 1; //ERRRRRRR !!
			}
			SDL_Delay(2); // slows it down a wee bit.
		}
		
		apply_surface( 325, 402 + choice * 50, cursor, screen ); // display cursor
		
		numbers = TTF_RenderText_Blended(font, "START", fontColor); // start
		apply_surface( 350, 400, numbers, screen );
		
		numbers = TTF_RenderText_Blended(font, "HIGH SCORES", fontColor); // high scores
		apply_surface( 350, 450, numbers, screen );
		
		numbers = TTF_RenderText_Blended(font, "EXIT", fontColor); // exit
		apply_surface( 350, 500, numbers, screen );
		
		if( SDL_Flip( screen ) == -1 )
		{
			return 1; //ERRRRRRR !!
		}
		
		paused = true; //pause for menu
		while(paused && !quit)
		{
			while( SDL_PollEvent( &event ) ) // wait for events
			{
				if( event.type == SDL_QUIT )
				{
					paused = false;
					playing = false;
					quit = true;
				}
				else if( event.type == SDL_KEYDOWN )
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							paused = false;
							quit = true;
							playing = false;
							break;
						case SDLK_RETURN:
							switch(choice)
							{
								case 0:
									paused = false;
									break;
								case 1:
									scores.display();
									break;
								case 2:
									paused = false;
									quit = true;
									playing = false;
									break;
							}
							break;
						case SDLK_DOWN: // down, move cursor down
							Mix_PlayChannel(-1, effect, 0); // dee doo sound
							if(choice < 2)
							{
								choice++;
							}
							else
							{
								choice = 0;
							}
							break;
						case SDLK_UP: // up, move cursor up
							Mix_PlayChannel(-1, effect, 0); // see above
							if(choice > 0)
							{
								choice--;
							}
							else
							{
								choice = 2;
							}
							break;
					}
					SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
					
					apply_surface( 200, 100, title, screen );
					apply_surface( 325, 402 + choice * 50, cursor, screen );
							
					numbers = TTF_RenderText_Blended(font, "START", fontColor);
					apply_surface( 350, 400, numbers, screen );
					
					numbers = TTF_RenderText_Blended(font, "HIGH SCORES", fontColor);
					apply_surface( 350, 450, numbers, screen );
		
					numbers = TTF_RenderText_Blended(font, "EXIT", fontColor);
					apply_surface( 350, 500, numbers, screen );
		
					if( SDL_Flip( screen ) == -1 )
					{
						return 1; //ERRRRRRR !!
					}
				}
			}
		}
		
		if(!quit) // if the user didn't quit
		{		
			Mix_FadeOutMusic(100); //fade out title music
			
			Mix_FreeMusic(music); //free it
			
			if(XM == true) // change to game music
			{
				music = Mix_LoadMUS("music.xm");
			}
			else
			{
				music = Mix_LoadMUS("music.mp3");
			}
		
			if(!music)
			{
	   			cout << "Mix_LoadMUS(\"music.mp3\"): %s\n" << Mix_GetError() << endl;
	   			// music didn't load...
			}
			
			Mix_PlayMusic(music, -1);
		
			SDL_Delay(1500); // wait for ba-ding! to finish
		}
		
		Tile *tile = NULL, *next = NULL, *next2 = NULL, *next3 = NULL, *hold = NULL, *blank = NULL; //current tile, next tiles 1 2 3, hold, and the tile used for switching hold
		
		setclips(); // set the clip boxes for tiles
		
		clearfield(); // initialize the field

		next = new Tile(); // inits new tiles
		next2 = new Tile();
		next3 = new Tile();
		
		while(!quit)
		{
			SDL_Delay(50); // wait a bit for the next block to drop
			tile = next; // next to current
			next = next2; // move nexts
			next2 = next3;
			next3 = new Tile(); // new next 3
			if(!tile->check(0, 0)) // if the tile is colliding from the get go
			{
				tile->show();
				if( SDL_Flip( screen ) == -1 )
				{
					return 1; //ERRRRRRR !!
				}
				gameOver(); // THEN YOU LOSE! HA!
				scores.checkScore(score); // Did you get a high score?
				scores.save(); // save the high score file
				scores.display(); // display the current high scores
				quit = true;
			}
			falling = true;
			
			while(falling && !quit) // the there a lot of !quits from now on so it goes straight back to the menu
			{
				apply_surface( 0, 0, back, screen ); // background
				tile->show(); // show the current tile
				if(hold != NULL)
				{
					hold->disp(27,78); // display hold
				}
				next->disp(390, 78); // display nexts
				next2->disp(390, 158);
				next3->disp(390, 238);
				start = SDL_GetTicks(); // start the timer
				while( falling && SDL_GetTicks() - start < (1/((static_cast<double>(level)/25) + 1))*(SPEED - MINSPEED)+MINSPEED) // spiffy formula I took way to long to make for speeding up
				{
					while( SDL_PollEvent( &event ) ) // get events
					{
						if( event.type == SDL_QUIT ) // quit immediatly
						{
							clean();
							exit(0);
							playing = false;
						}
						else if( event.type == SDL_KEYDOWN ) // was a key pressed?
						{
							switch( event.key.keysym.sym ) // what key was pressed?
							{
								case SDLK_LEFT: // move left
									tile->check(-1, 0);
									break;
								case SDLK_RIGHT: // move right
									tile->check(1, 0);
									break;
								case SDLK_z: // rotate CCW
									tile->rotate(CC);
									break;
								case SDLK_x: // rotate CW
									tile->rotate(CW);
									break;
								case SDLK_DOWN: // soft drop
									if(!tile->check(0,1)) // if it hits something as a result of dropping once
									{
	                                    falling = false; // apply the tile
	                                }
	                                start = SDL_GetTicks(); // reset the timer
									break;
								case SDLK_UP: // hard drop
									while(tile->check(0,1)) // move it down til it cant any more
									{
										score += 2 * (level + 1); // you get points for being so daring!
									}
									start -= 1000; //Lock that sucka!
									break;
								case SDLK_n: // this is the cheat key. But you have to be cheating to use it ;D
									if(cheating)
									{
										delete tile; // basically just gives you new tiles
										tile = new Tile();
									}
									break;
								case SDLK_ESCAPE: // back to main menu
									quit = true;
									break;
								case SDLK_k: //SUPER SECRET KYLE-MODE!
									block = load_image( "kyles.png" );
									back = load_image( "kyleback.png" );
									smallblock = load_image("smallkyles.png");
									Mix_HaltMusic();
									music = Mix_LoadMUS("rawk.mp3");
									Mix_PlayMusic(music, -1);
									break;
								case SDLK_o: // back to original
									block = load_image( "blocks.png" );
									back = load_image( "back.png" );
									smallblock = load_image("smallblocks.png");
									Mix_HaltMusic();
									music = Mix_LoadMUS("music.mp3");
									Mix_PlayMusic(music, -1);
									break;
								case SDLK_c: // hold
									if(hold && !held) // if there is something held and you haven't held yet this turn
									{
										blank = hold; // switch current and held
										hold = tile;
										tile = blank;
									}
									else if(!hold) // if you've never held
									{
										hold = tile; // hold current and make more tiles
										tile = next;
										next = next2;
										next2 = next3;
										next3 = new Tile();
									}
									hold->reset(); // reset the x and y values of the tile
									held = true;
									break;
								case SDLK_p: // pause
									paused = true;
									Mix_PauseMusic(); // pause music
									Mix_PlayChannel(-1, effect, 0); // play the dee doo
									apply_surface( 0, 0, back, screen ); // cover the tiles
									numbers = TTF_RenderText_Solid(font, intToStr(score).c_str(), fontColor); // but keep the scores
									apply_surface( 550, 50, numbers, screen );
									numbers = TTF_RenderText_Solid(font, intToStr(level).c_str(), fontColor);
									apply_surface( 550, 120, numbers, screen );
									if( SDL_Flip( screen ) == -1 ) // display screen
									{
										return 1; //ERRRRRRR !!
									}
									while(paused)
									{
										while(SDL_PollEvent( &event )) // get events
										{
											if( event.type == SDL_QUIT ) // quit immediatly
											{
												quit = true;
												paused = false;
												playing = false;
											}
											else if( event.type == SDL_KEYDOWN ) // key down?
											{
												switch( event.key.keysym.sym ) // what key?
												{
													case SDLK_ESCAPE: // unpause
													case SDLK_p: // unpause
														paused = false;
														break;
													case SDLK_UP: // CHEATING!! WOO!!! UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT A B. <3 CONTRA.
														if(cheatseq == 0 || cheatseq == 1)
														{
															cheatseq++;
														}
														else
														{
															cheatseq = 0; // if you screwed up
														}
														break;
													case SDLK_DOWN:
														if(cheatseq == 2 || cheatseq == 3)
														{
															cheatseq++;
														}
														else
														{
															cheatseq = 0;
														}
														break;
													case SDLK_LEFT:
														if(cheatseq == 4 || cheatseq == 6)
														{
															cheatseq++;
														}
														else
														{
															cheatseq = 0;
														}
														break;
													case SDLK_RIGHT:
														if(cheatseq == 5 || cheatseq == 7)
														{
															cheatseq++;
														}
														else
														{
															cheatseq = 0;
														}
														break;
													case SDLK_z:
														if(cheatseq == 8)
														{
															cheatseq++;
														}
														else
														{
															cheatseq = 0;
														}
														break;
													case SDLK_x:
														if(cheatseq == 9)
														{
															Mix_PlayChannel(-1, effect, 0);
															cheating = true;
														}
														else
														{
															cheatseq = 0;
														}
														break;
													default:
														cheatseq = 0;
														break;
												}
											}
										}
									}
									cheatseq = 0; // If you screwed up the cheat sequence
									Mix_ResumeMusic(); // play that funky music
									break;
							}
							apply_surface( 0, 0, back, screen ); // put the background up
							screen << *tile; //There. Overloading. Huzzah.
							if(hold != NULL) // display hold
							{
								hold->disp(27,78);
							}
							next->disp(390, 78); // display all the nexts
							next2->disp(390, 158);
							next3->disp(390, 238);
							numbers = TTF_RenderText_Solid(font, intToStr(score).c_str(), fontColor); //display scores
							apply_surface( 550, 50, numbers, screen );
							numbers = TTF_RenderText_Solid(font, intToStr(level).c_str(), fontColor); //Pretty sweet you can use all the sweet string functions with a function that returns a string =D
							apply_surface( 550, 120, numbers, screen );
							if( SDL_Flip( screen ) == -1 ) // show screen
							{
								return 1; //ERRRRRRR !!
							}
						}
					}
					numbers = TTF_RenderText_Solid(font, intToStr(score).c_str(), fontColor); // display scores
					apply_surface( 550, 50, numbers, screen );
					numbers = TTF_RenderText_Solid(font, intToStr(level).c_str(), fontColor);
					apply_surface( 550, 120, numbers, screen );
					if( SDL_Flip( screen ) == -1 )
					{
						return 1; //ERRRRRRR !!
					}
					SDL_Delay(1);
				}
				if(tile->check(0, 1) == 0) // if it can't fall any more
				{
					falling = false; // stop falling
				}
			}
			tile->apply(); // apply it to the pile
			held = false; // you can hold again!
			score += (level+1)*checklines(lines); // check the lines and score accordingly
			level = lines/10; // level up?
			delete tile; // delete the current tile so we don't get yelled at
		}
		delete next; // delete all the other tiles if you lost
		delete next2;
		delete next3;
		delete hold;
	}
	clean(); // uninitialize SDL and everything
	return 0; // RETURN THAT ZERO, BABY.
} // Fin
