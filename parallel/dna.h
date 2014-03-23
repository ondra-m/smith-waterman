#ifndef _DNA_H
#define _DNA_H

class String;

class DNA : public String{

  public:
            DNA();
           ~DNA();

    DNA & operator=(const String & str);
    DNA & operator=(const char* s);
    DNA & operator=(char c);

    bool from_file(char * file_name);
};

#endif
