#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "string.h"
#include "matrix.h"

using namespace std;

// -------------------------------------------------------------------------------------------

const short int DEBUG_LEVEL = 2;

// DEFAULT
int MATCH       =  5;
int MISMATCH    = -3;
int GAP_PENALTY = -4;

// -------------------------------------------------------------------------------------------

void parse_args(int argc, char * argv[]){
  switch(argc){
    case 5: GAP_PENALTY = atoi(argv[4]);
    case 4: MISMATCH    = atoi(argv[3]);
    case 3: MATCH       = atoi(argv[2]);
  }
}

// -------------------------------------------------------------------------------------------

void show_settings(){
  if(DEBUG_LEVEL > 0){
    cout << "DEBUG_LEVEL: " << DEBUG_LEVEL << endl;
    cout << "MATCH: " << MATCH << endl;
    cout << "MISMATCH: " << MISMATCH << endl;
    cout << "GAP_PENALTY: " << GAP_PENALTY << endl;
    cout << endl;
  }
}

// -------------------------------------------------------------------------------------------

int main(int argc, char * argv[]){
  
  parse_args(argc, argv);
  show_settings();

  Matrix matrix;
  matrix.load(argv[1]);
  matrix.prepare();
  matrix.fill();

  matrix.print();

  return 0;
}

// -------------------------------------------------------------------------------------------
