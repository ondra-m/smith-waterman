#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "string.h"
#include "matrix.h"
#include "matrix_file.h"

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

bool Matrix::load(char * file_name){
  MatrixFile file(file_name);
  file.open();

  sequence_1 = file.get_line();
  sequence_2 = file.get_line();

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
  directions.resize(size_x);

  for(int x=0; x<size_x; x++){
    matrix[x].resize(size_y);
    directions[x].resize(size_y);
  }

  for(int x=0; x<size_x; x++){ matrix[x][0] = 0; }
  for(int y=0; y<size_y; y++){ matrix[0][y] = 0; }

  for(int x=0; x<size_x; x++){
    for(int y=0; y<size_y; y++){
      directions[x][y] = 0;
    }
  }
}

// -------------------------------------------------------------------------------------------

bool Matrix::fill(){

  for(int y=1; y<size_y; y++){
    for(int x=1; x<size_x; x++){

      int direction;
      
      int match     = get_match(x, y);
      int deletion  = get_deletion(x, y);
      int insertion = get_insertion(x, y);

      int value = get(match, deletion, insertion, direction);

      matrix[x][y] = value;
      directions[x][y] = direction;

      // member coordinates of larges number
      if(value >= max_value){
        max_value = value;

        path_indexes[0].first  = x;
        path_indexes[0].second = y;
      }

      if(DEBUG_LEVEL > 1){
        cout << x << " " << y << "   " 
             << sequence_1[x-1] << " " << sequence_2[y-1] << "   "
             << setw(2) << match << " " 
             << setw(2) << deletion << " " 
             << setw(2) << insertion << "    " 
             << value << endl;

        if((y+1) == size_y){ cout << "---------------------------" << endl; }
      }      
    }
    
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
// |    001    |    010    |
// |___________|___________|
// |           |↖    ↑     |
// | deletion  |           |
// |    100    |←    D     |
// |___________|___________|
//
int Matrix::get(int match, int deletion, int insertion, int &direction){

  int _max = max(0, max(match, max(deletion, insertion)));

  direction = 0;

  if(_max == match)     { direction += 1; }
  if(_max == insertion) { direction += 2; }
  if(_max == deletion)  { direction += 4; }

  return _max;
}

// -------------------------------------------------------------------------------------------
// Matrix have +1 fields than sequence
//
int Matrix::get_match(int x, int y){

  return matrix[x-1][y-1] + (sequence_1[x-1] == sequence_2[y-1] ? MATCH : MISMATCH);
}

// -------------------------------------------------------------------------------------------

int Matrix::get_deletion(int x, int y){

  return matrix[x-1][y] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------

int Matrix::get_insertion(int x, int y){

  return matrix[x][y-1] + GAP_PENALTY;
}

// -------------------------------------------------------------------------------------------

bool Matrix::find_path(){

  int x = path_indexes[0].first;
  int y = path_indexes[0].second;

  do{

    if((directions[x][y] & 1) == 1){
      x--;
      y--;

      result_line1 += sequence_1[x];
      result_line2 += sequence_2[y];
    }
    else if((directions[x][y] & 2) == 2){
      y--;

      result_line1 += "-";
      result_line2 += sequence_2[y];
    }
    else if((directions[x][y] & 4) == 4){
      x--;

      result_line1 += sequence_1[x];
      result_line2 += "-";
    }

    path_indexes.push_back(make_pair(x, y));

    if(matrix[x][y] == 0){ break; }

  } while(true);

  std::reverse(path_indexes.begin(), path_indexes.end());
  std::reverse(result_line1.begin(), result_line1.end());
  std::reverse(result_line2.begin(), result_line2.end());
}

// -------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------

void Matrix::print(){
  String txt;

  // Filed matrix
  // --------------------------------------------
  txt = "Filled matrix:";

  cout << endl;
  cout << txt.bold() << endl;

  // vertical header
  cout << "          ";
  for(int i=0; i<sequence_1.size(); i++){
    cout << "  " << setw(2) << (i+1);
  }
  cout << endl;

  cout << "          ";
  for(int i=0; i<sequence_1.size(); i++){
    txt = sequence_1[i];
    cout << "   " << txt.bold();
  }
  cout << endl << endl;


  for(int y=1; y<size_y; y++){
    cout << "     ";

    for(int x=1; x<size_x; x++){
      // horizontal header
      if(x == 1){
        txt = sequence_2[y-1];
        cout << setw(2) << (y) << " " << txt.bold() << " ";
      }

      cout << " " << setw(3) << matrix[x][y];
    }

    cout << endl;
  }



  // Direction
  // --------------------------------------------
  txt = "Direction matrix:";

  cout << endl << endl;
  cout << txt.bold() << endl;

  for(int y=1; y<size_y; y++){
    cout << "   ";

    for(int x=1; x<size_x; x++){
      txt = "";

      if((directions[x][y] & 4) == 4){ txt += "←"; } else { txt += " "; }
      if((directions[x][y] & 1) == 1){ txt += "↖"; } else { txt += " "; }
      if((directions[x][y] & 2) == 2){ txt += "↑"; } else { txt += " "; }

      cout << "  " << txt;
    }

    cout << endl;
  }



  // Path
  // --------------------------------------------
  txt = "Path:";

  cout << endl << endl;
  cout << txt.bold() << endl;

  cout << "     ";
  for(std::vector< pair<int,int> >::iterator it=path_indexes.begin(); it!=path_indexes.end(); ++it){
    cout << " → [" << (*it).first << "," << (*it).second << "]";
  }
  cout << endl;
}

// -------------------------------------------------------------------------------------------

void Matrix::print_result(){

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

  cout << "\n    Best score: " << max_value << endl;
}

// -------------------------------------------------------------------------------------------
