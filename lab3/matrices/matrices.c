#include <stdio.h>
#include <gsl/gsl_blas.h>
#include "../../include/timing.h"

double* default_matrix;
double** results;

int max(int a, int b){
  if(a > b) return a;
  return b;
}

void save_res_to_csv(double** results, int max_size, int stride, int trials, char* file_name){
  FILE* csv = fopen(file_name, "w");
  if(csv == NULL) perror("Error creating file!");

  // header of the .csv
  fprintf(csv, "size,result\n" );
  // measurement results
  for(int i = 0; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      fprintf(csv, "%d,%lf\n", i * stride, results[i][j]);
    }
  }
  fclose(csv);

}

void init_matrix(int max_size, int stride){
  int actual_size = max_size * stride;
  default_matrix = (double*) malloc(actual_size* actual_size * sizeof(double));
  for (int i = 0; i < actual_size; i++) {
    for (int j = 0; j < actual_size; j++) {
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

void cleanup_matrix(int max_size, int stride){
  free(default_matrix);
}

void cleanup_results(int max_size){
  for (int i = 0; i < max_size; i++) {
    free(results[i]);
  }
  free(results);
}

double matrix_naive_test(int size){
  gsl_matrix* new_mat = gsl_matrix_alloc(size, size);
  gsl_matrix_view tmp_m = gsl_matrix_view_array(default_matrix, size, size);
  double res;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      gsl_matrix_set(new_mat, i, j, 0);
    }
  }
  double start = get_real_time();
  for(int j = 0; j< size; j++){
    for (int k = 0; k < size; k++) {
      for (int i = 0; i < size; i++) {
        double tmp_res = gsl_matrix_get(&tmp_m.matrix, i, k) * gsl_matrix_get(&tmp_m.matrix, k, j);
        gsl_matrix_set(new_mat, i, j, gsl_matrix_get(new_mat, i, j) + tmp_res);

      }
    }
  }
  double end = get_real_time();
  gsl_matrix_free(new_mat);
  return end - start;

}

double matrix_better_test(int size){
  gsl_matrix* new_mat = gsl_matrix_alloc(size, size);
  gsl_matrix_view tmp_m = gsl_matrix_view_array(default_matrix, size, size);
  double res;
  double start = get_real_time();
  for(int i = 0; i< size; i++){
    for (int j = 0; j < size; j++) {
      double tmp_res = 0;
      for (int k = 0; k < size; k++) {
        tmp_res += gsl_matrix_get(&tmp_m.matrix, i, k) * gsl_matrix_get(&tmp_m.matrix, k, j);
      }
      gsl_matrix_set(new_mat, i, j, tmp_res);
    }
  }
  double end = get_real_time();
  gsl_matrix_free(new_mat);
  return end - start;

}

double matrix_blas_test(int size){
  gsl_matrix_view tmp_m = gsl_matrix_view_array(default_matrix, size, size);
  double res;
  double start = get_real_time();
  gsl_blas_dsymm(CblasLeft, CblasUpper, 1, &tmp_m.matrix, &tmp_m.matrix, 0,&tmp_m.matrix);

  double end = get_real_time();
  return end - start;

}

void matrix_naive_analysis(int max_size, int stride, int trials){
  init_matrix(max_size, stride);
  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      results[i][j] = matrix_naive_test(i * stride);
    }
  }
  save_res_to_csv(results, max_size, stride, trials, "naive.csv");
  cleanup_matrix(max_size, stride);
}

void matrix_better_analysis(int max_size, int stride, int trials){
  init_matrix(max_size, stride);
  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      results[i][j] = matrix_better_test(i * stride);
    }
  }
  save_res_to_csv(results, max_size, stride, trials, "better.csv");
  cleanup_matrix(max_size, stride);
}

void matrix_blas_analysis(int max_size, int stride, int trials){
  init_matrix(max_size, stride);
  for(int i = 1; i < max_size; i++){
    for (int j = 0; j < trials; j++) {
      results[i][j] = matrix_blas_test(i * stride);
    }
  }
  save_res_to_csv(results, max_size, stride, trials, "blas.csv");
  cleanup_matrix(max_size, stride);
}


int
main (void)
{
  int max_size = 25;
  int stride = 20;
  int trials = 10;
  int results_size = max_size;
  init_results(results_size, trials);
  printf("Analyzing naive multiplication.\n" );
  matrix_naive_analysis(max_size, stride, trials);
  printf("Analyzing better multiplication.\n" );
  matrix_better_analysis(max_size, stride, trials);
  printf("Analyzing blas multiplication.\n" );
  matrix_blas_analysis(max_size, stride, trials);
  cleanup_results(results_size);

  return 0;
}
