#ifndef _BIT_ARRAY_H
#define _BIT_ARRAY_H

class BitArray{

  unsigned short int bits;           // # bits of the one field
  // unsigned short int fields_in_char; // # field in one char
  // unsigned int size;                 // size of array
  // unsigned int char_size;            // # of allocated chars

  // char * data;

  std::vector<bool> data;

  public:
         BitArray();
        ~BitArray();

    void make(int bits, int size, char value=0);

    void set(int id, char value);
    bool compare(int id, char value);
    int get_data_index(int id);

    void print();

    static void test();

};

#endif
