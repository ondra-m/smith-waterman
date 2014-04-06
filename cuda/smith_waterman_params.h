struct Sequence{
  char * data;
  int size;
};

struct Directions{

};

struct Column{
  long * current;
  long * prev;
  long * before_prev;
  int size;
};

struct CUDA{
  Column column;

  char * sequence_1;
  char * sequence_2;
  char * directions;

  int iteration;
  int rows_count;
  int columns_count;
  int match;
  int mismatch;
  int gap_penalty;

  int threads_count;
  int blocks_count;
  int threads_per_block;
  int cells_per_thread;
};

struct Result{
  char * directions;
  long * column;
};

struct CUDA_params{
  CUDA cuda;

  Sequence sequence_1;
  Sequence sequence_2;

  Result result;

  int column_size;
  int directions_size;
};
