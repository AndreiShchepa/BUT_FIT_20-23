/**
 * Project: Packets sniffer
 *
 * @file disassembler.cpp
 *
 * @brief Contain functions for packets processing,
          arguments parsing, printing data, converting
          time zones and others.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#include "disassembler.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <getopt.h>
#include <time.h>
#include <ifaddrs.h>
#include <libnet.h>

void help_msg() {
    printf("Usage: ./ipk-sniffer [-i name | --interface name] {-p port} \
{[--tcp|-t] [--udp|-u] [--arp] [--icmp] } {-n num}\n");
    printf("\t-i interface : the name of the interface to listen to\n");
    printf("\t-p port      : on this port will be packet filtering performed; \
the given port can occur in both the source and destination part\n");
    printf("\t-t --tcp     : display only TCP packets\n");
    printf("\t-u --udp     : display only UDP packets\n");
    printf("\t--arp        : display only ARP packets\n");
    printf("\t--icmp       : display only ICMPv4 a ICMPv6 packets\n");
    printf("\t-n           : number of packets to be displayed; by default n = 1\n");
}

void print_hex(u_char *data, int st, int end) {
    printf("0x%04x  ", st);
    for (int i = st; i < st + DATA; i++)
        printf(i > end ? "   " : "%02x ", data[i]);

    printf(" ");
}

void print_ascii(u_char *data, int st, int end) {
    for(int i = st; i < end; i++)
        printf("%c", isprint(data[i]) ? data[i] : 46); // 46 - '.' ASCII

    printf("\n");
}

void print_data(u_char *data, int length) {
    int end;
    for (int st = 0; st < length; st += DATA) {
        end = st + DATA;
        print_hex(data, st, end > length ? length : end);
        print_ascii(data, st, end > length ? length : end);
    }
}

void process_4th_layer(int next_hdr, const u_char* data) {
    struct tcphdr* tcp;
    struct udphdr* udp;
    struct icmphdr* icmp;

    switch (next_hdr) {
        case IPPROTO_ICMPV6: case IPPROTO_ICMP:
            icmp = (struct icmphdr*) (data);
            printf("icmp type: %d\n", icmp->type);
            printf("icmp code: %d\n", icmp->code);
            break;
        case IPPROTO_TCP:
            tcp = (struct tcphdr*) (data);
            PRINT_PORT("src port: ", tcp->th_sport);
            PRINT_PORT("dst port: ", tcp->th_dport);
            printf("\n");
            break;
        case IPPROTO_UDP:
            udp = (struct udphdr*) (data);
            PRINT_PORT("src port: ", udp->uh_sport);
            PRINT_PORT("dst port: ", udp->uh_dport);
            printf("\n");
            break;
    }
}

void print_time(struct timeval ts) {

    char tmbuf[90], zone[10];
    time_t timestamp = ts.tv_sec;
    struct tm* tm_loc = localtime(&timestamp);
    std::string zone_f;

    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%dT%H:%M:%S", tm_loc);
    strftime(zone, sizeof(zone), "%z", tm_loc);
    zone_f.insert(zone_f.end(), {zone[0], zone[1], zone[2], ':', zone[3], zone[4]});

    zone_f = !strcmp("+0000", zone) ? std::string(1, 'Z') : zone_f;
    printf("timestamp: %s.%06ld%s\n", tmbuf, ts.tv_usec, zone_f.c_str());
}

void packets_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    struct ether_header* eth;
    struct ip* ipv4;
    struct libnet_ipv6_hdr* ipv6;

    eth = (struct ether_header *) packet;
    print_time(header->ts);

    switch (ntohs(eth->ether_type)) {
        case ETHERTYPE_IP:
            PRINT_MAC_ADDR("src MAC: ", eth->ether_shost);
            PRINT_MAC_ADDR("dst MAC: ", eth->ether_dhost);

            PRINT_LENGTH(header->len);

            ipv4 = (struct ip*)(packet + ETHER_HDR_LEN); // 14

            PRINT_IP_ADDR("src IP: ", ipv4->ip_src);
            PRINT_IP_ADDR("dst IP: ", ipv4->ip_dst);

            process_4th_layer(ipv4->ip_p, packet + ETHER_HDR_LEN + (ipv4->ip_hl * 4));
            print_data((u_char *)(packet), header->len);
            break;
        case ETHERTYPE_ARP:
            PRINT_MAC_ADDR("src MAC: ", packet + 22);
            PRINT_MAC_ADDR("dst MAC: ", packet + 32);

            PRINT_LENGTH(header->len);

            printf("src IP: %d.%d.%d.%d\n", *(packet + 28), *(packet + 29), *(packet + 30), *(packet + 31));
            printf("dst IP: %d.%d.%d.%d\n", *(packet + 38), *(packet + 39), *(packet + 40), *(packet + 41));

            print_data((u_char *)(packet), header->len);
            break;
        case ETHERTYPE_IPV6:
            PRINT_MAC_ADDR("src MAC: ", eth->ether_shost);
            PRINT_MAC_ADDR("dst MAC: ", eth->ether_dhost);

            PRINT_LENGTH(header->len);

            ipv6 = (struct libnet_ipv6_hdr*)(packet + ETHER_HDR_LEN);

            char addr[INET6_ADDRSTRLEN];
            printf("src IP: %s\n", inet_ntop(AF_INET6, (struct in6_addr*)(&ipv6->ip_src), addr, INET6_ADDRSTRLEN));
            memset(addr, 0, INET6_ADDRSTRLEN);
            printf("dst IP: %s\n", inet_ntop(AF_INET6, (struct in6_addr*)(&ipv6->ip_dst), addr, INET6_ADDRSTRLEN));

            process_4th_layer(ipv6->ip_nh, packet + ETHER_HDR_LEN + 40);
            print_data((u_char *)(packet), header->len);
            break;
    }

    printf("\n***********************************************************************************\n\n");
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
    pcap_if_t *alldevsp, *dev;

    if (pcap_findalldevs(&alldevsp, errbuf) == -1) {
        std::cerr << "Error in pcap_findalldevs()\n" << errbuf << std::endl;
        exit(-1);
    }

    int j = 1;
    std::cout << "Available interfaces:" << std::endl;
    for (dev = alldevsp ; dev->next; dev = dev->next)
        std::cout << j++ << ". " << dev->name << std::endl;

    exit(1);
}

bool parse_args(int argc, char* argv[], Rule* rule) {

    if (argc == 2 && !strcmp("--help", argv[1])) {
        help_msg();
        exit(0);
    }

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
                CHECK_DUPLICATE(rule->getTcp(), false);
                rule->setTcp(true);
                break;
            case 'u':
                CHECK_DUPLICATE(rule->getUdp(), false);
                rule->setUdp(true);
                break;
            case 'c':
                CHECK_DUPLICATE(rule->getIcmp(), false);
                rule->setIcmp(true);
                break;
            case 'i':
                CHECK_DUPLICATE(rule->getNic(), "");
                rule->setNic(optarg);
                INC_ID(argc, id);
                break;
            case 'a':
                CHECK_DUPLICATE(rule->getArp(), false);
                rule->setArp(true);
                break;
            case 'p':
                CHECK_DUPLICATE(rule->getPort(), "");
                rule->setPort(optarg);
                INC_ID(argc, id);
                break;
            case 'n':
                CHECK_DUPLICATE(rule->getCount(), 1);
                if (!check_count(optarg, &tmp_num))
                    return false;

                rule->setCount(tmp_num);
                INC_ID(argc, id);
                break;
            default:
                if (strcmp(argv[id], "-i") == 0 || strcmp(argv[id], "--interface") == 0) {
                    print_name_nics();
                    exit(1);
                }

                return false;
        }
        INC_ID(argc, id);
    }

    if (id != argc)
        return false;

    return true;
}

void create_rule(Rule* rule) {
    std::string final = "";
    std::string tcp   = "tcp";
    std::string udp   = "udp";
    std::string icmp  = "icmp or icmp6";
    std::string arp   = "arp";
    std::string port  = "port " + rule->getPort();

    if (rule->getUdp())
        FILL_RULE(final, udp);
    if (rule->getTcp())
        FILL_RULE(final, tcp);
    if (rule->getArp())
        FILL_RULE(final, arp);
    if (rule->getIcmp())
        FILL_RULE(final, icmp);
    if (rule->getPort() != "")
        ADD_PORT_TO_RULE(final, port);
    else
        END_RULE(final);

    rule->setRule(final);
}
