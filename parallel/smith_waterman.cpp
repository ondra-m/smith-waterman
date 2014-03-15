#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

#include <omp.h>

#include "string.h"
#include "setting.h"
#include "smith_waterman.h"
#include "dna_file.h"
#include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

SmithWaterman::SmithWaterman(){
  max_value = 0;

  result_line1 = "";
  result_line2 = "";

  path_indexes.resize(1);
  path_indexes[0].first  = 0;
  path_indexes[0].second = 0;
}

// -------------------------------------------------------------------------------------------

SmithWaterman::~SmithWaterman(){}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::load(char * file1, char * file2){
  DNAFile file;

  file.set(file1);
  file.open();
  sequence_1 = file.get_line();
  file.close();

  file.set(file2);
  file.open();
  sequence_2 = file.get_line();
  file.close();

  if(Setting::print_level >= 4){
    cout << "Sequence 1: " << sequence_1.bold() << endl;
    cout << "Sequence 2: " << sequence_2.bold() << endl;
    cout << endl;
  }

  size_x = sequence_1.size() + 1; // +1=first row is filled with 0
  size_y = sequence_2.size() + 1; // +1=first column is filled with 0
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::prepare_shared(std::vector<long> * prev_diagonal, 
                                   std::vector<long> * prev_prev_diagonal, 
                                   std::vector<long> * current_diagonal, 
                                   std::vector<BitArray> &_directions){

  _directions.resize(size_x);

  for(int x=0; x<size_x; x++){
    _directions[x].make(2, size_y, 0);
  }


  // prev_diagonal = new std::vector<long>(size_y);
  // prev_prev_diagonal = new std::vector<long>(size_y);
  // current_diagonal = new std::vector<long>(size_y);

  // for(int i=0; i<size_y; i++){
  //   (*prev_prev_diagonal)[i] = (*prev_diagonal)[i] = (*current_diagonal)[i] = 0;
  // }

}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::fill(){

  double t1, t2, t3, t4, _t1, _t2;

  long match, deletion, insertion, value, _max_value;
  int thread_id, size, iteration, x, y, start_y, end_y, direction, end_x, thread_count, _max_value_x, _max_value_y;

  std::vector<long> * prev_diagonal;
  std::vector<long> * prev_prev_diagonal;
  std::vector<long> * current_diagonal;
  std::vector<long> * tmp_diagonal;
  std::vector<BitArray> _directions;

  prev_diagonal = new std::vector<long>(size_y);
  prev_prev_diagonal = new std::vector<long>(size_y);
  current_diagonal = new std::vector<long>(size_y);

  for(int i=0; i<size_y; i++){
    (*prev_prev_diagonal)[i] = (*prev_diagonal)[i] = (*current_diagonal)[i] = 0;
  }

  prepare_shared(prev_diagonal, prev_prev_diagonal, current_diagonal, _directions);



  // Parallel ==================================================
  #pragma omp parallel num_threads(Setting::thread_count) private(thread_id, size, iteration, x, y, start_y, end_y, \
                                                         direction, match, deletion, insertion, value, _max_value, \
                                                         end_x, _max_value_x, _max_value_y, t1, t2, t3, t4, _t1, _t2) \
                                                 shared(prev_prev_diagonal, prev_diagonal, current_diagonal, tmp_diagonal, _directions)
  {
// t1 = t2 = t3 = t4 = 0;

    thread_count = omp_get_num_threads();
    thread_id    = omp_get_thread_num();


    // variables declaration
    // ---------------------
    _max_value_x = _max_value_y = _max_value = 0;

    size    = ceil(sequence_2.size()/THREAD_COUNT); // ceil = round up
    start_y = (thread_id * size) + 1;
    end_y   = start_y + size;

    // last thread has least fields
    if((thread_id+1) == THREAD_COUNT){
      size = sequence_2.size() - ((THREAD_COUNT-1) * size);
    }



    // iterration with upper diagonal matrix
    // -------------------------------------
    for(iteration=1; iteration<(size_x+size_y); iteration++){

      x = iteration-(thread_id*size);
      y = start_y;


      // go antidiagonally
      while(x > 0 && y < end_y){

        if(x >= size_x){
          x--; y++;

          continue;
        }


// _t1 = omp_get_wtime();
// _t2 = omp_get_wtime();
// t1 += (_t2 - _t1);

// _t1 = omp_get_wtime();
        match     = (*prev_prev_diagonal)[y-1] + (sequence_1[x-1] == sequence_2[y-1] ? Setting::match : Setting::mismatch);
        deletion  = (*prev_diagonal)[y] + Setting::gap_penalty;
        insertion = (*prev_diagonal)[y-1] + Setting::gap_penalty;


        value = max((long)0, max(match, max(deletion, insertion)));

        direction = 0;

        if     (value == match)     { direction = 1; }
        else if(value == insertion) { direction = 2; }
        else if(value == deletion)  { direction = 3; }

        (*current_diagonal)[y] = value;
// _t2 = omp_get_wtime();
// t2 += (_t2 - _t1);


// _t1 = omp_get_wtime();
        _directions[x].set(y, direction);
// _t2 = omp_get_wtime();
// t3 += (_t2 - _t1);

        // member coordinates of larges number
        if(value >= _max_value){
          _max_value   = value;
          _max_value_x = x;
          _max_value_y = y;
        }


        x--;
        y++;
      }


      #pragma omp barrier
// _t1 = omp_get_wtime();
      #pragma omp master
      {
        // prev_diagonal.swap(prev_prev_diagonal);
        // prev_diagonal.swap(current_diagonal);

        tmp_diagonal = prev_prev_diagonal;

        prev_prev_diagonal = prev_diagonal;
        prev_diagonal = current_diagonal;
        current_diagonal = tmp_diagonal;
      }
// _t2 = omp_get_wtime();
// t4 += (_t2 - _t1);
      #pragma omp barrier
    }



    #pragma omp critical
    {
  // cout << "[" << thread_id << "] t1: " << t1 << endl;
  // cout << "[" << thread_id << "] t2: " << t2 << endl;
  // cout << "[" << thread_id << "] t3: " << t3 << endl;
  // cout << "[" << thread_id << "] t4: " << t4 << endl;
  // cout << endl;

      if(_max_value >= max_value){
        max_value = _max_value;

        path_indexes[0].first  = _max_value_x;
        path_indexes[0].second = _max_value_y;
      }
    }

  }
  // End parallel ==============================================



// t1 = omp_get_wtime();
  directions = _directions;
// t2 = omp_get_wtime();

  // cout << "1: " << (t2-t1) << endl;
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
long SmithWaterman::get(long match, long deletion, long insertion, int &direction){

  long _max = max((long)0, max(match, max(deletion, insertion)));

  direction = 0;

  if     (_max == match)     { direction = 1; }
  else if(_max == insertion) { direction = 2; }
  else if(_max == deletion)  { direction = 3; }

  return _max;
}

// -------------------------------------------------------------------------------------------
// Matrix have +1 fields than sequence (filled with 0)
//   -> sequence -1
//
long SmithWaterman::get_match(int x, int y, std::vector<long> * prev_prev_diagonal){

  return (*prev_prev_diagonal)[y-1] + (sequence_1[x-1] == sequence_2[y-1] ? Setting::match : Setting::mismatch);
}

// -------------------------------------------------------------------------------------------

long SmithWaterman::get_deletion(int y, std::vector<long> * prev_diagonal){

  return (*prev_diagonal)[y] + Setting::gap_penalty;
}

// -------------------------------------------------------------------------------------------

long SmithWaterman::get_insertion(int y, std::vector<long> * prev_diagonal){

  return (*prev_diagonal)[y-1] + Setting::gap_penalty;
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::find_path(){

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

void SmithWaterman::print(){

  switch(Setting::print_level){
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

void SmithWaterman::print_result(){

  cout << "\nBest score: " << max_value << endl;
}

// -------------------------------------------------------------------------------------------

void SmithWaterman::print_result_sequence(){
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

void SmithWaterman::print_path(){
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

void SmithWaterman::print_matrices(){
  String txt;



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
