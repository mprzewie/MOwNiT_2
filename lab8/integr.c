#include <stdio.h>
#include <math.h>
#include <gsl/gsl_integration.h>


double square(double x, void* params){
  return x*x;
}

double square_int(double x){
  return (x*x*x)/3;
}


double root(double x, void * params){
  return sqrt(x);
}

double root_int(double x){
  return (2.0/3) * sqrt(x*x*x);
}

double osc(double x, void* params){
  double a = *(double *) params;
  return a * cos(a* x);
}

double osc_int(double x, double a){
  return sin(a* x);
}

double rev(double x, void* params){
  return 1.0/ (x*x);
}

double rev_int(double x){
  return (1.0) /x;
}

int
main (void)
{
  gsl_integration_workspace * w
    = gsl_integration_workspace_alloc (1000);

  double result;
  double error;
  gsl_function F;
  printf("method,funct,result,expected,est_error,intervals\n" );

  F.function = &square;
  // F.params = &alpha;
  size_t neval = 5;
  double desired_error;

  //////////////////////////////
  // QNG
  desired_error = 1.0;
  for(int i=0; i<6; i++){
    F.function = &square;

    gsl_integration_qng(&F, 0, 1, 0, desired_error, &result, &error, &neval);
    printf("qng,square,%.18f,%.18f,%.18f,%zd\n",result,square_int(1),error,neval );
    F.function = root;
    gsl_integration_qng(&F, 0.001, 1, 0, desired_error, &result, &error, &neval);
    printf("qng,root,%.18f,%.18f,%.18f,%zd\n",result,root_int(1),error,neval );
    desired_error = desired_error / 10;
  }

  /////////////////////

  ////////////////////////
  //QAG

  desired_error = 1.0;

  neval = 15;
  for(int i=0; i<6; i++){
    F.function = &square;
    gsl_integration_qag(&F, 0, 1, 0, desired_error, 1000, i, w, &result, &error);
    printf("qag,square,%.18f,%.18f,%.18f,%zd\n",result,square_int(1),error,w->size);
    F.function = root;
    gsl_integration_qag(&F, 0.001, 1, 0, desired_error, 1000, i, w, &result, &error);
    printf("qag,root,%.18f,%.18f,%.18f,%zd\n",result,root_int(1),error,w->size );
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }

  /////////////////////

  ////////////////////////
  //QAGS

  desired_error = 1.0;

  neval = 15;
  for(int i=0; i<6; i++){
    F.function = &square;
    gsl_integration_qags(&F, 0, 1, 0, desired_error, 1000, w, &result, &error);
    printf("qags,square,%.18f,%.18f,%.18f,%zd\n",result,square_int(1),error,w->size);
    F.function = root;
    gsl_integration_qags(&F, 0.001, 1, 0, desired_error, 1000, w, &result, &error);
    printf("qags,root,%.18f,%.18f,%.18f,%zd\n",result,root_int(1),error,w->size);
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }
  /////

  ////////////////////////
  //QAGP

  double pts[2];
  pts[0] = 0;
  pts[1] = 1;
  desired_error = 1.0;

  neval = 15;
  for(int i=0; i<6; i++){
    F.function = &square;
    gsl_integration_qagp(&F, pts, 2, 0, desired_error, 1000, w, &result, &error);
    printf("qags,square,%.18f,%.18f,%.18f,%zd\n",result,square_int(1),error,w->size);
    F.function = root;
    gsl_integration_qagp(&F,pts, 2, 0, desired_error, 1000, w, &result, &error);
    printf("qags,root,%.18f,%.18f,%.18f,%zd\n",result,root_int(1),error,w->size);
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }
  /////

  /////////////////
  // Oscillation
  //////////
  // QNG
  double stop;
  double alpha;
  alpha = 4.0;
  F.params = &alpha;
  F.function = &osc;
  desired_error = 1.0;
  double start=1.0;
  stop = (9.0 / (2.0 * M_PI));
  for(int i=0; i<6; i++){
    gsl_integration_qng(&F, start, stop, 0, desired_error, &result, &error, &neval);
    printf("qng,osc,%.18f,%.18f,%.18f,%zd\n",result,(osc_int(stop, alpha) - osc_int(start, alpha)),error,neval);
    desired_error = desired_error / 10;
  }

  // QAG
  alpha = 4.0;
  F.params = &alpha;
  F.function = &osc;
  desired_error = 1.0;
  stop = (9.0 / (2.0 * M_PI));
  neval=15;
  for(int i=0; i<6; i++){
    gsl_integration_qag(&F, start, stop, 0, desired_error, 1000, i, w, &result, &error);
    printf("qag,osc,%.18f,%.18f,%.18f,%zd\n",result,(osc_int(stop, alpha) - osc_int(start, alpha)),error,neval);
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }

  //QAWO
  gsl_integration_qawo_table * wf = gsl_integration_qawo_table_alloc(alpha, (stop - start), GSL_INTEG_SINE, 2);
  desired_error = 1.0;

  for(int i=0; i<6; i++){
    gsl_integration_qawo(&F, start, 0, desired_error, 1000, w, wf, &result, &error);
    printf("qawo,osc,%.18f,%.18f,%.18f,%zd\n",result,(osc_int(stop, alpha) - osc_int(start, alpha)),error, w->size);
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }
  gsl_integration_qawo_table_free(wf);



  ///QAGIU
  F.function = &rev;
  desired_error = 1.0;
  start = 2.5;
  for(int i=0; i<6; i++){
    gsl_integration_qagiu(&F, start, 0, desired_error, 1000, w, &result, &error);
    printf("qagiu,osc,%.18f,%.18f,%.18f,%zd\n",result,(rev_int(start)),error, w->size);
    desired_error = desired_error / 10;
    if(i==0) neval+=6;
    else neval +=10;
  }



  gsl_integration_workspace_free (w);
  return 0;
}
