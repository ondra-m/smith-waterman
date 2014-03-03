#ifndef _MATRIX_H
#define _MATRIX_H

class String;
class BitArray;

extern int PRINT_LEVEL;
extern int GAP_PENALTY;
extern int MATCH;
extern int MISMATCH;

class Matrix{

  String sequence_1;
  String sequence_2;

  int size_x;
  int size_y;

  unsigned long max_value;

  String result_line1;
  String result_line2;

  std::vector<BitArray> directions;

  std::vector<unsigned long> prev_row;
  std::vector<unsigned long> current_row;

  std::vector< std::pair<int, int> > path_indexes;

  public:
         Matrix();
        ~Matrix();

    bool load(char * file1, char * file2);
    bool prepare();
    bool fill();
    bool find_path();

    unsigned long get(unsigned long match, unsigned long deletion, unsigned long insertion, int &direction);
    unsigned long get_match(int x, int y);
    unsigned long get_deletion(int x);
    unsigned long get_insertion(int x);

    void print();
    void print_result();
    void print_result_sequence();
    void print_path();
    void print_matrices();
};

#endif
