#ifndef _STRING_H
#define _STRING_H

class String : public std::string{

  public:
           String();
           String(std::string x);
          ~String();

    String bold();
};

#endif
