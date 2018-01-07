/* ========================================================================== */
/*   Jmeno:         Sipos Marek (xsipos03)                                    */                                 */
/*   Soubor:        pgrep.c                                                   */
/*   PREDMET IPS - paralelni GREP                                             */
/* ========================================================================== */

#define DEBUG_USLEEP 0
#define DEBUG_MSGS 0

/* Cross platform */
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

/* Independent includes */
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <new>
#include <regex>
#include <thread>
#include <mutex>
#include <string>

using namespace std;

/* Error codes */
typedef enum ERR_CODE {
    E_OK,
    E_SYSTEM_FAULT,
    E_WRONG_ARGS
}ERR_CODE;

/* Function signatures */
void quit(ERR_CODE code);
void handleArgs();
void thread_f(int ID);
void createThreads();
void initMutexes();
void mySleep(long ms);

/* Global args */
int g_argc;
char **g_argv;

/* Global vars */
double minscore;        // Min score to print line
double *argscore;       // Score for passing specific regex test
string *argregex;       // String representation of regex args
thread **threads;       // Thread pointers
double score;           // Actual score (beware data race!)
string current_line;    // Current line to be processed
bool finished = false;  // If all lines has been processed

/* Mutexes */
int line_wait = 0;
mutex *mtx_crit_line_wait;
mutex *mtx_wait_for_line;

int main_wait = 0;
mutex *mtx_crit_main_wait;
mutex *mtx_wait_for_main;

int main(int argc, char **argv) {

    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - === STARTING ===\n");
    #endif

    // Make args global
    g_argc = argc;
    g_argv = argv;

    // Prepare memory
    argscore = new (nothrow) double[(argc - 2) / 2];
    argregex = new (nothrow) string[(argc - 2) / 2];
    threads = new (nothrow) thread*[(argc - 2) / 2];
    if (argscore == NULL || argregex == NULL || threads == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        quit(E_SYSTEM_FAULT);
    }

    // Execute
    handleArgs();
    initMutexes();

    // First lock (needed to work properly)
    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - LOCK wait_for_main\n");
    #endif
    (*mtx_wait_for_main).lock();
    #if DEBUG_USLEEP == 1
        mySleep(1);
    #endif
    // Read lines
    for (string line; getline(cin, line);) {
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - Reading a line\n");
        #endif
        // MAKE THREADS WAIT FOR LINE PREPARATION
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - LOCK wait_for_line\n");
        #endif
        (*mtx_wait_for_line).lock();
        // THREAD - DONT WAIT FOR MAIN, WAIT FOR LINE INSTEAD!
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - UNLOCK wait_for_main\n");
        #endif
        (*mtx_wait_for_main).unlock();
        createThreads(); // Threads will be made just once
        // WAIT UNTIL ALL THREADS ARE WAITING FOR LINE PREPARATION
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - Waiting until all threads are locked in wait_for_line\n");
        #endif
        while (line_wait < (g_argc - 2) / 2)
            continue;
        // PREPARE LINE (all threads are waiting)
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - Preparing line\n");
        #endif
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        score = 0;
        current_line = line;
        // MAKE THREADS WAIT FOR MAIN AFTER WORK IS DONE
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - LOCK wait_for_main\n");
        #endif
        (*mtx_wait_for_main).lock();
        // LINE PREPARED, THREADS CAN WORK
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - UNLOCK wait_for_line\n");
        #endif
        (*mtx_wait_for_line).unlock();
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        // WAIT UNTIL ALL THREADS ARE WAITING FOR MAIN
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - Waiting until all threads are locked in wait_for_main\n");
        #endif
        while (main_wait < (g_argc - 2) / 2)
            continue;
        // PRINT LINE IF IT HAS ENOUGH SCORE
        if (score >= minscore) {
            #if DEBUG_MSGS == 1
                printf("[DEBUG] MAIN - Score is enough! Printing line!\n");
            #endif
            cout << line << "\n";
        }
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        // RESET COUNTERS
        line_wait = 0;
        main_wait = 0;
    }
    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - finished = true\n");
    #endif
    finished = true;
    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - UNLOCK wait_for_main\n");
    #endif
    (*mtx_wait_for_main).unlock();
    #if DEBUG_USLEEP == 1
        mySleep(1);
    #endif
    quit(E_OK);
}

