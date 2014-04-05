#include <iostream>

#include "smith_waterman_params.h"

// ===========================================================================================
// DEVICE
// ===========================================================================================
__device__ void get_value(long long match, long long deletion, long long insertion, long long &value, char &direction){

  value = max((long long)0, max(match, max(deletion, insertion)));

  direction = 0;

  if(value == 0){ return; }

  if     (value == match)    { direction = 1; }
  else if(value == deletion) { direction = 2; }
  else if(value == insertion){ direction = 3; }
}

__global__ void run(CUDA params){

  long long match, deletion, insertion, value;

  int x = params.iteration;
  char direction;

  for(int y=1; y<=params.row_count; y++){
    match = params.prev_column[y-1] + (params.sequence_1[x] == params.sequence_2[y-1] ? params.match : params.mismatch);
    deletion = params.prev_column[y] + params.gap_penalty;
    insertion = params.current_column[y-1] + params.gap_penalty;

    get_value(match, deletion, insertion, value, direction);

    params.current_column[y] = value;
    params.directions[y-1] = direction;
  }
}



// Init CUDA
// -------------------------------------------------------------------------------------------
void CUDA_init(CUDA_params &params){

  params.column_size = (params.sequence_2.size+1) * sizeof(long); // first row is 0

  params.cuda.row_count = params.sequence_2.size;


  cudaMalloc( (void**)&params.cuda.sequence_1,     params.sequence_1.size );
  cudaMalloc( (void**)&params.cuda.sequence_2,     params.sequence_2.size );
  cudaMalloc( (void**)&params.cuda.current_column, params.column_size );
  cudaMalloc( (void**)&params.cuda.prev_column,    params.column_size );
  cudaMalloc( (void**)&params.cuda.directions,     params.directions.size ); 

  cudaMemcpy( params.cuda.sequence_1, params.sequence_1.data, params.sequence_1.size, cudaMemcpyHostToDevice );
  cudaMemcpy( params.cuda.sequence_2, params.sequence_2.data, params.sequence_2.size, cudaMemcpyHostToDevice );

  cudaMemset( params.cuda.current_column, 0, params.column_size );
}



// Pre-run CUDA
// -------------------------------------------------------------------------------------------
void CUDA_delete(CUDA_params &params){
  cudaFree(params.cuda.sequence_1);
  cudaFree(params.cuda.sequence_2);
  cudaFree(params.cuda.current_column);
  cudaFree(params.cuda.prev_column);
  cudaFree(params.cuda.directions);
}



// Prepare for run CUDA
// -------------------------------------------------------------------------------------------
void CUDA_run(CUDA_params &params){

  cudaMemset( params.cuda.directions, 0, params.directions.size );
  cudaMemcpy( params.cuda.prev_column, params.cuda.current_column, params.column_size, cudaMemcpyDeviceToDevice );

  run<<<1, 1>>>(params.cuda);

  cudaMemcpy( params.directions.data, params.cuda.directions, params.directions.size, cudaMemcpyDeviceToHost );

  // for(int i=0; i<8; i++){
  //   std::cout << tmp[i] << std::endl;
  // }
}
