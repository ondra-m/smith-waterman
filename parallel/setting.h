#ifndef _SETTING_H
#define _SETTING_H

extern const int THREAD_COUNT;
extern const int PRINT_LEVEL;
extern const int GAP_PENALTY;
extern const int MATCH;
extern const int MISMATCH;

class Setting{

  public:
                static int thread_count;
                static int match;
                static int mismatch;
                static int gap_penalty;
                static int print_level;

                Setting();
               ~Setting();

    static void parse(int argc, char * argv[]);
    static void print();
};

#endif
