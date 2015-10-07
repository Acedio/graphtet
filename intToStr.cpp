//  []
//  [][]
//  []
//
// TETRIS
// FILENAME: "intToStr.cpp"
// JOSH SIMMONS
// LAST MODIFICATION : 12/14/2006 11:47:46 AM

#include <sstream>
#include <string>
using namespace std;

string intToStr(int i) //Converting ints to strs is RETARDED.
{
	ostringstream oss;
	
	oss << i;
	
	return oss.str();
}
