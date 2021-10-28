#include "back.hpp"
#include "outputtools.hpp"

using namespace std;

int main(){
	try{
		
	}
	catch(exception& error){
		otools::colored_out("Internal error\n", red);
		cout << error.what() << endl;
	}
}