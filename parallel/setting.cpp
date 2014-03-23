#include <iostream>
#include "getopt_pp.h"

#include "setting.h"

using namespace std;

int Setting::thread_count = THREAD_COUNT;
int Setting::match        = MATCH;
int Setting::mismatch     = MISMATCH;
int Setting::gap_penalty  = GAP_PENALTY;
int Setting::char_per_row = CHAR_PER_ROW;
bool Setting::debug       = DEBUG;

// -------------------------------------------------------------------------------------------

Setting::Setting(){}

// -------------------------------------------------------------------------------------------

Setting::~Setting(){}

// -------------------------------------------------------------------------------------------

void Setting::parse(int argc, char * argv[]){
  GetOpt::GetOpt_pp ops(argc, argv);

  ops >> GetOpt::Option('t', "thread", thread_count);
  ops >> GetOpt::Option('m', "match", match);
  ops >> GetOpt::Option('s', "mismatch", mismatch);
  ops >> GetOpt::Option('g', "gap", gap_penalty);
  ops >> GetOpt::Option('c', "char", char_per_row);
  ops >> GetOpt::OptionPresent('d', "debug", debug);

  if(debug){
    print();
  }
}

// -------------------------------------------------------------------------------------------

void Setting::print(){
  cout << "THREAD_COUNT: " << thread_count << endl;
  cout << "MATCH: "        << match << endl;
  cout << "MISMATCH: "     << mismatch << endl;
  cout << "GAP_PENALTY: "  << gap_penalty << endl;
  cout << "DEBUG: "        << std::boolalpha << debug << endl;
  cout << endl;
}

// -------------------------------------------------------------------------------------------
