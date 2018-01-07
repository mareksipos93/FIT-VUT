/************************************
*   IPK projekt 2 - chat klient     *
*   Autor: Marek Sipos (xsipos03)   *
*   Datum: 22.4.2017                *
*************************************/
#define _GNU_SOURCE

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG 0

#define PORT_NUMBER 21011
#define BUFFER_SIZE 10100 // BUFFER_SIZE > USERNAME_LEN + MSG_LEN
#define USERNAME_LEN 50
#define MSG_LEN 10000

struct sockaddr_in *address = NULL;
char *username = NULL;
int client_socket = 0;
pthread_t *t_main, *t_stdin, *t_smsg = NULL;
char *buffer, *buffer_stdin, *buffer_smsg = NULL;
char thread_error[100]; // chybova zprava predana z vlakna

/************ Vlastni funkce ************/

/* Velmi jednoducha debug funkce, ridi se makrem DEBUG nahore */
void debug_msg(const char *format, ...) {
    #if DEBUG==1
        va_list args;
        fprintf(stdout, "DEBUG - ");
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
    #endif
}

/* Vse uzavre a ukonci program */
void exit_program(int code) {

    debug_msg("exit_program(%d)", code);

    if (t_stdin != NULL)
        pthread_cancel(*t_stdin);
    if (t_smsg != NULL)
        pthread_cancel(*t_smsg);

    if (buffer != NULL) {
        snprintf(buffer, BUFFER_SIZE, "%s logged out\r\n", username);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    if (client_socket != 0)
        shutdown(client_socket, SHUT_RDWR);

    free(address);
    free(t_main);
    free(t_stdin);
    free(t_smsg);
    free(buffer);
    free(buffer_stdin);
    free(buffer_smsg);

    exit(code);

}

/* Zpusobi chybu a se zpravou ukonci program */
void make_error(int code, const char *format, ...) {
    if (format == NULL || strlen(format) == 0)
        fprintf(stderr, "Nastala chyba #%d", code);
    else {
        va_list args;
        fprintf(stderr, "Nastala chyba #%d - ", code);
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\n");
    } 
    exit_program(code);
}

/* Telo pro vlakno ktere cte ze stdin */
void *f_stdin(void *ptr) {
    debug_msg("vlakno stdin vytvoreno");
    buffer_stdin = malloc(sizeof(char) * BUFFER_SIZE);
    memset(buffer_stdin, 0, sizeof(char) * BUFFER_SIZE);
    int select_result;

    for(;;) {
        // SELECT STDIN
        do {
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(0, &fdset);
            struct timeval timeout;
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;

            select_result = select(1, &fdset, NULL, NULL, &timeout);
        } while ((select_result == -1 && errno == EINTR) || select_result == 0);

        if (select_result == -1) {
            strcpy(thread_error, "Cteni vstupu selhalo");
            pthread_kill(*t_main, SIGUSR1);
            return NULL;
        } else {
            // CTENI VSTUPU (bez konce radku, ignoruje prazdny vstup)
            char temp[MSG_LEN];
            fgets(temp, MSG_LEN-1, stdin);
            if (temp[strlen(temp)-1] == '\n')
                temp[strlen(temp)-1] = '\0';
            if (strlen(temp) == 0)
                continue;
            snprintf(buffer_stdin, BUFFER_SIZE-1, "%s: %s\r\n", username, temp);
            // SELECT SOCKET
            do {
                fd_set fdset;
                FD_ZERO(&fdset);
                FD_SET(client_socket, &fdset);
                struct timeval timeout;
                timeout.tv_sec = 10;
                timeout.tv_usec = 0;
                select_result = select(client_socket+1, NULL, &fdset, NULL, &timeout);
            } while (select_result == -1 && errno == EINTR);

            if (select_result == -1) {
                strcpy(thread_error, "Nepodarilo se spojit se serverem");
                pthread_kill(*t_main, SIGUSR1);
                return NULL;
            } else {
                // ODESLANI ZPRAVY
                if (send(client_socket, buffer_stdin, strlen(buffer_stdin), 0) == -1) {
                    strcpy(thread_error, "Nepodarilo se odeslat zpravu");
                    pthread_kill(*t_main, SIGUSR1);
                    return NULL;
                }
                memset(buffer_stdin, 0, sizeof(char) * BUFFER_SIZE);
            }
        }
    }
    pthread_kill(*t_main, SIGUSR1);
    return NULL;
}

/* Telo pro vlakno ktere cte ze serveru */
void *f_smsg(void *ptr) {
    debug_msg("vlakno smsg vytvoreno");
    buffer_smsg = malloc(sizeof(char) * BUFFER_SIZE);
    memset(buffer_smsg, 0, sizeof(char) * BUFFER_SIZE);
    int select_result, recv_result;
    int test = 0;
    for (;;) {
        // SELECT SOCKET
        do {
            test++;
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(client_socket, &fdset);
            struct timeval timeout;
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            debug_msg("SMSG select socket - #%d", test);
            select_result = select(client_socket+1, &fdset, NULL, NULL, &timeout);
        } while (select_result == -1 && errno == EINTR);

        if (select_result == -1) {
            strcpy(thread_error, "Nepodarilo se spojit se serverem");
            pthread_kill(*t_main, SIGUSR1);
            return NULL;
        } else {
            // CTENI ZE SERVERU
            debug_msg("SMSG recv socket");
            recv_result = recv(client_socket, buffer_smsg, BUFFER_SIZE-1, 0);
            if (recv_result == 0) {
                strcpy(thread_error, "Spojeni se serverem bylo preruseno");
                pthread_kill(*t_main, SIGUSR1);
                return NULL;
            } else if (recv_result == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) // Nic ke cteni
                    continue;
                strcpy(thread_error, "Nepodarilo se ziskat data ze serveru");
                pthread_kill(*t_main, SIGUSR1);
                return NULL;
            } else {
                // TISK ZPRAVY
                debug_msg("SMSG fprintf socket", test);
                fprintf(stdout, buffer_smsg);
                memset(buffer_smsg, 0, sizeof(char) * BUFFER_SIZE);
            }
        }
    }
    pthread_kill(*t_main, SIGUSR1);
    return NULL;
}

