#ifndef _MATRIX_H
#define _MATRIX_H

class String;
class BitArray;

class SmithWaterman{

  String sequence_1;
  String sequence_2;

  int size_x;
  int size_y;

  long max_value;

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
    bool prepare_shared(std::vector<long> * prev_diagonal,
                        std::vector<long> * prev_prev_diagonal, 
                        std::vector<long> * current_diagonal, 
                        std::vector<BitArray> &_directions);
    bool fill();
    bool find_path();

    long get(long match, long deletion, long insertion, int &direction);
    long get_match(int x, int y, std::vector<long> * prev_prev_diagonal);
    long get_deletion(int y, std::vector<long> * prev_diagonal);
    long get_insertion(int y, std::vector<long> * prev_diagonal);

    void print();
    void print_result();
    void print_result_sequence();
    void print_path();
    void print_matrices();
};

#endif
