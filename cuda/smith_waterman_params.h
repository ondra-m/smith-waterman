struct Sequence{
  char * data;
  int size;
};

struct Directions{
  char * data;
  int size;
};

struct CUDA{
  char * sequence_1;
  char * sequence_2;

  long * current_column;
  long * prev_column;

  char * directions;

  int iteration;
  int row_count;

  int match;
  int mismatch;
  int gap_penalty;
};

struct CUDA_params{
  Sequence sequence_1;
  Sequence sequence_2;

  Directions directions;

  int column_size;

  CUDA cuda;
};
