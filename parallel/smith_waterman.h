#ifndef _MATRIX_H
#define _MATRIX_H

class String;
class BitArray;

class SmithWaterman{

  String sequence_1;
  String sequence_2;

  int size_x;
  int size_y;

  long best_score;

  String result_line1;
  String result_line2;

  std::vector<BitArray> directions;

  std::vector<long> prev_row;
  std::vector<long> current_row;

  std::vector< std::pair<int, int> > path_indexes;

  public:
         SmithWaterman();
        ~SmithWaterman();

    bool load(char * file1, char * file2);
    bool prepare_variables();
    bool fill();
    bool find_path();

    long get(long match, long deletion, long insertion, int &direction);
    long get_match(int x, int y, int local_x, int local_y, std::vector<long> &prev_column, std::vector<long> &prev_snake_diagonal_y);
    long get_deletion(int local_y, std::vector<long> &prev_column);
    long get_insertion(int local_x, int local_y, std::vector<long> &current_column, std::vector<long> &prev_snake_diagonal_y);

    void print();
    void print_result();
    void print_result_sequence();
    void print_path();
    void print_matrices();
};

#endif
