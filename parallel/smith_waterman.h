#ifndef _SMITH_WATERMAN_H
#define _SMITH_WATERMAN_H

class String;
class BitArray;
class DNA;

std::string to_s(int x, int digits=0);


struct Point{
  int x;
  int y;

  Point(int x=0, int y=0){
    this -> x = x;
    this -> y = y;
  }
};

struct Score : Point{
  long value;

  Score(unsigned long value, int x=0, int y=0) : Point(x, y){
    this -> value = value;
  }
};

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


class SmithWaterman{

  static const int MARK_MATCH     = 1;
  static const int MARK_DELETION  = 2;
  static const int MARK_INSERTION = 3;

  DNA sequence_1;
  DNA sequence_2;

  int size_x;
  int size_y;

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
    bool run();

    void find_path(std::vector<Score> &all_scores);
    void prepare_scores(std::vector<Score> &all_scores);
    void make_path(Score &score);
    void make_result();

    long get(long match, long deletion, long insertion, int &direction);
    long get_match(int x, int y, int local_x, int local_y, std::vector<long> &prev_column, std::vector<long> &prev_snake_diagonal_y);
    long get_deletion(int local_y, std::vector<long> &prev_column);
    long get_insertion(int local_x, int local_y, std::vector<long> &current_column, std::vector<long> &prev_snake_diagonal_y);

    void print(double duration=0);
    void print_result();
    void print_result_sequence();
    void print_path();
    void print_matrices();
};

#endif