/* Main thread function */
void thread_f(int ID) {
    #if DEBUG_MSGS == 1
        printf("[DEBUG] T_%d - === STARTING ===\n", ID);
    #endif
    while (finished == false) {
        // TELL MAIN THAT THREAD IS WAITING FOR LINE
        (*mtx_crit_line_wait).lock();
        line_wait++;
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        #if DEBUG_MSGS == 1
            printf("[DEBUG] T_%d - Being locked in wait_for_line (%d / %d)\n", ID, line_wait, (g_argc - 2) / 2);
        #endif
        (*mtx_crit_line_wait).unlock();
        // WAIT FOR LINE
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        (*mtx_wait_for_line).lock();
        (*mtx_wait_for_line).unlock();
        // DO WORK
        #if DEBUG_MSGS == 1
            printf("[DEBUG] T_%d - Doing regex work\n", ID);
        #endif
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        if (regex_match(current_line, regex(argregex[ID]))) {
            #if DEBUG_MSGS == 1
                printf("[DEBUG] T_%d - Regex matched pattern!\n", ID);
            #endif
            score += argscore[ID];
        }
        // TELL MAIN THAT THREAD IS WAITING FOR MAIN
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        (*mtx_crit_main_wait).lock();
        main_wait++;
        #if DEBUG_MSGS == 1
            printf("[DEBUG] T_%d - Being locked in wait_for_main (%d / %d)\n", ID, main_wait, (g_argc - 2) / 2);
        #endif
        (*mtx_crit_main_wait).unlock();
        // WAIT FOR MAIN
        #if DEBUG_USLEEP == 1
            mySleep(1);
        #endif
        (*mtx_wait_for_main).lock();
        (*mtx_wait_for_main).unlock();
    }
    #if DEBUG_USLEEP == 1
        mySleep(1);
    #endif
    #if DEBUG_MSGS == 1
        printf("[DEBUG] T_%d - === ENDING ===\n", ID);
    #endif
}

/* Validate command line arguments */
void handleArgs() {
    // At least 3 args
    if (g_argc < 4) {
        fprintf(stderr, "ERROR: Not enough arguments\n");
        quit(E_WRONG_ARGS);
    }
    // Amount of args must be odd number
    if (g_argc % 2 == 1) {
        fprintf(stderr, "ERROR: Wrong amount of arguments\n");
        quit(E_WRONG_ARGS);
    }
    // Handle args
    for (int i = 1; i < g_argc; i++) {
        // First argument (minscore) = min score to output line
        if (i == 1) {
            errno = 0;
            char *temp;
            minscore = strtod(g_argv[i], &temp);
            if (errno != 0 || *temp != '\0') {
                fprintf(stderr, "ERROR: First argument is not a valid number!\n");
                quit(E_WRONG_ARGS);
            }
        }
        // Odd arguments (argscore) = score for passing regex test
        else if (i % 2 == 1) {
            errno = 0;
            char *temp;
            argscore[(i - 3) / 2] = strtod(g_argv[i], &temp);
            if (errno != 0 || *temp != '\0') {
                fprintf(stderr, "ERROR: Argument %d is not a valid number!\n", i);
                quit(E_WRONG_ARGS);
            }
        }
        // Even arguments (argregex) = string representation of regex args
        else {
            argregex[(i - 2) / 2] = string(g_argv[i]);
        }
    }
}

/* Start threads */
void createThreads() {
    static bool created = false; // This will make sure threads are made just once
    if (!created) {
        #if DEBUG_MSGS == 1
            printf("[DEBUG] MAIN - Creating threads\n");
        #endif
        for (int i = 0; i < (g_argc - 2) / 2; i++) {
            threads[i] = new (nothrow) thread(thread_f, i);
            if (threads[i] == NULL) {
                fprintf(stderr, "ERROR: Thread creation failed!\n");
                quit(E_SYSTEM_FAULT);
            }
        }
        created = true;
    }
}

/* Prepare mutexes */
void initMutexes() {
    mtx_crit_line_wait = new (nothrow) mutex();
    mtx_wait_for_line = new (nothrow) mutex();
    mtx_crit_main_wait = new (nothrow) mutex();
    mtx_wait_for_main = new (nothrow) mutex();
    if (mtx_crit_line_wait == NULL || 
        mtx_wait_for_line == NULL  || 
        mtx_crit_main_wait == NULL || 
        mtx_wait_for_main == NULL) {
        fprintf(stderr, "ERROR: Mutex creation failed!\n");
        quit(E_SYSTEM_FAULT);
    }
}

/* Quit program execution */
void quit(ERR_CODE code) {

    /*
    // Free used memory
    if (argscore != NULL)
        delete[] argscore;
    if (argregex != NULL)
        delete[] argregex;
    for (int i = 0; i < (g_argc - 2) / 2; i++) {
        delete mutexes[i];
        delete threads[i];
    }
    if (mutexes != NULL)
        delete[] threads;
    if (threads != NULL)
        delete[] threads;
    if (mx_line_ready != NULL)
        delete mx_line_ready;
    if (mx_score != NULL)
        delete mx_score;
    if (mx_extra != NULL)
        delete mx_extra;
    */

    // Kill all threads and program on error
    if (code != E_OK && code != E_WRONG_ARGS) {
    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - === TERMINATING :O :O :O ===\n");
    #endif
        terminate(); 
    }

    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - Waiting till all threads finishes properly\n");
    #endif

    // Wait till all threads finishes properly
    if (code != E_WRONG_ARGS) {
        for (int i = 0; i < (g_argc - 2) / 2; i++) {
            if (threads != NULL) {
                (*threads[i]).join();
            }
        }
    }

    #if DEBUG_USLEEP == 1
        mySleep(1);
    #endif

    #if DEBUG_MSGS == 1
        printf("[DEBUG] MAIN - === ENDING ===\n");
    #endif

    exit(code);
}

/* Cross platform sleep command */
void mySleep(long ms)
{
    #ifdef LINUX
        usleep(sleepMs * 1000);
    #endif
    #ifdef WINDOWS
        Sleep(sleepMs);
    #endif
}
