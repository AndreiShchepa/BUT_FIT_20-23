/*
 * I have decided to use dynamic memory for working with strings. 
 * Therefore a big part of the code is related to working with
 * dynamic strings. These functions are write under dunction main().
 */
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define CMD_LOADCPU  "cat /proc/stat | grep \"\\<cpu\\>\""
#define CMD_HOSTNAME "cat /proc/sys/kernel/hostname"
#define CMD_CPUINFO  "cat /proc/cpuinfo | grep \"model name\" | head -n 1 | " \
                     "awk '{ for(i = 4; i<=NF; i++) printf $i\" \"; print \"\"}'"

#define SIZE_CPU_ARR 10
#define REALLOC_SIZE 20
#define ALLOC_SIZE   10

#define WRONG_REQUEST "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain;\r\n\r\nBad request!\n"
#define RIGHT_REQUEST "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n"
#define HELP_MSG      "Usage: %s port_number\nThe range of possible values " \
                      "of port_number is 0 to 65,535\n"

/* 
 * It is related to working with dynamic memory, 
 * it is not very interesting.
 * All macros are implemented for code readability.
 */ 
#define CHECK_ERR(cond, ret_val)         \
        if ((cond)) {                    \
            return (ret_val);            \
        }

#define ERR_FCLOSE(cond, file)           \
        if ((cond)) {                    \
            fclose((file));              \
            return false;                \
        }

#define ERR_FREE(cond, ptr)              \
        if ((cond)) {                    \
            free((ptr));                 \
            return false;                \
        }

#define ERR_FREE_FCLOSE(cond, file, ptr) \
        if ((cond)) {                    \
            free((ptr));                 \
            fclose((file));              \
            return false;                \
        }

//////////////////////////////////////////

// It is related to working with dynamic memory, 
// it is not very interesting.
typedef struct dynstr {
    char *str;
    int length;
    int alloc_size;
} dynstr_t;

bool alloc_str(dynstr_t *);
bool realloc_str(dynstr_t *);
bool fill_dynstr(dynstr_t *, FILE *);
//////////////////////////////////

// Possible requests 
typedef enum type_of_req {
    ERR = -1,
    HOST_NAME,
    CPU_INFO,
    LOAD_CPU
} type_of_req_t;

// Const values to help calculate a load of the CPU
typedef enum stat_cpu {
    USER = 0,
    NICE,
    SYSTEM,
    IDLE,
    IOWAIT,
    IRQ,
    SOFTIRQ,
    STEAL,
    GUEST,
    GUEST_NICE
} stat_cpu_t;

/*
 * @brief Additional function to calc_load_cpu().
 *        Get info from /proc/stat and separate string by space.
 *
 * @return On success true, otherwise false
 */
bool get_values(long long *arr) {
    FILE *in;
    bool ret;
    dynstr_t tmp;

    in = popen(CMD_LOADCPU, "r");
    CHECK_ERR(in == NULL, false);
    
    ret = fill_dynstr(&tmp, in);
    ERR_FCLOSE(ret == false, in);
    
    char* token;
    token = strtok(tmp.str, " ");
    token = strtok(NULL, " ");    

    int i = 0;
    while (token != NULL) {
        arr[i++] = strtol(token, (char **)NULL, 10);
        token = strtok(NULL, " ");
    }
    
    fclose(in);
    free(tmp.str);
    return true;
}

/*
 * @brief Calculate the load of CPU 
 *        by https://stackoverflow.com/questions/23367857/
 *        accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 *
 * @return On success true, otherwise false
 */
bool calc_load_cpu(char *loadcpu) {
    bool ret;
    long long PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total;
    long long prev_values[SIZE_CPU_ARR];
    long long curr_values[SIZE_CPU_ARR];
    long long totald, idled;
    float load;
    
    ret = get_values(prev_values);
    CHECK_ERR(ret == false, false);
    sleep(1);
    ret = get_values(curr_values);
    CHECK_ERR(ret == false, false);
    
    PrevIdle = prev_values[IDLE] + prev_values[IOWAIT];
    Idle = curr_values[IDLE] + curr_values[IOWAIT];

    PrevNonIdle = prev_values[USER] + prev_values[NICE] + prev_values[SYSTEM] + 
                  prev_values[IRQ] + prev_values[SOFTIRQ] + prev_values[STEAL];
    
    NonIdle = curr_values[USER] + curr_values[NICE] + curr_values[SYSTEM] + 
              curr_values[IRQ] + curr_values[SOFTIRQ] + curr_values[STEAL];
    
    PrevTotal = PrevIdle + PrevNonIdle;
    Total = Idle + NonIdle;
    
    totald = Total - PrevTotal;
    idled = Idle - PrevIdle;
    load = (totald - idled)/(float)totald * 100;
                
    snprintf(loadcpu, 32, "%f", load);
    strcat(loadcpu, "%\n\0");

    return true;
}

/*
 * @brief Call system commands and save output.
 * @return On success true, otherwise false
 */
bool get_info(dynstr_t *hostname, dynstr_t *cpuinfo) {
    FILE* in;
    bool ret;

    in = popen(CMD_HOSTNAME, "r");
    CHECK_ERR(in == NULL, false);
    ret = fill_dynstr(hostname, in);
    ERR_FCLOSE(ret == false, in);
    
    fclose(in);
    
    in = popen(CMD_CPUINFO, "r");
    ERR_FREE(in == NULL, hostname->str);
    ret = fill_dynstr(cpuinfo, in);
    ERR_FREE_FCLOSE(ret == false, in, hostname->str);

    fclose(in);
    return true;
}

