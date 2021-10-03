#ifndef OTOOLS_H
#define OTOOLS_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm> 
#include <cctype>
#include <locale>

enum color{red, yellow, green};

namespace std::otools{
	void colored_out(string s, color c);
	vector<string> explode(string const & s, char delim);
}

#endif