#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cmath>

#include <omp.h>

#include "string.h"
#include "setting.h"
#include "dna.h"
#include "smith_waterman.h"
#include "bit_array.h"

using namespace std;

// -------------------------------------------------------------------------------------------

SmithWaterman::SmithWaterman(){
  best_score = 0;
  best_path_value = 0;

  result = "";
}

// -------------------------------------------------------------------------------------------

SmithWaterman::~SmithWaterman(){}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::load(DNA sequence_1, DNA sequence_2){

  // Sequence 1 should be larger than sequence 2
  if(sequence_1.size() > sequence_2.size()){
    this -> sequence_1 = sequence_1;
    this -> sequence_2 = sequence_2;
  }
  else{
    this -> sequence_2 = sequence_1;
    this -> sequence_1 = sequence_2;
  }

  if(Setting::debug){
    cout << "Sequence 1: " << this->sequence_1.bold() << endl;
    cout << "Sequence 2: " << this->sequence_2.bold() << endl;
    cout << endl;
  }

  size_x = this->sequence_1.size() + 1; // +1=first row is filled with 0
  size_y = this->sequence_2.size() + 1; // +1=first column is filled with 0
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
bool SmithWaterman::run(){

  // ================================================================
  // private for all threads
  // ================================================================
    int iteration, start_y, end_y, i, direction;
    int x, y, local_x, local_y, local_size_y;
    int thread_count, thread_id, last;
    long max_value, value, match, deletion, insertion;

    // for local calculation
    std::vector<long> current_column;
    std::vector<long> prev_column;
    std::vector<Score> local_scores;


  // ================================================================
  // shared for all threads
  // ================================================================

    // velikost jsou pro y
    // pro všechny jádra kromě poledního
    int local_size_x;

    // first index = thread id
    // second index = 0 -> prev square
    //                1 -> current square
    // third index = values
    std::vector< std::vector< std::vector<long> > > snake_diagonal_y;
    std::vector<BitArray> _directions;
    std::vector<Score> all_scores;

    _directions.resize(size_x);

    for(int x=0; x<size_x; x++){
      _directions[x].make(2, size_y, 0);
    }



  // ================================================================
  // Parallel
  // ================================================================
  #pragma omp parallel num_threads(Setting::thread_count) \
                       private(iteration, max_value, start_y, end_y, x, y, local_x, local_y, i, \
                               match, deletion, insertion, value, direction, thread_count, thread_id, last, local_size_y, \
                               current_column, prev_column, local_scores) \
                       shared(local_size_x, snake_diagonal_y, _directions, all_scores)
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


    max_value = -1;
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

          // reset previous result if theres is better than old ones
          if(value > max_value){
            local_scores.clear();

            max_value = value;
          }
          if(value == max_value){
            Score s(value, x, y);

            local_scores.push_back(s);
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

        all_scores.reserve(local_scores.size());
        all_scores.insert(all_scores.end(), local_scores.begin(), local_scores.end());
      }
    }

  }
  // End parallel ==============================================

  directions = _directions;

  prepare_scores(all_scores);
  find_path(all_scores);
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

  if(_max == 0){ return 0; }

  if     (_max == match)    { direction = MARK_MATCH;     }
  else if(_max == deletion) { direction = MARK_DELETION;  }
  else if(_max == insertion){ direction = MARK_INSERTION; }

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

void SmithWaterman::find_path(std::vector<Score> &all_scores){

  for(std::vector<Score>::iterator score=all_scores.begin(); score!=all_scores.end(); ++score){
    make_path(*score);
  }

  for(std::vector<Path>::iterator path=paths.begin(); path!=paths.end(); ++path){

    if((*path).value == best_path_value){
      best_path = &(*path);
      break;
    }
  }

  make_result();
}

// -------------------------------------------------------------------------------------------
//
// Remove redundant and keep only highest scores
//
void SmithWaterman::prepare_scores(std::vector<Score> &all_scores){
  // long max_value = 0;

  // // find max value
  // for(int i=0; i<all_scores.size(); i++){
  //   long value = all_scores[i].value;

  //   if(value > max_value){
  //     max_value = value;
  //   }
  // }

  // remove all except value == max_value
  std::vector<Score>::iterator it = all_scores.begin();
  while(it != all_scores.end()){
    if((*it).value != best_score){
      it = all_scores.erase(it);
    }
    else{
      ++it;
    }
  }

}

