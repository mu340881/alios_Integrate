#include <stdio.h>
#include "k_api.h"
#include "k_dbg_api.h"

#define BACK_TRACE_LIMIT 64


extern int  printf(const char *fmt, ...);


/* printf call stack for context, return lvl */
int backtraceContext(char *PC, char *LR, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    int  lvl       = 0;
   // char prtbuff[] = "backtrace : 0x         \n";

    if (print_func == NULL) {
        print_func = printf;
    }

    print_func("======= Call stack Begin =======\n");


    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    //char *PC;
    //int  *SP;
    int   lvl       = 0;
    //char  prtbuff[] = "backtrace : 0x        \n";

    if (print_func == NULL) {
        print_func = printf;
    }

    print_func("======= Call stack Begin =======\n");
    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack for task */
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...))
{
    //char    *PC;
    //char    *LR;
    //int     *SP;
   // ktask_t *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    /*task = debug_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }


    print_func("TaskName  : %s\n", taskname);
    */
    return 0;
}
