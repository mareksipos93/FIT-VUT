/*
 * Program: ./proj2 P C PT RT
 * Author: Marek Sipos (xsipos03, VUT BIT 1)
 * Date: 19.04 2016
 *
 * Program arguments:
 *     P  - Amount of passenger processes (1+, P = k*C)
 *     C  - Capacity of roller coaster car (1+, P > C)
 *     PT - Max time (ms) between generating new passenger (0-5000)
 *     RT - Max time (ms) for full ride (0-5000)
 *
 *
 * Exit codes:
 *     0  - Everything OK (E_OK)
 *     1  - Input error (E_INPUT)
 *     2  - System call error (E_SYSCALL)
 *
*/

#include "proj2.h"

/* Global variables, because of functions for child processes */
programArgs args; // Structure to hold program arguments
FILE *file = NULL; // Log file
pid_t generatorID = -1; // To store generator PID
pid_t carID = -1; // To store car PID
pid_t myID; // Car and passengers store their PID to this variable
int passengerID = 1; // Passenger identifier (used to copy PID from generator to passenger)

/* Shared variables */
int *sh_actionID = NULL; // Action number
int *sh_onBoard = NULL; // How many passengers are on board
int *sh_finished = NULL; // How many processes are ready to finish

/* Semaphores */
sem_t *sem_mutex_write; // mutual exclusive for .out file + sh_action shared memory object
sem_t *sem_mutex_onBoard; // mutual exclusive for sh_onBoard shared memory object
sem_t *sem_mutex_finished; // mutual exclusive for sh_finished shared memory object
sem_t *sem_waitForLoad; // passengers must wait for car to allow boarding
sem_t *sem_waitForUnload; // passengers must wait for car to allow unboarding
sem_t *sem_waitUntilFull; // car must wait until all passengers are on the board
sem_t *sem_waitUntilEmpty; // car must wait until all passengers are off the board
sem_t *sem_waitToFinish; // processes must wait until everyone is ready to finish simultaneously

/* Shared memory descriptors (for shm_open) */
int desc_shm_action;
int desc_shm_onBoard;
int desc_shm_finished;

