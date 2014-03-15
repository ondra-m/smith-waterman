#include <iostream>
#include "getopt_pp.h"

#include "setting.h"

using namespace std;

int Setting::thread_count = THREAD_COUNT;
int Setting::match        = MATCH;
int Setting::mismatch     = MISMATCH;
int Setting::gap_penalty  = GAP_PENALTY;
int Setting::print_level  = PRINT_LEVEL;

// -------------------------------------------------------------------------------------------

Setting::Setting(){}

// -------------------------------------------------------------------------------------------

Setting::~Setting(){}

// -------------------------------------------------------------------------------------------

void Setting::parse(int argc, char * argv[]){
  GetOpt::GetOpt_pp ops(argc, argv);

  ops >> GetOpt::Option('t', "thread_count", thread_count);
  ops >> GetOpt::Option('m', "match", match);
  ops >> GetOpt::Option('s', "mismatch", mismatch);
  ops >> GetOpt::Option('g', "gap_penalty", gap_penalty);
  ops >> GetOpt::Option('p', "print_level", print_level);

  if(print_level >= 2){
    print();
  }
}

// -------------------------------------------------------------------------------------------

void Setting::print(){
  cout << "THREAD_COUNT: " << thread_count << endl;
  cout << "MATCH: "        << match << endl;
  cout << "MISMATCH: "     << mismatch << endl;
  cout << "GAP_PENALTY: "  << gap_penalty << endl;
  cout << "PRINT_LEVEL: "  << print_level << endl;
  cout << endl;
}

// -------------------------------------------------------------------------------------------