// -------------------------------------------------------------------------------------------
//
// Constructing result path
//
// x, y is +1 because of first row and column filled with zero
//
void SmithWaterman::make_path(Score &score){

  Path path;

  int x = score.x;
  int y = score.y;

  bool end=false;

  do{

    path.add(x, y);

    switch(directions[x][y]){
      case 0:
        end = true;
        break;

      case MARK_MATCH:
        x--;
        y--;

        path.result_line1 += sequence_1[x];
        path.result_line2 += sequence_2[y];

        path.value += (sequence_1[x] == sequence_2[y]) ? Setting::match : Setting::mismatch;
        break;

      case MARK_INSERTION:
        y--;

        path.result_line1 += '-';
        path.result_line2 += sequence_2[y];

        path.value += Setting::gap_penalty;
        break;

      case MARK_DELETION:
        x--;

        path.result_line1 += sequence_1[x];
        path.result_line2 += '-';

        path.value += Setting::gap_penalty;
        break;
    }

  } while(!end);

  std::reverse(path.points.begin(), path.points.end());
  std::reverse(path.result_line1.begin(), path.result_line1.end());
  std::reverse(path.result_line2.begin(), path.result_line2.end());

  if(path.value > best_path_value){
    best_path_value = path.value;
  }

  paths.push_back(path);
}

// -------------------------------------------------------------------------------------------

void SmithWaterman::make_result(){

  String result1;
  String result2;
  String result3;

  int biggest_number = max((best_path -> points).back().x, (best_path -> points).back().y);
  int digits = 0;
  do { biggest_number /= 10; digits++; } while (biggest_number != 0);
  
  int x = (best_path -> points).front().x;
  int y = (best_path -> points).front().y;

  int local_i = 0;

  for(int i=0; i<(best_path -> result_line1).size(); i++){
  
    if(local_i == 0){
      result1 = to_s(x, digits) + " ";
      result2 = string(digits+1, ' ');
      result3 = to_s(y, digits) + " ";
    }

    result1 += (best_path->result_line1)[i];
    result2 += ((best_path->result_line1)[i] == (best_path->result_line2)[i] ? '|' : ' ');
    result3 += (best_path->result_line2)[i];

    if(local_i == Setting::char_per_row || (i+1) == (best_path->result_line1).size()){
      local_i = -1;

      result1 += " " + to_s(x, digits);
      result3 += " " + to_s(y, digits);

      result += result1;
      result += '\n';
      result += result2;
      result += '\n';
      result += result3;
      result += '\n';
      result += '\n';
    }

    if((best_path->result_line1)[i] != '-'){ x++; }
    if((best_path->result_line2)[i] != '-'){ y++; }

    local_i++;
  }
}

// -------------------------------------------------------------------------------------------

void SmithWaterman::print(double duration){

  if(Setting::debug){
    print_matrices();
  }

  cout << "Match: " << Setting::match << endl;
  cout << "Mismatch: " << Setting::mismatch << endl;
  cout << "Gap penalty: " << Setting::gap_penalty << endl;

  cout << "Size of #1: " << sequence_1.size() << endl;
  cout << "Size of #2: " << sequence_2.size() << endl;

  cout << "Score: " << best_score << endl;
  cout << "Finded path: " << paths.size() << endl;

  cout << "Thread: " << Setting::thread_count << endl;
  cout << "Time: " << duration << "s" << endl;

  cout << endl;

  cout << result << endl;
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
    cout << "     " << (i+1);
  }
  cout << endl;

  cout << "       ";
  for(int i=0; i<sequence_1.size(); i++){
    txt = sequence_1[i];
    cout << "     " << txt.bold();
  }
  cout << endl << endl;

  for(int y=1; y<size_y; y++){
    cout << "    ";

    for(int x=1; x<size_x; x++){

      // horizontal header
      if(x == 1){
        txt = sequence_2[y-1];
        cout << y << " " << txt.bold() << " ";
      }

      txt = "";

      if     (directions[x].compare(y, 1)){ txt = "↖"; }
      else if(directions[x].compare(y, 2)){ txt = "↑"; }
      else if(directions[x].compare(y, 3)){ txt = "←"; }
      else { txt = " "; }

      cout << "   " << txt;
    }

    cout << endl;
  }
}

// -------------------------------------------------------------------------------------------
