#include "back.hpp"
#include "outputtools.hpp"


using namespace std;

int main(){
	try{
		string params[5] = {"test", "secondAA", "ashfas", "18249", "Epstein didn't kill himself"};
		bool indexes[5] = {0,1,0,0,1};
		table newtable("New table", params, indexes, 5);
		map <string, string> insertion;
		insertion["ashfas"] = "some gibberish";
		insertion["test"] = "TEST ROW WOW";
		insertion["Epstein didn't kill himself"] = "absolute truth";
		newtable.insert(insertion);
	}
	catch(exception& error){
		otools::colored_out("Internal error\n", red);
		cout << error.what() << endl;
	}
}