// #include <stdlib.h>
// #include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
// #include <bitset>

// #include <stdlib.h>
// #include <omp.h>

#include "setting.h"
#include "string.h"
#include "dna.h"
#include "smith_waterman.h"
// #include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

// DEFAULT
const int  MATCH        =  5;
const int  MISMATCH     = -3;
const int  GAP_PENALTY  = -4;
const int  CHAR_PER_ROW = 50;
const bool DEBUG       = false;

// -------------------------------------------------------------------------------------------

string to_s(int x, int digits){
  ostringstream tmp;

  if(digits == 0){
    tmp << x;
  }
  else{
    tmp << setw(digits) << x;
  }
  
  return tmp.str();
}

// -------------------------------------------------------------------------------------------

int main(int argc, char * argv[]){
  // BitArray::test();

  // double t1 = omp_get_wtime();

  Setting::parse(argc, argv);

  DNA sequence_1, sequence_2;
  sequence_1.from_file(argv[1]);
  sequence_2.from_file(argv[2]);

  SmithWaterman sw;
  sw.load(sequence_1, sequence_2);
  sw.run();

  // double t2 = omp_get_wtime();
  
  // sw.print(t2-t1);

  return 0;
}

// -------------------------------------------------------------------------------------------
