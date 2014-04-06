#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cmath>
#include <bitset>

#include "string.h"
#include "setting.h"
#include "dna.h"
#include "smith_waterman.h"
#include "smith_waterman_params.h"
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

  size_x = this->sequence_1.size();
  size_y = this->sequence_2.size();
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::prepare(CUDA_params &params){
  // params.directions.size = ceil((float)sequence_2.size()/4.0); // 2-bit for 1 direction
  params.directions_size = size_y;

  params.result.directions = new char[size_y];
  params.result.column = new long[size_y+1];

  params.sequence_1.data = sequence_1.to_char();
  params.sequence_1.size = size_x;

  params.sequence_2.data = sequence_2.to_char();
  params.sequence_2.size = size_y;

  params.cuda.match = Setting::match;
  params.cuda.mismatch = Setting::mismatch;
  params.cuda.gap_penalty = Setting::gap_penalty;

  params.cuda.cells_per_thread = 64;
  params.cuda.threads_per_block = 512;
  params.cuda.threads_count = ceil((float)size_y / (float)params.cuda.cells_per_thread);
  params.cuda.blocks_count = ceil((float)params.cuda.threads_count / (float)params.cuda.threads_per_block);

  threads_count = params.cuda.threads_count;

  directions.resize(size_x+1);

  for(int x=0; x<size_x+1; x++){
    directions[x].make(2, size_y+1, 0);
  }
}

// -------------------------------------------------------------------------------------------

bool SmithWaterman::run(){

  long max_value, value;

  CUDA_params params;

  prepare(params);

  CUDA_init(params);

  std::vector<Score> all_scores;




  max_value = 0;
  for(int iteration=0; iteration<(size_x+(size_y/params.cuda.threads_count)); iteration++){

    params.cuda.iteration = iteration;

    CUDA_run(params);

    value = *std::max_element(params.result.column, params.result.column+size_y+1);
    if(value > max_value){
      all_scores.clear();
      best_score = max_value = value;
    }

    for(int thread=0; thread<params.cuda.threads_count; thread++){
      int x = iteration - thread;
      int y = thread * params.cuda.cells_per_thread;
      int end_y = y + params.cuda.cells_per_thread;

      // while(y < end_y && x >= 0 && x < size_x){
      while(y < end_y && y <= size_y && x >= 0 && x < size_x){

        directions[x+1].set(y+1, params.result.directions[y]);

        if(params.result.column[y+1] == max_value){
          Score s(value, x+1, y+1);

          all_scores.push_back(s);
        }

        y++;
      }
    }

  }








  CUDA_delete(params);

  find_path(all_scores);
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
  cout << "Threads: " << threads_count << endl;

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
  cout << "        ";
  for(int i=0; i<size_x; i++){
    cout << "   " << i;
  }
  cout << endl;

  cout << "        ";
  for(int i=0; i<size_x; i++){
    txt = sequence_1[i];
    cout << "   " << txt.bold();
  }
  cout << endl << endl;



  for(int y=1; y<=size_y; y++){
    cout << "    ";

    for(int x=1; x<=size_x; x++){

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

  cout << endl;
}

// -------------------------------------------------------------------------------------------
