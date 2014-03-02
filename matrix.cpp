#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "string.h"
#include "matrix.h"
#include "matrix_file.h"
#include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

Matrix::Matrix(){
  max_value = 0;

  result_line1 = "";
  result_line2 = "";

  path_indexes.resize(1);
  path_indexes[0].first  = 0;
  path_indexes[0].second = 0;
}

// -------------------------------------------------------------------------------------------

Matrix::~Matrix(){}

// -------------------------------------------------------------------------------------------

bool Matrix::load(char * file1, char * file2){
  MatrixFile file;

  file.set(file1);
  file.open();
  sequence_1 = file.get_line();
  file.close();

  file.set(file2);
  file.open();
  sequence_2 = file.get_line();
  file.close();

  if(PRINT_LEVEL >= 4){
    cout << "Sequence 1: " << sequence_1.bold() << endl;
    cout << "Sequence 2: " << sequence_2.bold() << endl;
    cout << endl;
  }

  size_x = sequence_1.size() + 1;
  size_y = sequence_2.size() + 1;
}

// -------------------------------------------------------------------------------------------

bool Matrix::prepare(){
  directions.resize(size_x);

  for(int x=0; x<size_x; x++){
    directions[x].make(2, size_y, 0);
  }

  // for(int x=0; x<size_x; x++){
  //   for(int y=0; y<size_y; y++){
  //     directions[x][y] = 0;
  //   }
  // }
}

// -------------------------------------------------------------------------------------------

bool Matrix::fill(){
  prev_row.resize(size_x, 0);
  current_row.resize(size_x, 0);

  for(int y=1; y<size_y; y++){

    for(int x=1; x<size_x; x++){

      int direction;
      
      int match     = get_match(x, y);
      int deletion  = get_deletion(x);
      int insertion = get_insertion(x);

      int value = get(match, deletion, insertion, direction);

      current_row[x] = value;
      directions[x].set(y, direction);

      // member coordinates of larges number
      if(value >= max_value){
        max_value = value;

        path_indexes[0].first  = x;
        path_indexes[0].second = y;
      }


      if(PRINT_LEVEL >= 6){
        cout << x << " " << y << "   " 
             << sequence_1[x-1] << " " << sequence_2[y-1] << "   "
             << setw(2) << match << " " 
             << setw(2) << deletion << " " 
             << setw(2) << insertion << "    " 
             << value << endl;

        if((y+1) == size_y){ cout << "---------------------------" << endl; }
      }


    }

    current_row.swap(prev_row);
    
  }
}

// -------------------------------------------------------------------------------------------
//
//      0
// max  M(i-1, j-1) + w(Xi, Yj)     match/mismatch
//      M(i-1, j) + w(Xi, -)        deletion
//      M(i, j-1) + w(-, Yi)        insertion
//
//  _______________________
// |           |           |
// |   match   | insertion |
// |    01     |    10     |
// |___________|___________|
// |           |↖    ↑     |
// | deletion  |           |
// |    11     |←    D     |
// |___________|___________|
//
int Matrix::get(int match, int deletion, int insertion, int &direction){

  int _max = max(0, max(match, max(deletion, insertion)));

  direction = 0;

  if     (_max == match)     { direction = 1; }
  else if(_max == insertion) { direction = 2; }
  else if(_max == deletion)  { direction = 3; }

  return _max;
}

// -------------------------------------------------------------------------------------------
// Matrix have +1 fields than sequence
//
int Matrix::get_match(int x, int y){

  return prev_row[x-1] + (sequence_1[x-1] == sequence_2[y-1] ? MATCH : MISMATCH);
}

// -------------------------------------------------------------------------------------------