/* Funkce zpracovavajici signaly */
void sig_handler(int sig) {
    // Timto signalem uzivatel normalne chatovaciho klienta ukonci
    if (sig == SIGINT) {
        debug_msg("Prisel signal SIGINT");
        exit_program(0);
    }
    // Tento signal zasle nektere z vlaken, pokud dojde k nejake chybe
    if (sig == SIGUSR1) {
        debug_msg("Prisel signal SIGUSR1");
        if (errno == 0)
            make_error(110, "%s", thread_error);
        else
            make_error(errno, "%s: %s", thread_error, strerror(errno));
    }
}

/* Funkce pro preklad hostname do IP */
void resolve_hostname(char *hostname , char *ip)
{
    struct hostent *host;
    struct in_addr **addr_list;
         
    if ((host = gethostbyname(hostname)) == NULL)
        make_error(h_errno, "Nepodarilo se zjistit IP adresu");
 
    addr_list = (struct in_addr **) host->h_addr_list;
    
    if (addr_list[0] == NULL)
        strcpy(ip , hostname);
    else
        strcpy(ip , inet_ntoa(*addr_list[0]) );
}

int main(int argc, char **argv) {

    /************ Zpracovani parametru ************/
    
    int c, i;
    opterr = 0;
    char ip[100];
    while ((c = getopt (argc, argv, "i:u:")) != -1) {
        switch (c) {
            case 'i':
                if (address != NULL)
                    make_error(1, "Parametr -i byl zadan vicekrat");
                resolve_hostname(optarg, ip);
                address = malloc(sizeof(struct sockaddr_in));
                memset(address, 0, sizeof(struct sockaddr_in));
                address->sin_family = AF_INET;
                inet_aton(ip, &(address->sin_addr));
                address->sin_port = htons(PORT_NUMBER);
                break;
            case 'u':
                if (username != NULL)
                    make_error(1, "Parametr -u byl zadan vicekrat");
                if (strlen(optarg) > USERNAME_LEN)
                    make_error(1, "Maximalni delka uzivatelskeho jmena je %d znaku", USERNAME_LEN);
                for (i = 0; i < strlen(optarg); i++) {
                    if (!isprint(optarg[i]))
                        make_error(1, "Uzivatelske jmeno obsahuje neplatne znaky");
                }
                username = optarg;
                break;
            case '?':
                if (optopt == 'i' || optopt == 'u')
                    make_error(1, "Parametr -%c vyzaduje hodnotu", optopt);
                else if (isprint(optopt))
                    make_error(1, "Neznamy parametr -%c", optopt);
                else
                    make_error(1, "Chybny parametr \\x%x", optopt);
                break;
            default:
                abort();
        }
    }
    
    if (address == NULL)
        make_error(1, "Je nutne zadat IP adresu parametrem -i");
    else if (username == NULL)
        make_error(1, "Je nutne zadat uzivatelske jmeno parametrem -u");
    
    /************ Tvorba spojeni ************/
    
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        make_error(errno, "Nepodarilo se vytvorit soket: %s", strerror(errno));
    
    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
        make_error(errno, "Nepodarilo se nastavit soket na neblokujici: %s", strerror(errno));

    // pripojeni a cekani na uspech
    int select_result;
    debug_msg("Pripojuji se k \'%s\'", inet_ntoa(address->sin_addr));
    connect(client_socket, (struct sockaddr*)address, sizeof(*address));
    do {
        // doba pro pripojeni
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        // socket ke sledovani
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(client_socket, &fdset);

        select_result = select(client_socket+1, NULL, &fdset, NULL, &timeout);
    } while (select_result == -1 && errno == EINTR);

    // povedlo se pripojit
    if (select_result == 1) {
        int select_error;
        socklen_t len = sizeof(select_error);
        getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &select_error, &len);
        if (select_error != 0)
            make_error(select_error, "Nepodarilo se vytvorit spojeni: %s", strerror(select_error));
    // nepovedlo se pripojit v limitu
    } else if (select_result == 0)
        make_error(110, "Cas na spojeni vyprsel");
    // chyba
    else
        make_error(errno, "Nepodarilo se vytvorit spojeni: %s", strerror(errno));

    buffer = malloc(sizeof(char) * BUFFER_SIZE);
    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);

    /************ Tvorba vlaken ************/

    t_main = malloc(sizeof(pthread_t));
    *t_main = pthread_self();

    t_stdin = malloc(sizeof(pthread_t));
    if ((errno = pthread_create(t_stdin, NULL, f_stdin, NULL)) != 0)
        make_error(errno, "Nepodarilo se vytvorit vlakno pro stdin: %s", strerror(errno));

    t_smsg = malloc(sizeof(pthread_t));
    if ((errno = pthread_create(t_smsg, NULL, f_smsg, NULL)) != 0)
        make_error(errno, "Nepodarilo se vytvorit vlakno pro server: %s", strerror(errno));


    /************ Registrace signalu ************/

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        make_error(errno, "Nepodarilo se zaregistrovat signal SIGINT: %s", strerror(errno));
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        make_error(errno, "Nepodarilo se zaregistrovat signal SIGUSR1: %s", strerror(errno));

    /************ Dokonceni cinnosti ************/

    if (snprintf(buffer, BUFFER_SIZE, "%s logged in\r\n", username) < 0 || send(client_socket, buffer, strlen(buffer), 0) < 0)
        make_error(errno, "Nepodarilo se odeslat \'logged in\' zpravu", strerror(errno));

    pthread_join(*t_stdin, NULL);
    pthread_join(*t_smsg, NULL);

    exit_program(0); // To by nemelo nastat, je to pro jistotu
}
