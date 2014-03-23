#include <fstream>

#include "string.h"
#include "dna.h"

using namespace std;

// -------------------------------------------------------------------------------------------

DNA::DNA(){
  (*this) = "";
}

// -------------------------------------------------------------------------------------------

DNA::~DNA(){}

// -------------------------------------------------------------------------------------------

DNA & DNA::operator=(const String & str){
  String::operator=(str);
}

DNA & DNA::operator=(const char* s){
  String::operator=(s);
}

DNA & DNA::operator=(char c){
  String::operator=(c);
}


// -------------------------------------------------------------------------------------------

bool DNA::from_file(char * file_name){

  std::ifstream file;

  file.open(file_name, std::ifstream::in);

  string tmp;

  do{
    getline(file, tmp);
  } while(tmp[0] == '#' || tmp[0] == '>' || tmp[0] == '\n');

  (*this) = tmp;

  file.close();
}

// -------------------------------------------------------------------------------------------