int Matrix::get_deletion(int x){

  return current_row[x-1] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------

int Matrix::get_insertion(int x){

  return prev_row[x] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------

bool Matrix::find_path(){

  int x = path_indexes[0].first;
  int y = path_indexes[0].second;

  do{

    if(directions[x].compare(y, 1)){
      x--;
      y--;

      result_line1 += sequence_1[x];
      result_line2 += sequence_2[y];
    }
    else if(directions[x].compare(y, 2)){
      y--;

      result_line1 += "-";
      result_line2 += sequence_2[y];
    }
    else if(directions[x].compare(y, 3)){
      x--;

      result_line1 += sequence_1[x];
      result_line2 += "-";
    }
    else{
      break;
    }

    path_indexes.push_back(make_pair(x, y));

  } while(true);

  std::reverse(path_indexes.begin(), path_indexes.end());
  std::reverse(result_line1.begin(), result_line1.end());
  std::reverse(result_line2.begin(), result_line2.end());
}

// -------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------

void Matrix::print(){

  switch(PRINT_LEVEL){
    case 6:
    case 5:
    case 4:
    case 3: print_matrices();
    case 2: print_path();
    case 1: print_result_sequence();
    case 0: print_result();
  }
}

// -------------------------------------------------------------------------------------------

void Matrix::print_result(){

  cout << "\nBest score: " << max_value << endl;
}

// -------------------------------------------------------------------------------------------

void Matrix::print_result_sequence(){
  String txt("Result:");

  int front_x = path_indexes.front().first;
  int front_y = path_indexes.front().second;

  int max_front = max(front_x, front_y);

  int back_x = path_indexes.back().first;
  int back_y = path_indexes.back().second;

  int back_size = abs(back_x - back_y);

  cout << endl << endl;
  cout << txt.bold() << endl;

  // Sequence 1
  txt = "";
  txt.resize(max_front, '-');
  for(int i=0; i<front_x; i++){
    txt[max_front-i-1] = sequence_1[i];
  }

  cout << "    " << txt << result_line1.bold().green();

  txt = "";
  txt.resize(back_size, '-');
  for(int i=0; (i+back_x)<sequence_1.size(); i++){
    txt[i] = sequence_1[i+back_x];
  }

  cout << txt << endl;



  // Sequence 2
  txt = "";
  txt.resize(max_front, '-');
  for(int i=0; i<front_y; i++){
    txt[max_front-i-1] = sequence_2[i];
  }

  cout << "    " << txt << result_line2.bold().green();

  txt = "";
  txt.resize(back_size, '-');
  for(int i=0; (i+back_y)<sequence_2.size(); i++){
    txt[i] = sequence_2[i+back_y];
  }

  cout << txt << endl;
}


// -------------------------------------------------------------------------------------------

void Matrix::print_path(){
  String txt("Path:");

  cout << endl << endl;
  cout << txt.bold() << endl;

  cout << "     ";
  for(std::vector< pair<int,int> >::iterator it=path_indexes.begin(); it!=path_indexes.end(); ++it){
    cout << " → [" << (*it).first << "," << (*it).second << "]";
  }
  cout << endl;
}

// -------------------------------------------------------------------------------------------

void Matrix::print_matrices(){
  String txt;

  // // Filed matrix
  // // --------------------------------------------
  // txt = "Filled matrix:";

  // cout << endl;
  // cout << txt.bold() << endl;

  // // vertical header
  // cout << "          ";
  // for(int i=0; i<sequence_1.size(); i++){
  //   cout << "  " << setw(2) << (i+1);
  // }
  // cout << endl;

  // cout << "          ";
  // for(int i=0; i<sequence_1.size(); i++){
  //   txt = sequence_1[i];
  //   cout << "   " << txt.bold();
  // }
  // cout << endl << endl;


  // for(int y=1; y<size_y; y++){
  //   cout << "     ";

  //   for(int x=1; x<size_x; x++){
  //     // horizontal header
  //     if(x == 1){
  //       txt = sequence_2[y-1];
  //       cout << setw(2) << (y) << " " << txt.bold() << " ";
  //     }

  //     cout << " " << setw(3) << matrix[x][y];
  //   }

  //   cout << endl;
  // }



  // Direction
  // --------------------------------------------
  txt = "Direction matrix:";

  cout << endl << endl;
  cout << txt.bold() << endl;

  // vertical header
  cout << "       ";
  for(int i=0; i<sequence_1.size(); i++){
    cout << "  " << (i+1);
  }
  cout << endl;

  cout << "       ";
  for(int i=0; i<sequence_1.size(); i++){
    txt = sequence_1[i];
    cout << "  " << txt.bold();
  }
  cout << endl << endl;

  for(int y=1; y<size_y; y++){
    cout << "   ";

    for(int x=1; x<size_x; x++){

      // horizontal header
      if(x == 1){
        txt = sequence_2[y-1];
        cout << y << " " << txt.bold() << " ";
      }

      txt = "";

      if(directions[x].compare(y, 1)){ txt = "↖"; }
      else if(directions[x].compare(y, 2)){ txt = "↑"; }
      else if(directions[x].compare(y, 3)){ txt = "←"; }
      else { txt = " "; }

      cout << "  " << txt;
    }

    cout << endl;
  }
}

// -------------------------------------------------------------------------------------------
