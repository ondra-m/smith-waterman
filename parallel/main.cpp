// http://download.springer.com/static/pdf/469/chp%253A10.1007%252F978-3-642-13119-6_7.pdf?auth66=1394477403_0f012a06da3478721d94ba78b5e825e9&ext=.pdf
// http://jawadmanzoor.files.wordpress.com/2012/01/final-report-ampp.pdf

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#include <stdio.h>

#include "string.h"
#include "smith_waterman.h"
#include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

int THREAD_COUNT = 2; // 0=auto

// DEFAULT
int PRINT_LEVEL =  4;
int MATCH       =  5;
int MISMATCH    = -3;
int GAP_PENALTY = -4;

// -------------------------------------------------------------------------------------------

void parse_args(int argc, char * argv[]){
  switch(argc){
    case 8: THREAD_COUNT = atoi(argv[7]);
    case 7: PRINT_LEVEL  = atoi(argv[6]);
    case 6: GAP_PENALTY  = atoi(argv[5]);
    case 5: MISMATCH     = atoi(argv[4]);
    case 4: MATCH        = atoi(argv[3]);
  }
}

// -------------------------------------------------------------------------------------------

void show_settings(){
  // if(PRINT_LEVEL >= 5){
    cout << "THREAD_COUNT: " << THREAD_COUNT << endl;
    cout << "PRINT_LEVEL: " << PRINT_LEVEL << endl;
    cout << "MATCH: " << MATCH << endl;
    cout << "MISMATCH: " << MISMATCH << endl;
    cout << "GAP_PENALTY: " << GAP_PENALTY << endl;
    cout << endl;
  // }
}

// -------------------------------------------------------------------------------------------

int main(int argc, char * argv[]){

  // BitArray::test();

  parse_args(argc, argv);
  show_settings();

  SmithWaterman sw;
  sw.load(argv[1], argv[2]);
  sw.fill();
  sw.find_path();

  sw.print();

  return 0;
}

// -------------------------------------------------------------------------------------------
