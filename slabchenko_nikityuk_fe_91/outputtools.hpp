#ifndef OTOOLS_H
#define OTOOLS_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm> 
#include <functional>
#include <cctype>
#include <locale>

enum color{red, yellow, green};

namespace std::otools{
	void colored_out(string s, color c);
	vector<string> explode(string const & s, char delim);
	static inline string & ltrim(string &s);
	static inline string & rtrim(string &s);
	static inline string & trim(string &s);
}

#endif