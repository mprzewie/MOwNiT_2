#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

int i;
int max_lines=30;
double xi, yi, x[30], y[30];

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

void read_csv(char* filename, int max_lines){
  FILE * f = fopen(filename, "r");
  char line[1024];
  int l = 0;
  fgets(line, 1024, f);
  while(fgets(line, 1024, f)){
    char* tmp = strdup(line);
    double y_tmp = atof(getfield(tmp, 2));
    double x_tmp = atof(getfield(tmp, 1));
    if(l < max_lines){
      x[l] = x_tmp;
      y[l] = y_tmp;
    }
    l++;

    free(tmp);
  }
}


int
main (void)
{
  read_csv("secret_fun.csv", max_lines);

  // printf ("#m=0,S=17\n");
  //
  // for (i = 0; i < max_lines; i++)
  //   {
  //     // x[i] = i + 0.5 * sin (i);
  //     // y[i] = i + cos (i * i);
  //     printf ("%f %f \n", x[i], y[i]);
  //   }

  // printf ("#m=1,S=0\n");
  //
  {
    gsl_interp_accel *acc
      = gsl_interp_accel_alloc ();
    gsl_spline *spline
      = gsl_spline_alloc (gsl_interp_cspline, 30);

    gsl_spline_init (spline, x, y, 30);

    printf("x,y\n" );
    for (xi = x[0]; xi < x[29]; xi += 0.01)
      {
        printf("%g, ", xi);
        yi = gsl_spline_eval (spline, xi, acc);
        printf ("%g\n", yi);
      }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
  }
  return 0;
}