/* Main process uses this, children have separated functions */
int main(int argc, char *argv[])
{
    errno = 0;
    pid_t forkResult;
    srand(time(NULL)); // Every program execution = different random values

    // Set main process as group leader, so if it dies, it kills all child processes (auto SIGHUP)
    setpgid(0, 0);

    // Signal handlers
    signal(SIGUSR1, signalHandler); // Main process could obtain this signal from child
    signal(SIGUSR2, signalHandler); // Children could obtain this signal from parent
    signal(SIGTERM, signalHandler); // Soft exit
    signal(SIGINT, signalHandler); // CTRL+C

    // Get program arguments
    args = handleArguments(argc, argv);

    // Initialize shared semaphores
    sem_mutex_write = sem_open(NAME_SEM_WRITE, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_mutex_onBoard = sem_open(NAME_SEM_ONBOARD, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_mutex_finished = sem_open(NAME_SEM_FINISHED, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_waitForLoad = sem_open(NAME_SEM_WAITFORLOAD, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_waitForUnload = sem_open(NAME_SEM_WAITFORUNLOAD, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_waitUntilFull = sem_open(NAME_SEM_WAITUNTILFULL, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_waitUntilEmpty = sem_open(NAME_SEM_WAITUNTILEMPTY, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_waitToFinish = sem_open(NAME_SEM_WAITTOFINISH, O_CREAT, S_IRUSR | S_IWUSR, 0);

    if (sem_mutex_write == SEM_FAILED ||
        sem_mutex_onBoard == SEM_FAILED ||
        sem_mutex_finished == SEM_FAILED ||
        sem_waitForLoad == SEM_FAILED ||
        sem_waitForUnload == SEM_FAILED ||
        sem_waitUntilFull == SEM_FAILED ||
        sem_waitUntilEmpty == SEM_FAILED ||
        sem_waitToFinish == SEM_FAILED) {
            handleError(E_SYSCALL, errno, EA_ABORT, "Could not initialize semaphore");
    }

    // Initialize shared memory objects
    desc_shm_action = shm_open(NAME_SHM_ACTION, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    desc_shm_onBoard = shm_open(NAME_SHM_ONBOARD, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    desc_shm_finished = shm_open(NAME_SHM_FINISHED, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (desc_shm_action == -1 ||
        desc_shm_onBoard == -1 ||
        desc_shm_finished == -1) {
            handleError(E_SYSCALL, errno, EA_ABORT, "Could not initialize shared memory object");
    }

    // Set size of shared memory objects
    if (ftruncate(desc_shm_action, sizeof(int)) == -1 ||
        ftruncate(desc_shm_onBoard, sizeof(int)) == -1 ||
        ftruncate(desc_shm_finished, sizeof(int)) == -1) {
            handleError(E_SYSCALL, errno, EA_ABORT, "Could not set size of shared memory object");
    }

    // Map shared memory object
    sh_actionID = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, desc_shm_action, 0);
    sh_onBoard = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, desc_shm_onBoard, 0);
    sh_finished = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, desc_shm_finished, 0);
    if (sh_actionID == MAP_FAILED ||
        sh_onBoard == MAP_FAILED ||
        sh_finished == MAP_FAILED) {
            handleError(E_SYSCALL, errno, EA_ABORT, "Failed to map shared memory object");
    }

    // Start values
    (*sh_actionID) = 1;
    (*sh_onBoard) = 0;
    (*sh_finished) = 0;

    // Open log file
    file = fopen("proj2.out", "w");
    if (file == NULL) {
        handleError(E_SYSCALL, errno, EA_ABORT, "Could not open 'proj2.out' for writing");
    }

    // Car creation
    forkResult = fork();
    if (forkResult == -1) { // Error
        handleError(E_SYSCALL, errno, EA_ABORT, "System call 'fork()' failed");
    }
    else if (forkResult == 0) { // Child
        car();
        exit(0);
    }
    else { // Parent
        carID = forkResult;
    }

    // Generator creation
    forkResult = fork();
    if (forkResult == -1) { // Error
        handleError(E_SYSCALL, errno, EA_ABORT, "System call 'fork()' failed");
    }
    else if (forkResult == 0) { // Child
        generator();
        exit(0);
    }
    else { // Parent
        generatorID = forkResult;
        waitpid(generatorID, NULL, 0);
        errno = 0;
    }

    exitProgram(E_OK); // Here should be all forked processes exited already
}

/* Creation of passengers */
void generator() {

    pid_t forkResult;

    for (int i = 0; i < args.P; i++) // totally P passengers
    {
        if (msSleep(rand() % (args.PT + 1)) == -1) { // Sleep randomly from 0 to PT ms
            handleError(E_SYSCALL, errno, EA_SIGNAL, "System call 'usleep()' failed");
        }

        // Generate passengers
        forkResult = fork();
        if (forkResult == -1) { // Error
            handleError(E_SYSCALL, errno, EA_SIGNAL, "System call 'fork()' failed");
        }
        else if (forkResult == 0) { // Child
            passenger();
            exit(0);
        }
        else { // Parent
            passengerID++; // Increment AFTER child copies this
        }
    }

    // Wait for all generated passengers to exit
    while (waitpid(-1, NULL, 0)) {
        if (errno == ECHILD) {
            break;
        }
    }

    return;
}

/* There is always only one car */
void car() {

    myID = 1;

    sem_wait(sem_mutex_write);
    writeAction("%d\t: C %d\t: started\n", EA_SIGNAL, (*sh_actionID), myID);
    sem_post(sem_mutex_write);

    for (int i = 0; i < args.P/args.C; i++) {
        // Start LOAD operation (passengers can enter)
        sem_wait(sem_mutex_write);
        writeAction("%d\t: C %d\t: load\n", EA_SIGNAL, (*sh_actionID), myID);
        sem_post(sem_mutex_write);

        // Allow C passengers to go on the board
        for (int j = 0; j < args.C; j++) {
            sem_post(sem_waitForLoad);
        }

        // Wait for all passengers to board
        sem_wait(sem_waitUntilFull);

        // Go on the ride, passengers must wait until unload happens, nobody can enter
        sem_wait(sem_mutex_write);
        writeAction("%d\t: C %d\t: run\n", EA_SIGNAL, (*sh_actionID), myID);
        sem_post(sem_mutex_write);
        if (msSleep(rand() % (args.RT + 1)) == -1) { // Sleep randomly from 0 to RT ms
            handleError(E_SYSCALL, errno, 2, "System call 'usleep()' failed");
        }

        // Start UNLOAD operation (passengers can leave after that)
        sem_wait(sem_mutex_write);
        writeAction("%d\t: C %d\t: unload\n", EA_SIGNAL, (*sh_actionID), myID);
        sem_post(sem_mutex_write);

        // Allow passengers to unboard
        for (int j = 0; j < args.C; j++) {
            sem_post(sem_waitForUnload);
        }

        // Wait until everyone go out, then start load again
        sem_wait(sem_waitUntilEmpty);
    }

    // Everyone needs to finish simultaneously
    int finishAll = 0;
    sem_wait(sem_mutex_finished);
    (*sh_finished)++;
    if ((*sh_finished) == args.P+1)
        finishAll = 1;
    sem_post(sem_mutex_finished);

    if (finishAll == 1) {
        for (int i = 0; i < args.P; i++) {
            sem_post(sem_waitToFinish);
        }
    }
    else {
        sem_wait(sem_waitToFinish);
    }

    sem_wait(sem_mutex_write);
    writeAction("%d\t: C %d\t: finished\n", EA_SIGNAL, (*sh_actionID), myID);
    sem_post(sem_mutex_write);

    return;
}

/* Generated by generator one by one */
void passenger() {

    myID = passengerID;

    sem_wait(sem_mutex_write);
    writeAction("%d\t: P %d\t: started\n", EA_SIGNAL, (*sh_actionID), myID);
    sem_post(sem_mutex_write);

    // Wait for LOAD operation
    sem_wait(sem_waitForLoad);

    // Board on the car and inform car when full
    sem_wait(sem_mutex_onBoard);
    (*sh_onBoard)++;
    sem_wait(sem_mutex_write);
    writeAction("%d\t: P %d\t: board\n", EA_SIGNAL, (*sh_actionID), myID);
    if ((*sh_onBoard) < args.C)
        writeAction("%d\t: P %d\t: board order %d\n", EA_SIGNAL, (*sh_actionID), myID, (*sh_onBoard));
    else {
        writeAction("%d\t: P %d\t: board last\n", EA_SIGNAL, (*sh_actionID), myID);
        sem_post(sem_waitUntilFull);
    }
    sem_post(sem_mutex_write);
    sem_post(sem_mutex_onBoard);

    // Wait for UNLOAD operation
    sem_wait(sem_waitForUnload);

    // Unboard from the car and inform car when empty
    sem_wait(sem_mutex_onBoard);
    (*sh_onBoard)--;
    sem_wait(sem_mutex_write);
    writeAction("%d\t: P %d\t: unboard\n", EA_SIGNAL, (*sh_actionID), myID);
    if ((*sh_onBoard) > 0)
        writeAction("%d\t: P %d\t: unboard order %d\n", EA_SIGNAL, (*sh_actionID), myID, args.C-(*sh_onBoard));
    else {
        writeAction("%d\t: P %d\t: unboard last\n", EA_SIGNAL, (*sh_actionID), myID);
        sem_post(sem_waitUntilEmpty);
    }
    sem_post(sem_mutex_write);
    sem_post(sem_mutex_onBoard);

    // Everyone needs to finish simultaneously
    int finishAll = 0;
    sem_wait(sem_mutex_finished);
    (*sh_finished)++;
    if ((*sh_finished) == args.P+1)
        finishAll = 1;
    sem_post(sem_mutex_finished);

    if (finishAll == 1) {
        for (int i = 0; i < args.P; i++) {
            sem_post(sem_waitToFinish);
        }
    }
    else {
        sem_wait(sem_waitToFinish);
    }

    sem_wait(sem_mutex_write);
    writeAction("%d\t: P %d\t: finished\n", EA_SIGNAL, (*sh_actionID), myID);
    sem_post(sem_mutex_write);

    return;
}

/* This handles all user input and its conditions, so everything is okay (validation, etc...) */
programArgs handleArguments(int argc, char *argv[]) {

    // We need at least 4(+1 default) arguments (extra arguments are dropped)
    if (argc < 5) {
        handleError(E_INPUT, 0, EA_ABORT, "Missing program arguments");
    }

    // Check if numbers are valid and put them to structure
    programArgs args = {
        .P = strToLong(argv[1]),
        .C = strToLong(argv[2]),
        .PT = strToLong(argv[3]),
        .RT = strToLong(argv[4])
    };

    // Check values and ranges
    if (args.P <= 0)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'P' must be higher than 0 (was %d)", args.P);
    if (args.C <= 0)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'C' must be higher than 0 (was %d)", args.C);
    if (args.P <= args.C)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'P' must be higher than argument 'C' (P was %d, C was %d)", args.P, args.C);
    if (args.P % args.C != 0)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'P' must be multiple of argument 'C' (P was %d, C was %d)", args.P, args.C);
    if (args.PT < 0 || args.PT > 5000)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'PT' must be value between 0-5000 (was %d)", args.PT);
    if (args.RT < 0 || args.RT > 5000)
        handleError(E_INPUT, 0, EA_ABORT, "Argument 'RT' must be value between 0-5000 (was %d)", args.RT);

    return args; // Structure with valid arguments (see header file)
}

/* Unified interface to handle errors */
void handleError(int errorType, int errorNo, int action, const char *reason, ...) {

    // Variable arguments <stdarg.h>
    va_list args;
    va_start(args, reason);

    // Print error message
    fprintf(stderr, "FATAL ERROR: ");
    if (reason[strlen(reason)-1] != '\n') {
        vfprintf(stderr, reason, args);
        fprintf(stderr, "\n");
    }
    else {
        vfprintf(stderr, reason, args);
    }
    va_end(args);
    if (errorNo != 0)
        fprintf(stderr, "Additional information: %s\n", strerror(errorNo));

    // Abort program execution
    if (action == EA_ABORT) {
        exitProgram(errorType);
    }
    // Tell main process to abort program execution
    else if (action == EA_SIGNAL) {
        kill(getpgid(0), SIGUSR1);
    }

    return;
}

/* usleep() accepts max 0.999999 second, we need maximum of 5 seconds */
int msSleep(unsigned long ms) {

    int result = 0;

    for (unsigned long i = 0; i <= ms/1000; i++) {
        if (ms >= 999)
            result = usleep(999*1000);
        else
            result = usleep(ms*1000);
        if (ms >= 999) { ms -= 999; } else { ms = 0; } // Decrease remainder
        if (result == -1) // Error
            return -1;
    }
    return 0;
}

/* Auxiliary function, outputs to FILE, used for logs, increases action ID */
void writeAction(const char *text, int action, ...) {

    // Variable arguments <stdarg.h>
    va_list args;
    va_start(args, action);

    if (vfprintf(file, text, args) < 0) {
        handleError(E_SYSCALL, errno, action, "Failed to output text (fprintf)");
    }
    va_end(args);

    // Important, we need to really DO the output from buffer to stream
    if (fflush(file) == EOF) {  // This is better approach then setbuf(), because it keeps its speed
        handleError(E_SYSCALL, errno, action, "Failed to flush I/O buffer");
    }

    (*sh_actionID)++;

    return;
}

/* Auxiliary function, catches any wrong input and calls fatal error */
long strToLong(const char *str) {

    // To be sure
    if (str == NULL)
        handleError(E_INPUT, 0, EA_ABORT, "Invalid/Missing argument");

    long converted;
    char *endptr;

    // Catch invalid input
    errno = 0;
    converted = strtol(str, &endptr, 10);
    if (*endptr != '\0' || strcmp(endptr, str) == 0)
        handleError(E_INPUT, 0, EA_ABORT, "Invalid argument: '%s'", str);
    if (errno == ERANGE)
        handleError(E_INPUT, 0, EA_ABORT, "Argument outside range: '%s'", str);

    return converted; // Valid cast value
}

// SIGUSR1 and SIGUSR2 handler
void signalHandler(int sigNum) {
    if (sigNum == SIGUSR1 && getpgid(0) == getpid()) { // Only process group leader (main process) can do exitProgram()
        signal(SIGUSR1, SIG_IGN);
        exitProgram(E_SYSCALL);
    }
    else if ((sigNum == SIGTERM || sigNum == SIGINT) && getpgid(0) == getpid()) { // Quit called on main process
        signal(SIGTERM, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        exitProgram(E_SYSCALL);
    }
    else if (sigNum == SIGUSR2 && getpgid(0) != getpid()) { // Only children can be killed with this signal
        signal(SIGUSR2, SIG_IGN);
        exit(0);
    }
    else if ((sigNum == SIGTERM || sigNum == SIGINT) && getpgid(0) != getpid()) { // Quit called on any child process
        signal(SIGTERM, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        kill(getpgid(0), SIGUSR1);
        exit(0);
    }
    return;
}

void exitProgram(int errorType) {

    // Kill children (if any) - but first try to kill generator to avoid generation of new passengers
    if (generatorID != -1) {
        kill(generatorID, SIGUSR2);
    }
    killpg(getpid(), SIGUSR2);

    // Free resources
    if (file != NULL) {
        if (fclose(file) == EOF) {
            handleError(E_SYSCALL, errno, EA_INFORM, "Could not close 'proj2.out'");
        }
    }

    // free these only if needed = ignore errors there
    sem_close(sem_mutex_write);
    sem_close(sem_mutex_onBoard);
    sem_close(sem_mutex_finished);
    sem_close(sem_waitForLoad);
    sem_close(sem_waitForUnload);
    sem_close(sem_waitUntilFull);
    sem_close(sem_waitUntilEmpty);
    sem_close(sem_waitToFinish);
    sem_unlink(NAME_SEM_WRITE);
    sem_unlink(NAME_SEM_ONBOARD);
    sem_unlink(NAME_SEM_FINISHED);
    sem_unlink(NAME_SEM_WAITFORLOAD);
    sem_unlink(NAME_SEM_WAITFORUNLOAD);
    sem_unlink(NAME_SEM_WAITUNTILFULL);
    sem_unlink(NAME_SEM_WAITUNTILEMPTY);
    sem_unlink(NAME_SEM_WAITTOFINISH);

    close(desc_shm_action);
    close(desc_shm_onBoard);
    close(desc_shm_finished);
    shm_unlink(NAME_SHM_ACTION);
    shm_unlink(NAME_SHM_ONBOARD);
    shm_unlink(NAME_SHM_FINISHED);
    munmap(sh_actionID, sizeof(int));
    munmap(sh_onBoard, sizeof(int));
    munmap(sh_finished, sizeof(int));

    // Exit main process execution
    exit(errorType);
}
