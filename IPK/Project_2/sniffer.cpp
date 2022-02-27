#include <iostream>
#include <cstdio>
#include <pcap/pcap.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <getopt.h>

#define INC_ID(_argc_, i) \
        if ((_argc_) > (i)) { \
            (i)++; \
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

void packets_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    struct ether_header* eth;
    struct ip* ipv4;
    struct tcphdr* tcp;
    struct udphdr* udp;
    struct ipv6_header* ipv6;
    struct icmp *icm;

    eth = (struct ether_header *) packet;
    
    switch (ntohs(eth->ether_type)) {
        case ETHERTYPE_IP:
            ipv4 = (struct ip*)(packet + 14); // create a const 14
            std::cout << "IPV4\n";
            switch (ipv4->ip_p) {
                case 1:
                    std::cout << "ICMP\n";
                    break;
                case 6:
                    std::cout << "TCP\n";
                    break;
                case 17:
                    std::cout << "UDP\n";
                    break;
            }
            break;
        case ETHERTYPE_ARP:
            std::cout << "ARP\n";
            break;
        case ETHERTYPE_IPV6:
            std::cout << "IPV6\n";
            break;
    }

    printf("\n\n\n");

   // printf("%hhu\n", my_ip->ip_p);
}

bool check_count(char *str_num, int *number) {
    char *trash;

    *number = strtol(str_num, &trash, 10);
    if (trash[0] != '\0' || *number < 0) 
        return false;
    
    return true;
}

void print_name_nics() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevsp;
    if (pcap_findalldevs(&alldevsp, errbuf) == -1) {
        exit(-1);
    }
    std::cout << "Available interface:" << std::endl;
    while (alldevsp->next != NULL) {
        printf("%s\n", alldevsp->name);
        alldevsp = alldevsp->next;
    }
    exit(0);
}

bool parse_args(int argc, char* argv[], Rule* rule) {
    opterr = 1;
    const struct option longopts[] = 
    {
        {"tcp",       no_argument,       0,  't'},
        {"udp",       no_argument,       0,  'u'},
        {"icmp",      no_argument,       0,  'c'},
        {"arp",       no_argument,       0,  'a'},
        {"interface", required_argument, 0,  'i'},
        {NULL,        0,                 0, '\0'}
    };
    
    int c, tmp_num, id = 1;
    while ((c = getopt_long(argc, argv, ":i:p:n:tu", longopts, NULL)) != -1) {
        switch (c) {
            case 't':
                rule->setTcp(true);
                break;
            case 'u':
                rule->setUdp(true);
                break;
            case 'c':
                rule->setIcmp(true);
                break;
            case 'i':
                rule->setNic(optarg);
                INC_ID(argc, id);
                break;
            case 'a':
                rule->setArp(true);
                break;
            case 'p':
                rule->setPort(optarg);
                INC_ID(argc, id);
                break;
            case 'n':
                if (!check_count(optarg, &tmp_num))
                    return false;

                rule->setCount(tmp_num);
                INC_ID(argc, id);
                break;
            default:
                if (strcmp(argv[id], "-i") == 0 || strcmp(argv[id], "--interface") == 0)
                    print_name_nics();
                
                return false;
        }
        INC_ID(argc, id);
    }

    if (id != argc)
        return false;

    return true;
}

void tmpPrintRule(Rule* rule) {
    printf("Tcp = %d\n", (int)rule->getTcp());
    printf("Arp = %d\n", (int)rule->getArp());
    printf("Udp = %d\n", (int)rule->getUdp());
    printf("Icmp = %d\n", (int)rule->getIcmp());
    std::cout << "Nic = " << rule->getNic() << "\n";
    std::cout << "Port = " << rule->getPort() << "\n";
    std::cout << "Rule = " << rule->getRule() << "\n";
    printf("Count = %d\n", rule->getCount());
}

void create_rule(Rule* rule) {
    std::string final_rule = "";
    std::string protocols = "";

    if (rule->getPort() != "")
        final_rule = "port " + rule->getPort();

    if (rule->getUdp() || rule->getTcp() || rule->getArp() || rule->getIcmp()) {
        if (rule->getUdp() && rule->getTcp())
            final_rule += "(tcp or udp";
        else if (rule->getUdp())
            final_rule += "(udp";
        else if (rule->getTcp())
            final_rule += "(tcp";
        
        if (rule->getArp())
            protocols += protocols != "" ? " or arp" : "(arp";
        if (rule->getIcmp())
            protocols += protocols != "" ? " or icmp" : "(icmp";
    }
    
    protocols += protocols != "" ? ")" : "";
    final_rule += final_rule != "" ? (" and " + protocols) : protocols;
    rule->setRule(final_rule);
}

int main (int argc, char* argv[]) {
          
    pcap_t *p;                     /* packet capture descriptor */
    char errbuf[PCAP_ERRBUF_SIZE]; /* buffer to hold error text */
    struct bpf_program prog;       /* compiled bpf filter program */
    u_int32_t net = 0;             /* network IP address */
    u_int32_t mask = 0;            /* network address mask */
    Rule rule;
    
    if (!parse_args(argc, argv, &rule))
        return -1;

    create_rule(&rule);
    tmpPrintRule(&rule);

    if (!(p = pcap_open_live((rule.getNic()).c_str(), BUFSIZ, 1, 1000, errbuf))) {
        std::cout << "Error opening interface " << rule.getNic() << ": " << errbuf << "\n";
        exit(2);
    }

    if (pcap_lookupnet((rule.getNic()).c_str(), &net, &mask, errbuf) < 0) {
        std::cout << "Error looking up nenwork " << errbuf << "\n";
        exit(3);
    }
    
    if (pcap_compile(p, &prog, (rule.getRule()).c_str(), 0, net) == -1) {
        std::cout << "Unexpected err with compile of filter\n";
        exit(4);
    }

    if (pcap_setfilter(p, &prog) == -1) {
        std::cout << "Unexpected err with setting of filter\n";
        exit(5);
    }
         
    if (pcap_loop(p, rule.getCount(), packets_handler, NULL) < 0) {
        std::cout << "Unexpected err with pcap_loop\n";
        exit(7);
    }

    pcap_close(p);
    return 0;
}
