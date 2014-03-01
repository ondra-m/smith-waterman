#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include "string.h"
#include "matrix.h"
#include "matrix_file.h"

using namespace std;

// -------------------------------------------------------------------------------------------

Matrix::Matrix(){}

// -------------------------------------------------------------------------------------------

Matrix::~Matrix(){}

// -------------------------------------------------------------------------------------------

bool Matrix::load(char * file_name){
  MatrixFile file(file_name);
  file.open();

  sequence_2 = file.get_line();
  sequence_1 = file.get_line();

  if(DEBUG_LEVEL > 0){
    cout << "Sequence 1: " << sequence_1.bold() << endl;
    cout << "Sequence 2: " << sequence_2.bold() << endl;
    cout << endl;
  }

  file.close();

  size_x = sequence_1.size() + 1;
  size_y = sequence_2.size() + 1;
}

// -------------------------------------------------------------------------------------------

bool Matrix::prepare(){
  matrix.resize(size_x);

  for(int i=0; i<size_x; i++){
    matrix[i].resize(size_y);
  }
}

// -------------------------------------------------------------------------------------------

bool Matrix::fill(){

  for(int i=0; i<size_x; i++){ matrix[i][0] = 0; }
  for(int j=0; j<size_y; j++){ matrix[0][j] = 0; }

  for(int i=1; i<size_x; i++){
    for(int j=1; j<size_y; j++){
      
      int match = get_match(i, j);
      int deletion = get_deletion(i, j);
      int insertion = get_insertion(i, j);

      matrix[i][j] = get_value(match, deletion, insertion);

      if(DEBUG_LEVEL > 1){
        cout << i << " " << j << "   " 
             << sequence_1[i-1] << " " << sequence_2[j-1] << "   "
             << setw(2) << match << " " 
             << setw(2) << deletion << " " 
             << setw(2) << insertion << "    " 
             << get_value(match, deletion, insertion) << endl;

        if((j+1) == size_y){ cout << "---------------------------" << endl; }
      }      
    }
    
  }
}






// -------------------------------------------------------------------------------------------
//      0
// max  M(i-1, j-1) + w(Xi, Yj)     match/mismatch
//      M(i-1, j) + w(Xi, -)        deletion
//      M(i, j-1) + w(-, Yi)        insertion
//
int Matrix::get_value(int match, int deletion, int insertion){
  return max(0, max(match, max(deletion, insertion)));
}

// -------------------------------------------------------------------------------------------

int Matrix::get_match(int i, int j){

  return matrix[i-1][j-1] + (sequence_1[i-1] == sequence_2[j-1] ? MATCH : MISMATCH);
}

// -------------------------------------------------------------------------------------------

int Matrix::get_deletion(int i, int j){

  return matrix[i-1][j] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------

int Matrix::get_insertion(int i, int j){

  return matrix[i][j-1] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------






// -------------------------------------------------------------------------------------------

void Matrix::print(){
  String txt("Filled matrix:");

  cout << endl;
  cout << txt.bold() << endl;

  for(int i=1; i<size_x; i++){
    cout << "   ";

    for(int j=1; j<size_y; j++){
      cout << " " << setw(3) << matrix[i][j];
    }

    cout << endl;
  }
}

// -------------------------------------------------------------------------------------------
