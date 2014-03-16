#include <iostream>
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
  best_score = 0;

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

  if(sequence_1.size() < sequence_2.size()){
    sequence_1.swap(sequence_2);
  }

  if(Setting::print_level >= 4){
    cout << "Sequence 1: " << sequence_1.bold() << endl;
    cout << "Sequence 2: " << sequence_2.bold() << endl;
    cout << endl;
  }

  size_x = sequence_1.size() + 1; // +1=first row is filled with 0
  size_y = sequence_2.size() + 1; // +1=first column is filled with 0
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::prepare_variables(){

}

// -------------------------------------------------------------------------------------------
//
// Snake diagonal        Snake diagonal x       Snake diagonal y
//
//             |                     |                     
//             |                     |                     
//        _____|                     |                _____
//       |                     |                     
//       |                     |                     
//  _____|                     |                _____
// |                     |                     
// |                     |                     
// |                     |                     
//
bool SmithWaterman::fill(){

  prepare_variables();

  // ===== private for all threads =============================
    int iteration, max_value_x, max_value_y, start_y, end_y, i, direction;
    int x, y, local_x, local_y, local_size_y;
    int thread_count, thread_id, last;
    long max_value, value, match, deletion, insertion;

    // for local calculation
    std::vector<long> current_column;
    std::vector<long> prev_column;


  // ===== shared for all threads ==============================

    // velikost jsou pro y
    // pro všechny jádra kromě poledního
    int local_size_x;

    // first index = thread id
    // second index = 0 -> prev square
    //                1 -> current square
    // third index = values
    std::vector< std::vector< std::vector<long> > > snake_diagonal_y;

    std::vector<BitArray> _directions;

    _directions.resize(size_x);

    for(int x=0; x<size_x; x++){
      _directions[x].make(2, size_y, 0);
    }


  // Parallel ==================================================
  #pragma omp parallel num_threads(Setting::thread_count) \
                       private(iteration, max_value, max_value_x, max_value_y, start_y, end_y, x, y, local_x, local_y, i, \
                               match, deletion, insertion, value, direction, thread_count, thread_id, last, local_size_y, \
                               current_column, prev_column) \
                       shared(local_size_x, snake_diagonal_y, _directions)
  {

    thread_count = omp_get_num_threads();
    thread_id    = omp_get_thread_num();



    // variables declaration
    // ---------------------
    #pragma omp master
    {
      local_size_x = ceil((float)sequence_2.size()/(float)thread_count);

      snake_diagonal_y.resize(thread_count);
      
      for(i=0; i<thread_count; i++){

        snake_diagonal_y[i].resize(2);

        snake_diagonal_y[i][0].resize(local_size_x+1, 0);
        snake_diagonal_y[i][1].resize(local_size_x+1, 0);
      }
    }
    #pragma omp barrier

    last = (thread_id+1) == thread_count ? true : false;

    if(last){
      // last thread
      local_size_y = sequence_2.size() - (local_size_x*(thread_count-1));

      if(local_size_y < 0){
        local_size_y = 0;
      }
    }
    else{
      // other threads
      local_size_y = local_size_x;
    }


    max_value = 0;
    start_y = (thread_id * local_size_x) + 1;
    end_y   = start_y + local_size_y;

    current_column.resize(local_size_y, 0);
    prev_column.resize(local_size_y, 0);



    // iterration
    // -------------------------------------
    for(iteration=1; iteration<(size_x+size_y); iteration++){

      // na mojí čísti přesunu poslední prvek na první
      // diagonála se posunuje ale každý prvek je zkoumaný match a insertion
      if(!last){
        snake_diagonal_y[thread_id+1][1][0] = snake_diagonal_y[thread_id+1][0].back();
      }


      x = (iteration*local_size_x)-(thread_id*local_size_x)-local_size_x+1;
      for(local_x=0; local_x<local_size_x && x<size_x && x>0; local_x++){

        y = start_y;
        for(local_y=0; local_y<local_size_y && y<size_y; local_y++){

          // because first line is filled with zero, index of snake_diagonal == thread_id+1
          // so prev snake_diagonal == thread_id
          match     = get_match(x, y, local_x, local_y, prev_column, snake_diagonal_y[thread_id][0]);
          deletion  = get_deletion(local_y, prev_column);
          insertion = get_insertion(local_x, local_y, current_column, snake_diagonal_y[thread_id][0]);

          value = get(match, deletion, insertion, direction);

          current_column[local_y] = value;
          if((local_y+1) == local_size_y && !last){
            snake_diagonal_y[thread_id+1][1][local_x+1] = value;
          }

          _directions[x].set(y, direction);

          // member coordinates of larges number
          if(value >= max_value){
            max_value   = value;
            max_value_x = x;
            max_value_y = y;
          }

          y++;
        }

        prev_column.swap(current_column);
        x++;
      }


      #pragma omp barrier
        if(!last){
          std::reverse(snake_diagonal_y[thread_id+1].begin(), snake_diagonal_y[thread_id+1].end());
        }
      #pragma omp barrier
    }


    #pragma omp critical
    {
      if(max_value >= best_score){
        best_score = max_value;

        path_indexes[0].first  = max_value_x;
        path_indexes[0].second = max_value_y;
      }
    }

  }
  // End parallel ==============================================


  directions = _directions;
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
long SmithWaterman::get_match(int x, int y, int local_x, int local_y, std::vector<long> &prev_column, std::vector<long> &prev_snake_diagonal_y){

  if(local_y == 0){
    return prev_snake_diagonal_y[local_x] + (sequence_1[x-1] == sequence_2[y-1] ? Setting::match : Setting::mismatch);
  }

  return prev_column[local_y-1] + (sequence_1[x-1] == sequence_2[y-1] ? Setting::match : Setting::mismatch);
}

// -------------------------------------------------------------------------------------------

long SmithWaterman::get_deletion(int local_y, std::vector<long> &prev_column){

  return prev_column[local_y] + Setting::gap_penalty;
}

// -------------------------------------------------------------------------------------------

long SmithWaterman::get_insertion(int local_x, int local_y, std::vector<long> &current_column, std::vector<long> &prev_snake_diagonal_y){

  if(local_y == 0){
    return prev_snake_diagonal_y[local_x+1] + Setting::gap_penalty; 
  }

  return current_column[local_y-1] + Setting::gap_penalty;
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::find_path(){

  // Constructing result path
  // ------------------------

  String result_line1("");
  String result_line2("");

  int x = path_indexes[0].first;
  int y = path_indexes[0].second;

  int biggest_number = max(x, y);
  int digits = 0;
  do { biggest_number /= 10; digits++; } while (biggest_number != 0);

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




  // Constructing result seqence
  // ---------------------------
  
  x = path_indexes[0].first;
  y = path_indexes[0].second;


  int local_i = 0;

  for(int i=0; i<result_line1.size(); i++){
  
    if(local_i == 0){
      std::vector<String> tmp;
      tmp.resize(3);

      tmp[0] = to_s(x, digits) + " ";
      tmp[1] = string(digits+1, ' ');
      tmp[2] = to_s(y, digits) + " ";

      result_line.push_back(tmp);
    }

    result_line.back()[0] += result_line1[i];
    result_line.back()[1] += (result_line1[i] == result_line2[i] ? '|' : ' ');
    result_line.back()[2] += result_line2[i];

    if(local_i == Setting::char_per_row || (i+1) == result_line1.size()){
      local_i = -1;

      result_line.back()[0] += " " + to_s(x, digits);
      result_line.back()[2] += " " + to_s(y, digits);     
    }

    if(result_line1[i] != '-'){ x++; }
    if(result_line2[i] != '-'){ y++; }

    local_i++;
  }
}

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

  cout << "\nBest score: " << best_score << endl;
}

// -------------------------------------------------------------------------------------------

void SmithWaterman::print_result_sequence(){

  String txt("Result:");

  cout << endl << endl;
  cout << txt.bold() << endl;

  for(std::vector< std::vector<String> >::iterator it=result_line.begin(); it!=result_line.end(); ++it){
    cout << (*it)[0] << endl;
    cout << (*it)[1] << endl;
    cout << (*it)[2] << endl;
    cout << endl;
  }
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
