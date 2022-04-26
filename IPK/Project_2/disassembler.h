/**
 * Project: Packets sniffer
 *
 * @file disassembler.h
 *
 * @brief Ccontain the declaration of the class Rule for the rules
          of the filter and definition of macros.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#pragma once

#include <string.h>
#include <iostream>
#include <pcap/pcap.h>
#include <time.h>

#define DATA 16 // const for printing data

// macro for parsing command line arguments
#define INC_ID(_argc_, i) \
        (i) += (_argc_) > (i) ? 1 : 0

#define FILL_RULE(out, str) \
        (out) += (out) == "" ? "( " + (str) : " or " + (str)

#define ADD_PORT_TO_RULE(out, rule) \
        (out) += (out) == "" ? (rule) : " ) and " + (rule)

#define END_RULE(out) \
        (out) += (out) == "" ? (out) : " )"

#define PRINT_PORT(str, port) \
        printf(str "%i\n", ntohs((port)))

#define PRINT_LENGTH(pkt_len) \
        printf("frame length: %u bytes\n", (pkt_len))

#define PRINT_IP_ADDR(str, addr) \
        printf(str "%s\n", inet_ntoa((addr)))

#define PRINT_MAC_ADDR(str, addr) \
        printf(str \
        "%02x:%02x:%02x:%02x:%02x:%02x\n", \
        (addr)[0], (addr)[1], (addr)[2], \
        (addr)[3], (addr)[4], (addr)[5]);

#define CHECK_DUPLICATE(a, b) \
        if ((a) != (b)) { \
            help_msg(); \
            return false; \
        }

class Rule {
private:
    std::string port;
    bool tcp;
    bool udp;
    bool icmp;
    bool arp;
    int count;
    std::string nic;
    std::string rule;

public:
    Rule()
    {
        this->port  = "";
        this->tcp   = false;
        this->udp   = false;
        this->icmp  = false;
        this->arp   = false;
        this->count = 1;
        this->nic   = "";
        this->rule  = "";
    }

    std::string getPort() { return this->port;  }
    std::string getRule() { return this->rule;  }
    std::string getNic()  { return this->nic;   }
    int  getCount()       { return this->count; }
    bool getTcp()         { return this->tcp;   }
    bool getUdp()         { return this->udp;   }
    bool getArp()         { return this->arp;   }
    bool getIcmp()        { return this->icmp;  }

    void setPort(char* port)       { this->port = port;   }
    void setRule(std::string rule) { this->rule = rule;   }
    void setCount(int count)       { this->count = count; }
    void setTcp(bool val)          { this->tcp = val;     }
    void setUdp(bool val)          { this->udp = val;     }
    void setArp(bool val)          { this->arp = val;     }
    void setIcmp(bool val)         { this->icmp = val;    }
    void setNic(char* name)        { this->nic = name;    }
};

/**
 * @brief   The function help_msg.
 *
 * @details This function prints a help message with usage of the program.
 */
void help_msg();

/**
 * @brief   The function print_hex.
 *
 * @details This function prints 16 bytes
 *          of the packet in hex format.
 *
 * @param  data  Contents of the packet.
 * @param  st    Start position to print.
 * @param  end   ENd position to print.
 */
void print_hex(u_char *data, int st, int end);

/**
 * @brief   The function print_ascii.
 *
 * @details This function prints 16 bytes
 *          of the packet in ascii format.
 *
 * @param  data  Contents of the packet.
 * @param  st    Start position to print.
 * @param  end   ENd position to print.
 */
void print_ascii(u_char *data, int st, int end);

/**
 * @brief   The function print_data.
 *
 * @details This function prints one row of the packet contents.
 *
 * @param  data    Contents of the packet.
 * @param  length  Length of the packet.
 */
void print_data(u_char *data, int length);

/**
 * @brief   The function process_4th_layer.
 *
 * @details This function processes only the 4th layer of the captured packet.
 *
 * @param  next_hdr  Type of the header in the 4th layer.
 * @param  data      Pointer to the data in the 4th layer.
 */
void process_4th_layer(int next_hdr, const u_char *data);

/**
 * @brief   The function print_time.
 *
 * @details This function convert timestamp to the format by RFC 3339.
 *
 * @note    https://datatracker.ietf.org/doc/html/rfc3339 - RFC 3339
 *
 * @param  ts  Structure with packet timestamp.
 */
void print_time(struct timeval ts);

/**
 * @brief   The function packets_handler.
 *
 * @details This function processes a captured packets.
 *
 * @param  args    User's arguments
 * @param  header  Pointer to the timestamp
 *         and length of the packet.
 * @param  packet  Pointer to the first caplen
 *         bytes of data from the packet.
 */
void packets_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/**
 * @brief   The function check_count.
 *
 * @details This function prints all available interfaces.
 *
 * @param  str_num  Number of port in string format.
 * @param  number   Pointer to the variable for converted number of the port.
 *
 * @return       Bool result of the port validation.
 *
 * @retval true  Number of the port is correct.
 * @retval false The entered port is not valid.
 */
bool check_count(char *str_num, int *number);

/**
 * @brief   The function print_name_nics.
 *
 * @details This function prints all available interfaces.
 *
 * @return       Bool result of the arguments parsing.
 *
 * @retval true  Any error was not occured.
 * @retval false Input argument is wrong.
 */
void print_name_nics();

/**
 * @brief   The function parse_args.
 *
 * @details This function parses command line arguments and
 *          fill the class Rule.
 *
 * @note    If the user enters --help, help message is written to stdout
 *          and the program is finished. If the user does not enter a name
 *          of the interfaces, all available interfaces will be written
 *
 * @param  argc  Number of input arguments.
 * @param  argv  Array of input arguments.
 * @param  rule  Pointer to the class of the command line arguments.
 *
 * @return       Bool result of the arguments parsing.
 *
 * @retval true  Any error was not occured.
 * @retval false Input argument is wrong.
 */
bool parse_args(int argc, char **argv, Rule *rule);

/**
 * @brief   The function create_rule.
 *
 * @details This function create a rule for the filter using concatination
 *          single rules.
 *
 * @param  rule  Pointer to the class of the command line arguments to create a rule.
 */
void create_rule(Rule *rule);
