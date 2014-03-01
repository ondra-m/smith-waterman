#ifndef _MATRIX_H
#define _MATRIX_H

class String;

extern const short int DEBUG_LEVEL;
extern int GAP_PENALTY;
extern int MATCH;
extern int MISMATCH;

class Matrix{

  String sequence_1;
  String sequence_2;

  int size_x;
  int size_y;

  std::vector< std::vector<short int> > matrix;

  public:
         Matrix();
        ~Matrix();

    bool load(char * file_name);
    bool prepare();
    bool fill();

    int get_value(int match, int deletion, int insertion);
    int get_match(int i, int j);
    int get_deletion(int i, int j);
    int get_insertion(int i, int j);

    void print();
};

#endif
