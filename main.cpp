#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#include "string.h"
#include "matrix.h"
#include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

// DEFAULT
int PRINT_LEVEL =  4;
int MATCH       =  5;
int MISMATCH    = -3;
int GAP_PENALTY = -4;

// -------------------------------------------------------------------------------------------

void parse_args(int argc, char * argv[]){
  switch(argc){
    case 7: PRINT_LEVEL = atoi(argv[6]);
    case 6: GAP_PENALTY = atoi(argv[5]);
    case 5: MISMATCH    = atoi(argv[4]);
    case 4: MATCH       = atoi(argv[3]);
  }
}

// -------------------------------------------------------------------------------------------

void show_settings(){
  if(PRINT_LEVEL >= 5){
    cout << "PRINT_LEVEL: " << PRINT_LEVEL << endl;
    cout << "MATCH: " << MATCH << endl;
    cout << "MISMATCH: " << MISMATCH << endl;
    cout << "GAP_PENALTY: " << GAP_PENALTY << endl;
    cout << endl;
  }
}

// -------------------------------------------------------------------------------------------

int main(int argc, char * argv[]){

  // BitArray::test();

  parse_args(argc, argv);
  show_settings();

  Matrix matrix;
  matrix.load(argv[1], argv[2]);
  matrix.prepare();
  matrix.fill();
  matrix.find_path();

  matrix.print();

  return 0;
}

// -------------------------------------------------------------------------------------------
