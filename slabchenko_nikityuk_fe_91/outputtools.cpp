#include "outputtools.hpp"

using namespace std;

void otools::colored_out(string s, color c){
	switch(c){
		case 0:
			cout << "\033[31;1m" + s + "\033[0m";
		break;
		case 1:
			cout << "\033[33;1m" + s + "\033[0m";
		break;
		case 2:
			cout << "\033[32;1m" + s + "\033[0m";
		break;
	}
}


vector<string> otools::explode(string const & s, char delim){
    vector<string> result;
    istringstream iss(s);
    for (string token; getline(iss, token, delim);)
        result.push_back(move(token));
    return result;
}