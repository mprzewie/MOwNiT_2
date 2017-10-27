#ifndef TIMING_H
#define TIMING_H

#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct{
    struct timeval ru_utime; /* user time used */
    struct timeval ru_stime; /* system time used */
    long   ru_maxrss;        /* maximum resident set size */
    long   ru_ixrss;         /* integral shared memory size */
    long   ru_idrss;         /* integral unshared data size */
    long   ru_isrss;         /* integral unshared stack size */
    long   ru_minflt;        /* page reclaims */
    long   ru_majflt;        /* page faults */
    long   ru_nswap;         /* swaps */
    long   ru_inblock;       /* block input operations */
    long   ru_oublock;       /* block output operations */
    long   ru_msgsnd;        /* messages sent */
    long   ru_msgrcv;        /* messages received */
    long   ru_nsignals;      /* signals received */
    long   ru_nvcsw;         /* voluntary context switches */
    long   ru_nivcsw;        /* involuntary context switches */
}rusage;


double get_system_time(){
    struct timeval tim;
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim = ru.ru_stime;
    double t = (double)tim.tv_sec  + tim.tv_usec / 1000000.0;
    return t;
}

double get_user_time(){
    struct timeval tim;
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim = ru.ru_utime;
    double t = (double)tim.tv_sec + tim.tv_usec / 1000000.0;
    return t;
}

double get_real_time(){
    struct timeval tim;
    gettimeofday(&tim,NULL);
    double t = (double)tim.tv_sec + tim.tv_usec / 1000000.0;
    return t;
}



#endif
