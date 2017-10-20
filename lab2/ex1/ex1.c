#include <stdio.h>
#include <gsl/gsl_blas.h>
#include "../../include/timing.h"

double* default_vector;
double* default_matrix;
double** results;

int max(int a, int b){
  if(a > b) return a;
  return b;
}

void save_res_to_csv(double** results, size_t results_c, int stride, int trials, char* file_name){
  FILE* csv = fopen(file_name, "w");
  if(csv == NULL) perror("Error creating file!");

  // header of the .csv
  fprintf(csv, "size,trial,result\n" );
  // measurement results
  for(int i = 0; i < results_c; i++){
    for (int j = 0; j < trials; j++) {
      fprintf(csv, "%d,%d,%lf\n", i * stride, j, results[i][j]);
    }
  }
  fclose(csv);

}

void init_vector(int max_size, int stride){
  int actual_size = max_size * stride;
  default_vector = (double*) malloc(actual_size * sizeof(double));
  for (int i = 0; i < actual_size; i++) {
    default_vector[i] = i;
  }
}

void init_matrix(int max_size, int stride){
  int actual_size = max_size * stride;
  default_matrix = (double*) malloc(actual_size* actual_size * sizeof(double));
  for (int i = 0; i < actual_size; i++) {
    for (int j = 0; j < actual_size; j++) {
      printf("%d,%d\n",i,j );
      default_matrix[actual_size * i + j] = actual_size * i + j;
    }
  }
}

void init_results(int max_size, int trials){
  results = (double**) malloc(max_size * sizeof(double*));
  for (int i = 0; i < max_size; i++) {
    results[i] = (double*) malloc(trials * sizeof(double));
  }
}

void cleanup_vector(int max_size, int stride){
  free(default_vector);
}

void cleanup_matrix(int max_size, int stride){

  int actual_size = max_size * stride;
  free(default_matrix);
}

void cleanup_results(int max_size){
  for (int i = 0; i < max_size; i++) {
    free(results[i]);
  }
  free(results);
}

double vector_multi_test(int size){
  gsl_vector_view tmp = gsl_vector_view_array(default_vector, size);
  double res;
  double start = get_real_time();
  gsl_blas_ddot(&tmp.vector, &tmp.vector, &res);
  double end = get_real_time();
  return end - start;
}

double matrix_multi_test(int size){
  gsl_vector_view tmp_v = gsl_vector_view_array(default_vector, size);
  gsl_matrix_view tmp_m = gsl_matrix_view_array(default_matrix, size, size);
  double res;
  double start = get_real_time();
  gsl_blas_dgemv(CblasNoTrans, 1, &tmp_m.matrix, &tmp_v.vector, 0, &tmp_v.vector);

  double end = get_real_time();
  return end - start;

}

void vector_analysis(int max_size, int stride, int trials){
  init_vector(max_size, stride);

  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      double res = vector_multi_test(i * stride);
      results[i][j] = res;
    }
  }

  save_res_to_csv(results, max_size, stride, trials, "vector_multi.csv");
  cleanup_vector(max_size, stride);
}

void matrix_analysis(int max_size, int stride, int trials){
  printf("v\n");
  init_vector(max_size, stride);
  printf("m\n");
  init_matrix(max_size, stride);
  printf("a\n" );
  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      printf("%d,%d\n",i,j );
      double res = matrix_multi_test(i * stride);
      results[i][j] = res;
    }
  }
  printf("b\n" );
  save_res_to_csv(results, max_size, stride, trials, "matrix_multi.csv");
  printf("cleanup_matrix\n");
  cleanup_matrix(max_size, stride);
  printf("cleanup_vector\n");

  cleanup_vector(max_size, stride);
  printf("c\n");

}


int
main (void)
{
  int vector_max_size = 10000;
  int vector_stride = 100;
  int matrix_max_size = 100; //50
  int matrix_stride = 100;
  int trials = 10;
  int results_size = max(vector_max_size, matrix_max_size);
  init_results(results_size, trials);
  printf("Analyzing vector*vector multiplication.\n" );
  vector_analysis(vector_max_size, vector_stride, trials);
  printf("Analyzing matrix*vector multiplication.\n" );
  matrix_analysis(matrix_max_size, matrix_stride, trials);
  cleanup_results(results_size);

  return 0;
}
