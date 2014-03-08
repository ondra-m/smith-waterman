#include <iostream>
#include <vector>
#include <bitset>

#include "bit_array.h"
#include "string.h"

using namespace std;

// -------------------------------------------------------------------------------------------

BitArray::BitArray(){
}

// -------------------------------------------------------------------------------------------

BitArray::~BitArray(){
  // delete [] data;
}

// -------------------------------------------------------------------------------------------

void BitArray::make(int bits, int size, char value){

  this -> bits = bits;
  // this -> size = size;

  // this -> fields_in_char = 8/bits;

  // this -> char_size = (int)((bits * size)/8) + 1;

  // this -> data.resize();

  // for(int i=0; i<char_size; i++){
  //   data[i] = value;
  // }

  data.resize(bits * size, value);
}

// -------------------------------------------------------------------------------------------

void BitArray::set(int id, char value){

  // // cout << "\nBitArray::set" << endl;
  // // cout << "  id: " << id << endl;
  // // cout << "  value: " << (bitset<8>)value << endl;
  // // cout << endl;

  // int data_index = get_data_index(id);    // index on array of chars
  // int field_index = id % fields_in_char;  // index on selected char

  // // cout << "  data_index: " << data_index << endl;
  // // cout << "  field_index: " << field_index << endl;
  // // cout << endl;

  // value <<= (field_index * bits);

  // // reset
  // data[data_index] &= (~(255 << bits) << (field_index * bits));

  // data[data_index] += value;

  // // cout << "  modified_value: " << (bitset<8>)value << endl;
  // // cout << "  data[data_index]: " << (bitset<8>)data[data_index] << endl;
  // // cout << endl;

  id *= bits;

  for(int i=0; i<bits; i++){
    data[id+i] = (value >> i) & 1;
  }
}

// -------------------------------------------------------------------------------------------

bool BitArray::compare(int id, char value){

  // int data_index = get_data_index(id);    // index on array of chars
  // int field_index = id % fields_in_char;  // index on selected char

  // // value <<= (field_index * bits);

  // char get_value = (data[data_index] >> (field_index * bits)) & (~(255 << bits));

  // cout << "id: " << id << endl;
  // cout << "value: " << (bitset<8>)value << endl;
  // // cout << "data[data_index]: " << (bitset<8>)data[data_index] << endl;
  // // cout << "(field_index * bits): " << (field_index * bits) << endl;
  // // cout << "(data[data_index] >> (field_index * bits)): " << (bitset<8>)value << endl;
  // // cout << "(~(255 << bits)): " << (bitset<8>)(~(255 << bits)) << endl;
  // // cout << "get_value: " << (bitset<8>)get_value << endl;

  // // cout << endl;

  // if(get_value == value){
  //   return true;
  // }


  // // if((value & data[data_index]) == value){
  // //   return true;
  // // }

  // return false;

  id *= bits;

  for(int i=0; i<bits; i++){
    if(data[id+i] != ((value >> i) & 1)){
      return false;
    }
  }


  return true;
}

// -------------------------------------------------------------------------------------------

int BitArray::get_data_index(int id){
  // return (int)(id / fields_in_char);
}

// -------------------------------------------------------------------------------------------

void BitArray::print(){

  int i=1;
  for(std::vector<bool>::iterator it=data.begin(); it!=data.end(); ++it){
    
    cout << (*it);

    if((i % 8) == 0){ cout << " "; }

    i++;
  }

  cout << endl;
}

// -------------------------------------------------------------------------------------------
// Static function
//
void BitArray::test(){

  String _true("TRUE");
  String _false("FALSE");

  _true  = _true.bold().green();
  _false = _false.bold().red();

  BitArray array;

  array.make(2, 20);

  array.set(3, 2);
  array.set(1, 3);
  array.set(5, 3);

  array.set(4, 1);
  array.set(4, 1);
  array.set(4, 1);

  cout << endl;
  cout << (array.compare(3, 0) == false ? _true : _false) << endl;
  cout << (array.compare(3, 1) == false ? _true : _false) << endl;
  cout << (array.compare(3, 2) == true  ? _true : _false) << endl;
  cout << (array.compare(3, 3) == false ? _true : _false) << endl;

  cout << endl;
  cout << (array.compare(1, 0) == false ? _true : _false) << endl;
  cout << (array.compare(1, 1) == false ? _true : _false) << endl;
  cout << (array.compare(1, 2) == false ? _true : _false) << endl;
  cout << (array.compare(1, 3) == true  ? _true : _false) << endl;

  cout << endl;
  cout << (array.compare(6, 0) == true  ? _true : _false) << endl;
  cout << (array.compare(6, 1) == false ? _true : _false) << endl;
  cout << (array.compare(6, 2) == false ? _true : _false) << endl;
  cout << (array.compare(6, 3) == false ? _true : _false) << endl;

  cout << endl;
  cout << (array.compare(4, 0) == false ? _true : _false) << endl;
  cout << (array.compare(4, 1) == true  ? _true : _false) << endl;
  cout << (array.compare(4, 2) == false ? _true : _false) << endl;
  cout << (array.compare(4, 3) == false ? _true : _false) << endl;

  array.set(3, 1);

  cout << endl;
  cout << (array.compare(3, 0) == false ? _true : _false) << endl;
  cout << (array.compare(3, 1) == true  ? _true : _false) << endl;
  cout << (array.compare(3, 2) == false ? _true : _false) << endl;
  cout << (array.compare(3, 3) == false ? _true : _false) << endl;
}

// -------------------------------------------------------------------------------------------
