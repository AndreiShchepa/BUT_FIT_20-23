/**
 * Project: Packets sniffer
 *
 * @file sniffer.cpp
 *
 * @brief There is created the filter for the network interface.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#include <cstdio>
#include <csignal>
#include "disassembler.h"

void handler(int signum) {
    std::cerr << std::endl << "Ctrl+C key was pressed. Goodbye :)" << std::endl;
    exit(0);
}

int main (int argc, char* argv[]) {

    pcap_t *p;                     /* packet capture descriptor */
    char errbuf[PCAP_ERRBUF_SIZE]; /* buffer to hold error text */
    struct bpf_program prog;       /* compiled bpf filter program */
    u_int32_t net = 0;             /* network IP address */
    u_int32_t mask = 0;            /* network address mask */
    Rule rule;

    if (!parse_args(argc, argv, &rule)) {
        std::cerr << "Wrong argument was entered." << std::endl << "Run ./ipk-sniffer --help for printing usage message" << std::endl;
        return 1;
    }

    if (!strcmp(rule.getNic().c_str(), ""))
        print_name_nics();

    create_rule(&rule);

    if (!(p = pcap_open_live((rule.getNic()).c_str(), BUFSIZ, 1, 1000, errbuf))) {
        std::cerr << "Error opening interface " << rule.getNic() << "\n" << errbuf << std::endl;
        exit(2);
    }

    if (pcap_lookupnet((rule.getNic()).c_str(), &net, &mask, errbuf) < 0) {
        std::cerr << "Error looking up nenwork\n" << errbuf << std::endl;
        exit(3);
    }

    if (pcap_compile(p, &prog, (rule.getRule()).c_str(), 0, net) == -1) {
        std::cerr << "Unexpected err with compile of filter" << std::endl;
        exit(4);
    }

    if (pcap_setfilter(p, &prog) == -1) {
        std::cerr << "Unexpected err with setting of filter" << std::endl;
        exit(5);
    }

    signal(SIGINT, handler);
    printf("***********************************************************************************\n\n");
    if (pcap_loop(p, rule.getCount(), packets_handler, NULL) < 0) {
        std::cerr << "Unexpected err with pcap_loop" << std::endl;
        exit(6);
    }

    pcap_close(p);
    return 0;
}
