#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <cstring>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE_CPU_ARR 10
#define CMD_LOADCPU  "cat /proc/stat | grep \"\\<cpu\\>\""
#define CMD_HOSTNAME "cat /proc/sys/kernel/hostname"
#define CMD_CPUINFO  "cat /proc/cpuinfo | grep \"model name\" | head -n 1 | " \
                     "awk '{ for(i = 4; i<=NF; i++) printf $i\" \"; print \"\"}'"

#define WRONG_REQUEST "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain;\r\n\r\nBad request!\n"
#define RIGHT_REQUEST "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n"
#define HELP_MSG      "Usage: %s port_number\nThe range of possible values " \
                      "of port_number is 0 to 65,535\n"

#define CHECK_ERR(cond, ret_val)         \
        if ((cond)) {                    \
            return (ret_val);            \
        }

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

void fill_str(std::string &str, FILE *in) {
    int c;
    while ((c = fgetc(in)) != EOF) {
        str += c;
    }
}

// Function for tokenizing string with info about cpu load
void tokenize(std::string const &str, const char* delim,
            std::vector<std::string> &out)
{
    char *token = strtok(const_cast<char*>(str.c_str()), delim);
    while (token != nullptr)
    {
        out.push_back(std::string(token));
        token = strtok(nullptr, delim);
    }
}

/*
 * @brief Additional function to calc_load_cpu().
 *        Get info from /proc/stat and separate string by space.
 *
 * @return On success true, otherwise false
 */
bool get_values(long long *arr) {
    FILE *in;
    std::string tmp;
    std::vector<std::string> out;

    in = popen(CMD_LOADCPU, "r");
    CHECK_ERR(in == nullptr, false);

    fill_str(tmp, in);
    tokenize(tmp, " ", out);

    int i = 0;
    for (std::string &s: out) {
        arr[i++] = strtol(s.c_str(), (char **)NULL, 10);
    }

    fclose(in);
    return true;
}

/*
 * @brief Calculate the load of CPU
 *        by https://stackoverflow.com/questions/23367857/
 *        accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 *
 * @return On success true, otherwise false
 */
bool calc_load_cpu(std::string &loadcpu) {
    long long PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total;
    long long prev_values[SIZE_CPU_ARR];
    long long curr_values[SIZE_CPU_ARR];
    long long totald, idled;
    float load;
    bool ret;

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

    loadcpu = std::to_string(load) + "%\n\0";
    return true;
}

/*
 * @brief Call system commands and save output.
 * @return On success true, otherwise false
 */
bool get_info(std::string &hostname, std::string &cpuinfo) {
    FILE* in;

    in = popen(CMD_HOSTNAME, "r");
    CHECK_ERR(in == nullptr, false);
    fill_str(hostname, in);
    fclose(in);

    in = popen(CMD_CPUINFO, "r");
    CHECK_ERR(in == nullptr, false);
    fill_str(cpuinfo, in);
    fclose(in);

    return true;
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

// Check type of request
void msg(char* request, type_of_req_t *type) {
    if (!strncmp("GET /hostname ", request, strlen("GET /hostname ")))
        *type = HOST_NAME;
    else if (!strncmp("GET /cpu-name ", request, strlen("GET /cpu-name ")))
        *type = CPU_INFO;
    else if (!strncmp("GET /load ", request, strlen("GET /load ")))
        *type = LOAD_CPU;
    else
        *type = ERR;
}

int main(int argc, char const *argv[]) {
    int server_fd, port;
    bool ret;
    struct sockaddr_in sa;

    ret = check_port(argc, argv, &port);
    CHECK_ERR(ret == false, -1);

    std::string hostname, cpuinfo, loadcpu;

    ret = get_info(hostname, cpuinfo);
    CHECK_ERR(ret == false, -1);

    // Create a mechanism that gives programs access to the network
    // AF_INET - IPv4
    // SOCK_STREAM - TCP connection
    // IPPROTO - TCP protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
        perror("ERROR: creating socket");
        exit(EXIT_FAILURE);
    }

    const int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("ERROR: set socket options");
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

        std::vector<char> buf(5000);
        if (recv(new_socket, buf.data(), buf.size(), 0) < 0) {
            perror("ERROR: recieve");
            exit(EXIT_FAILURE);
        }

        msg(buf.data(), &t);
        std::string resp;

        switch(t) {
            case HOST_NAME:
                resp = RIGHT_REQUEST + hostname;
                break;
            case CPU_INFO:
                resp = RIGHT_REQUEST + cpuinfo;
                break;
            case LOAD_CPU:
                ret = calc_load_cpu(loadcpu);
                CHECK_ERR(ret == false, -1);
                resp = RIGHT_REQUEST + loadcpu;
                break;
            default:
                resp = WRONG_REQUEST;
                break;
        }

        // send response to the client
        send(new_socket, resp.c_str(), resp.length(), 0);
        close(new_socket);
    }

    return 0;
}
