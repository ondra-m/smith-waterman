#include <iostream>

#include "smith_waterman_params.h"

// ===========================================================================================
// DEVICE
// ===========================================================================================



// Get value
// -------------------------------------------------------------------------------------------
__device__ void get_value(long long match, long long deletion, long long insertion, long long &value, char &direction){

  value = max((long long)0, max(match, max(deletion, insertion)));

  direction = 0;

  if(value == 0){ return; }

  if     (value == match)    { direction = 1; }
  else if(value == deletion) { direction = 2; }
  else if(value == insertion){ direction = 3; }
}



// Run
// -------------------------------------------------------------------------------------------
__global__ void run(CUDA params){

  long long match, deletion, insertion, value;

  int id = blockIdx.x * blockDim.x + threadIdx.x;
  int x = params.iteration - id;
  int y = id * params.cells_per_thread + 1;
  int end_y = y + params.cells_per_thread;

  char direction;
  bool first=true;



  while(y < end_y && y <= params.rows_count && x >= 0 && x < params.columns_count){
    match = (first == true ? params.column.before_prev : params.column.prev)[y-1] + (params.sequence_1[x] == params.sequence_2[y-1] ? params.match : params.mismatch);
    deletion = params.column.prev[y] + params.gap_penalty;
    insertion = (first == true ? params.column.prev : params.column.current)[y-1] + params.gap_penalty;

    get_value(match, deletion, insertion, value, direction);

    params.column.current[y] = value;
    params.directions[y-1] = direction;

    y++;
    first = false;
  }
}





// ===========================================================================================
// HOST
// ===========================================================================================



// Init CUDA
// -------------------------------------------------------------------------------------------
void CUDA_init(CUDA_params &params){

  params.cuda.column.size = (params.sequence_2.size+1) * sizeof(long); // first row is 0
  params.cuda.columns_count = params.sequence_1.size;
  params.cuda.rows_count = params.sequence_2.size;

  cudaMalloc( (void**)&params.cuda.sequence_1,     params.sequence_1.size );
  cudaMalloc( (void**)&params.cuda.sequence_2,     params.sequence_2.size );
  cudaMalloc( (void**)&params.cuda.column.current, params.cuda.column.size );
  cudaMalloc( (void**)&params.cuda.column.prev,    params.cuda.column.size );
  cudaMalloc( (void**)&params.cuda.column.before_prev, params.cuda.column.size );
  cudaMalloc( (void**)&params.cuda.directions,     params.directions_size ); 

  cudaMemcpy( params.cuda.sequence_1, params.sequence_1.data, params.sequence_1.size, cudaMemcpyHostToDevice );
  cudaMemcpy( params.cuda.sequence_2, params.sequence_2.data, params.sequence_2.size, cudaMemcpyHostToDevice );

  cudaMemset( params.cuda.column.current, 0, params.cuda.column.size );
  cudaMemset( params.cuda.column.before_prev, 0, params.cuda.column.size );
}



// Pre-run CUDA
// -------------------------------------------------------------------------------------------
void CUDA_delete(CUDA_params &params){
  cudaFree(params.cuda.sequence_1);
  cudaFree(params.cuda.sequence_2);
  cudaFree(params.cuda.column.current);
  cudaFree(params.cuda.column.prev);
  cudaFree(params.cuda.column.before_prev);
  cudaFree(params.cuda.directions);
}



// Prepare for run CUDA
// -------------------------------------------------------------------------------------------
void CUDA_run(CUDA_params &params){

  cudaMemset( params.cuda.directions, 0, params.directions_size );
  cudaMemcpy( params.cuda.column.before_prev, params.cuda.column.prev, params.cuda.column.size, cudaMemcpyDeviceToDevice );
  cudaMemcpy( params.cuda.column.prev, params.cuda.column.current, params.cuda.column.size, cudaMemcpyDeviceToDevice );
  cudaMemset( params.cuda.column.current, 0, params.cuda.column.size );

  run<<<params.cuda.blocks_count, params.cuda.threads_per_block>>>(params.cuda);

  cudaMemcpy( params.result.directions, params.cuda.directions, params.directions_size, cudaMemcpyDeviceToHost );
  cudaMemcpy( params.result.column, params.cuda.column.current, params.cuda.column.size, cudaMemcpyDeviceToHost );
}
