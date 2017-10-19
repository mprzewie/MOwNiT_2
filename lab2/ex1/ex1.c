#include <stdio.h>
#include <gsl/gsl_blas.h>
#include "../../include/timing.h"

int vector_max_size = 10000;
int vector_stride = 100;
int trials = 10;

double* default_vector;
double** default_matrix;

double** vector_results;
double** matrix_results;

void save_res_to_csv(double** results, size_t results_c, int stride, char* file_name){
  FILE* csv = fopen(file_name, "w");
  if(csv == NULL) perror("Error creating file!");

  // header of the .csv
  fprintf(csv, "size\n" );
  for (int t = 0; t < trials; t++) {
    fprintf(csv, ",trial_%d", t );
  }
  fprintf(csv, "\n");
  // measurement results
  for(int i = 0; i < results_c; i++){
    fprintf(csv, "%d",i * stride);
    for (int j = 0; j < trials; j++) {
      fprintf(csv, ",%lf",results[i][j] );
    }
    fprintf(csv, "\n");
  }
  fclose(csv);

}

void init_vector(int max_size, int stride){
  default_vector = malloc(max_size * stride * sizeof(double));
  vector_results = malloc(max_size * sizeof(double*));
  for (size_t i = 0; i < max_size; i++) {
    default_vector[i] = i;
    vector_results[i] = malloc(trials * sizeof(double));
  }
}

void init_matrix(int max_size, int stride){
  default_matrix = malloc(max_size * stride * sizeof(double*));
  matrix_results = malloc(max_size * sizeof(double*));
  for (size_t i = 0; i < max_size; i++) {
    default_matrix[i] = malloc(max_size * sizeof(double));
    for (size_t j = 0; j < max_size; j++) {
      default_matrix[i][j] = max_size * i + j;
    }
    matrix_results[i] = malloc(trials * sizeof(double));
  }
}

void cleanup_vector(int max_size, int stride){
  free(default_vector);
  for (size_t i = 0; i < max_size; i++) {
    free(vector_results[i]);
  }
  free(vector_results);
}

void cleanup_matrix(int max_size, int stride){
  for (size_t i = 0; i < max_size; i++) {
    free(default_matrix[i]);
    free(matrix_results[i]);
  }
  free(default_matrix);
  free(matrix_results);
}

double vector_multi_test(int size){
  gsl_vector_view tmp = gsl_vector_view_array(default_vector, size);
  double res;
  double start = get_real_time();
  gsl_blas_ddot(&tmp.vector, &tmp.vector, &res);
  double end = get_real_time();
  return end - start;
}



void vector_analysis(int max_size, int stride, int trials){
  init_vector(max_size, stride);

  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      double res = vector_multi_test(i * stride);
      vector_results[i][j] = res;
    }
  }

  save_res_to_csv(vector_results, max_size, stride, "vector_multi.csv");
  cleanup_vector(max_size, stride);
}



int
main (void)
{
  vector_analysis(vector_max_size, vector_stride, trials);

  return 0;
}
