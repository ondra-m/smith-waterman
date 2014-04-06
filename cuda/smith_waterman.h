#ifndef _SMITH_WATERMAN_H
#define _SMITH_WATERMAN_H

class String;
class BitArray;
class DNA;
struct CUDA_params;

std::string to_s(int x, int digits=0);
void CUDA_init(CUDA_params &params);
void CUDA_run(CUDA_params &params);
void CUDA_delete(CUDA_params &params);

// ================================================================
// Point
// ================================================================
struct Point{
  int x;
  int y;

  Point(int x=0, int y=0){
    this -> x = x;
    this -> y = y;
  }
};



// ================================================================
// Score
// ================================================================
struct Score : Point{
  long value;

  Score(unsigned long value, int x=0, int y=0) : Point(x, y){
    this -> value = value;
  }
};



// ================================================================
// Path
// ================================================================
struct Path{
  long value;

  String result_line1;
  String result_line2;

  std::vector<Point> points;

  Path(){
    this -> value = 0;
    this -> result_line1 = "";
    this -> result_line2 = "";
  }

  void add(int x, int y){
    Point point(x, y);
    points.push_back(point);
  }
};



// ================================================================
// SmithWaterman
// ================================================================
class SmithWaterman{

  static const int MARK_MATCH     = 1;
  static const int MARK_DELETION  = 2;
  static const int MARK_INSERTION = 3;

  DNA sequence_1;
  DNA sequence_2;

  int size_x;
  int size_y;

  int threads_count;

  long best_score;
  long best_path_value;

  String result;

  std::vector<BitArray> directions;

  Path * best_path;
  std::vector<Path> paths;

  public:
         SmithWaterman();
        ~SmithWaterman();

    bool load(DNA sequence_1, DNA sequence_2);  
    bool prepare(CUDA_params &params);
    bool run();

    void find_path(std::vector<Score> &all_scores);
    void make_path(Score &score);
    void make_result();

    void print(double duration=0);
    void print_matrices();
};

#endif
