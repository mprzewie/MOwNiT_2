#include <gsl/gsl_interp.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  printf("%s\n", "kierwa" );
  double x = 4096.0;
  int iterations = 1500;
  for(int i=0; i<iterations; i++){
    printf("%lf\n",x);
    gsl_ieee_printf_double(&x);
    printf("---------\n");
    x = x/2;
  }
  return 0;
}
