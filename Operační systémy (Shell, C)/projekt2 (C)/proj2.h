#ifndef PROJ2_H_INCLUDED
#define PROJ2_H_INCLUDED

// Standard libs
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Process sync libs
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Error types
#define E_OK        0 // No errors
#define E_INPUT     1 // Invalid program arguments, no errorCode (errno) set
#define E_SYSCALL   2 // System call error with proper errorCode (errno) set

// Error handler actions
#define EA_INFORM   0 // Print error info only
#define EA_ABORT    1 // Print error info + Call exitProgram()
#define EA_SIGNAL   2 // Print error info + Send signal SIGUSR1 to main process to call exitProgram(2)

// String identifiers for named semaphores
#define NAME_SEM_WRITE              "/iwi_sem_write"
#define NAME_SEM_ONBOARD            "/iwi_sem_onboard"
#define NAME_SEM_FINISHED           "/iwi_sem_finished"
#define NAME_SEM_WAITFORLOAD        "/iwi_sem_waitforload"
#define NAME_SEM_WAITFORUNLOAD      "/iwi_sem_waitforunload"
#define NAME_SEM_WAITUNTILFULL      "/iwi_sem_waituntilfull"
#define NAME_SEM_WAITUNTILEMPTY     "/iwi_sem_waituntilempty"
#define NAME_SEM_WAITTOFINISH       "/iwi_sem_waittofinish"

// String identifiers for shared memory objects
#define NAME_SHM_ACTION      "/iwi_shm_action"
#define NAME_SHM_ONBOARD     "/iwi_shm_onboard"
#define NAME_SHM_FINISHED    "/iwi_shm_finished"

/// Struct to hold program arguments
typedef struct programArgs {
    long P, C, PT, RT;
} programArgs;

/// Function for Generator process
void generator();

/// Function for Car process
void car();

/// Function for Passenger process
void passenger();

/// Handle any error that occurs
void handleError(int errorType, int errorNo, int action, const char *reason, ...);

/// Handle SIGUSR1 and SIGUSR2
void signalHandler(int sigNum);

/// Exit program, only main program process calls this
void exitProgram(int errorType);

/// Validate and handle program arguments
programArgs handleArguments(int argc, char *argv[]);

/// Function to output to file and increase Action ID
void writeAction(const char *text, int action, ...);

/// Convert string to long
long strToLong(const char *str);

/// Sleep for amount of milliseconds
int msSleep(unsigned long ms);

#endif // PROJ2_H_INCLUDED
