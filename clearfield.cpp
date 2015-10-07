//  []
//  [][]
//  []
//
// TETRIS
// FILENAME: "clearfield.cpp"
// JOSH SIMMONS
// LAST MODIFICATION : 12/14/2006 11:47:46 AM

const int FIELDX = 10;
const int FIELDY = 20;
extern int field[FIELDX][FIELDY];

//initializes the field array to all zeros (blanks)
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
