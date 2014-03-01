#ifndef _MATRIX_FILE_H
#define _MATRIX_FILE_H

class MatrixFile{

  char * file_name;
  std::ifstream file;

  public:
            MatrixFile(char * file_name);
           ~MatrixFile();

    bool    open();
    bool is_open();
    bool  is_eof();
    void   close();

    std::string get_line();
};

#endif
