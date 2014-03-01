#include <string>

#include "string.h"

using namespace std;

// -------------------------------------------------------------------------------------------

String::String(){}
String::String(std::string x) : string(x){}

// -------------------------------------------------------------------------------------------

String & String::operator=(const String & str){
  std::string::operator=(str);
}
String & String::operator=(const char* s){
  std::string::operator=(s);
}
String & String::operator=(char c){
  std::string::operator=(c);
}

// -------------------------------------------------------------------------------------------

String::~String(){}

// -------------------------------------------------------------------------------------------

String String::bold(){
  return "\e[1m" + *this + "\e[0m";
}

// -------------------------------------------------------------------------------------------

String String::green(){
  return "\e[32m" + *this + "\e[0m";
}

// -------------------------------------------------------------------------------------------