/*
 * @brief Check type of request 
 */
void msg(char* request, type_of_req_t *type) {
    char* token;
    token = strtok(request, " ");
    
    if (strcmp("GET", token) != 0) {
        *type = ERR;
        return;
    }
    
    token = strtok(NULL, " ");    
    
    if (!strcmp("/hostname", token)) {
        *type = HOST_NAME; 
    }
    else if (!strcmp("/cpu-name", token)) {
        *type = CPU_INFO;
    }
    else if (!strcmp("/load", token)) {
        *type = LOAD_CPU;
    }
    else {
        *type = ERR;
    }
}

/*
 * @brief Check validity of the entered number of the port.
 * @return On success true, otherwise false
 */
bool check_port(int argc, char const *argv[], int *port) {
    char *trash;

    if (argc != 2) {
        fprintf(stderr, HELP_MSG, argv[0]);
        return false;
    }
    
    *port = strtol(argv[1], &trash, 10);
    if (trash[0] != '\0' || *port < 1) {
        fprintf(stderr, HELP_MSG, argv[0]);
        return false;
    }
    
    return true;
}

int main(int argc, char const *argv[]) {
    int server_fd, port;
    bool ret;
    struct sockaddr_in sa;
    
    ret = check_port(argc, argv, &port);
    CHECK_ERR(ret == false, -1);

    dynstr_t hostname;
    dynstr_t cpuinfo;
    char loadcpu[32];
        
    ret = get_info(&hostname, &cpuinfo);
    CHECK_ERR(ret == false, -1);
    
    // Create a mechanism that gives programs access to the network
    // AF_INET - IPv4
    // SOCK_STREAM - TCP connection
    // IPPROTO - TCP protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
        perror("ERROR: creating socket");
        exit(EXIT_FAILURE);
    }

    // AF_INET - IP
    // s_addr - any source ip address
    // sin_port - entered port by the user  
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(port);

    // bind the socket to the address and port number specified in addr
    if (bind(server_fd, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
        perror("ERROR: bind"); 
        exit(EXIT_FAILURE);    
    }

    // wait for an incoming connection
    if (listen(server_fd, 3) < 0) {
        perror("ERROR: listen"); 
        exit(EXIT_FAILURE); 
    }

    int new_socket, length;
    type_of_req_t t;
    int size = sizeof(sa);

    while(1) {
        
        // grab the first connection request on the queue of pending connections 
        // and create a new socket for that connection
        int new_socket = accept(server_fd, (struct sockaddr *) &sa, (socklen_t*) &size);
        if (new_socket < 0) {
            perror("ERROR: accept");
            exit(EXIT_FAILURE);
        }
        
        // recieve message from the client
        char buffer[2048] = {0};
        if (recv(new_socket, buffer, 1024, 0) < 0) {
            perror("ERROR: recieve");
            exit(EXIT_FAILURE);
        }
        
        msg(buffer, &t);
        char response[1024] = {0};
        
        switch(t) {
            case HOST_NAME:
                strcpy(response, RIGHT_REQUEST);
                strcat(response, hostname.str);
                length = strlen(RIGHT_REQUEST) + hostname.length;
                break;
            case CPU_INFO:
                strcpy(response, RIGHT_REQUEST);
                strcat(response, cpuinfo.str);
                length = strlen(RIGHT_REQUEST) + cpuinfo.length;
                break;
            case LOAD_CPU:
                strcpy(response, RIGHT_REQUEST);
                ret = calc_load_cpu(loadcpu);
                if (ret == false) {
                    goto internal_err;
                }
                strcat(response, loadcpu);
                length = strlen(RIGHT_REQUEST) + strlen(loadcpu);
                break;
            default:
                strcpy(response, WRONG_REQUEST);
                length = strlen(WRONG_REQUEST);
                break;
        }
        
        // send response to the client
        send(new_socket, response, length, 0); 
        close(new_socket);
    }
    
    return 0;

internal_err:
    free(hostname.str);
    free(cpuinfo.str);
    return -1;
}

bool alloc_str(dynstr_t *str) {
    str->str = malloc(ALLOC_SIZE);
    CHECK_ERR(str->str == NULL, false);
    
    str->length = 0;
    str->alloc_size = ALLOC_SIZE;
    return true;
}

bool realloc_str(dynstr_t *str) {
    char *tmp;
    tmp = realloc(str->str, str->alloc_size + REALLOC_SIZE);
    ERR_FREE(tmp == NULL, str->str);

    str->str = tmp;
    str->alloc_size += REALLOC_SIZE;
    return true;
}

bool fill_dynstr(dynstr_t *str, FILE *in) {
    int c, i = 0;
    bool ret;
 
    ret = alloc_str(str);
    CHECK_ERR(ret == false, false);

    while ((c = fgetc(in)) != EOF) {
        str->str[i++] = c;
        
        if (i == str->alloc_size) {
            ret = realloc_str(str);
            CHECK_ERR(ret == false, false);
        }
    }
    
    str->str[i] = '\0';
    str->length = i ;

    return true;
}
