#ifndef _MATRIX_H
#define _MATRIX_H

class String;

extern int PRINT_LEVEL;
extern int GAP_PENALTY;
extern int MATCH;
extern int MISMATCH;

class Matrix{

  String sequence_1;
  String sequence_2;

  int size_x;
  int size_y;

  int max_value;

  String result_line1;
  String result_line2;

  std::vector< std::vector<short int> > matrix;
  std::vector< std::vector<short int> > directions;

  std::vector< std::pair<int, int> > path_indexes;

  public:
         Matrix();
        ~Matrix();

    bool load(char * file1, char * file2);
    bool prepare();
    bool fill();
    bool find_path();

    int get(int match, int deletion, int insertion, int &direction);
    int get_match(int x, int y);
    int get_deletion(int x, int y);
    int get_insertion(int x, int y);

    void print();
    void print_result();
    void print_result_sequence();
    void print_path();
    void print_matrices();
};

#endif
