#ifndef _MATRIX_FILE_H
#define _MATRIX_FILE_H

class DNAFile{

  char * file_name;
  std::ifstream file;

  public:
            DNAFile();
           ~DNAFile();

    void     set(char * file_name);

    bool    open();
    bool is_open();
    bool  is_eof();
    void   close();

    std::string get_line();
};

#endif
