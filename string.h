#ifndef _STRING_H
#define _STRING_H

class String : public std::string{

  public:
           String();
           String(std::string x);
          ~String();

    String & operator=(const String & str);
    String & operator=(const char* s);
    String & operator=(char c);

    String bold();
    String green();
    
};

#endif
