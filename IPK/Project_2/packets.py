from scapy.all import *
import os
import sys

def create_pkt(dict_pkt, name):
    pkt = dict_pkt['layer2']/dict_pkt['layer3']/dict_pkt['layer4']/dict_pkt['message']
    wrpcap("pkt_"+name+".pcap", pkt)

def find_addrs():
    if len(sys.argv) != 2:
        print("Enter the name of the interface as the 2nd argument")
        exit(1)

    ip_addr = os.popen("ifconfig | grep "+sys.argv[1]+" -A 1 | grep inet | awk -F ' ' '{print $2}'").read().rstrip('\n')
    ip6_addr = os.popen("ifconfig | grep "+sys.argv[1]+" -A 2 | grep inet6 | awk -F ' ' '{print $2}'").read().rstrip('\n')
    mac_addr = os.popen("ifconfig | grep "+sys.argv[1]+" -A 3 | grep ether | awk -F ' ' '{print $2}'").read().rstrip('\n')

    if ip_addr == "" or ip6_addr == "" or mac_addr == "":
        print("One of the addresses is empty, please check name of the interface or enter address manually")
        print("|"+ip_addr+"| - ip addr")
        print("|"+ip6_addr+"| - ip6 addr")
        print("|"+mac_addr+"| - mac addr")
        exit(1)

    return ip_addr, ip6_addr, mac_addr

def fill_frames():
    ip_addr_interface, ip6_addr_interface, mac_addr_interface = find_addrs()

    dict_dict_pkts = {
        'tcp' : dict(),
        'udp' : dict(),
        'arp' : dict(),
        'icmp' : dict(),
        'icmpv6' : dict(),
        'example' : dict()
    }

    ########## Simple packet TCP ##########
    dict_dict_pkts['tcp']['layer2']  = Ether(dst=mac_addr_interface, src="00:11:22:33:44:55")
    dict_dict_pkts['tcp']['layer3']  = IP(dst=ip_addr_interface, src="190.190.13.1")
    dict_dict_pkts['tcp']['layer4']  = TCP(dport=12345, sport=12001)
    dict_dict_pkts['tcp']['message'] = ""
    ##############################

    ########## Simple packet UDP ##########
    dict_dict_pkts['udp']['layer2']  = Ether(dst=mac_addr_interface, src="00:11:22:33:44:55")
    dict_dict_pkts['udp']['layer3']  = IP(dst=ip_addr_interface, src="190.190.13.1")
    dict_dict_pkts['udp']['layer4']  = UDP(dport=7777, sport=52309)
    dict_dict_pkts['udp']['message'] = ""
    ##############################

    ########## Simple packet ARP ##########
    dict_dict_pkts['arp']['layer2']  = Ether(dst=mac_addr_interface)
    dict_dict_pkts['arp']['layer3']  = ARP(psrc="210.16.100.10", hwdst=mac_addr_interface, pdst=ip_addr_interface)
    dict_dict_pkts['arp']['layer4']  = ""
    dict_dict_pkts['arp']['message'] = ""
    ##############################

    ########## Simple packet ICMP ##########
    dict_dict_pkts['icmp']['layer2']  = Ether(dst=mac_addr_interface)
    dict_dict_pkts['icmp']['layer3']  = IP(dst=ip_addr_interface, proto="icmp")
    dict_dict_pkts['icmp']['layer4']  = ICMP(type="echo-request", code=0)
    dict_dict_pkts['icmp']['message'] = ""
    ##############################

    ########## Simple packet ICMPv6 ##########
    dict_dict_pkts['icmpv6']['layer2']  = Ether(dst=mac_addr_interface, src="00:11:22:33:44:55")
    dict_dict_pkts['icmpv6']['layer3']  = IPv6(dst=ip6_addr_interface)
    dict_dict_pkts['icmpv6']['layer4']  = ICMPv6EchoRequest(data="!___VUT***FIT___!")
    dict_dict_pkts['icmpv6']['message'] = ""
    ##############################

    ########## Simple packet TCP with text ##########
    dict_dict_pkts['example']['layer2']  = Ether(dst=mac_addr_interface, src="00:11:22:33:44:55")
    dict_dict_pkts['example']['layer3']  = IP(dst=ip_addr_interface, src="190.190.13.1")
    dict_dict_pkts['example']['layer4']  = TCP(sport=60001, dport=12345)
    dict_dict_pkts['example']['message'] = "()()(_-_-_!!!Andrei Shchapaniak, xshcha00!!!_-_-_)()()"
    ##############################

    return dict_dict_pkts

#### main ####
if len(sys.argv) > 1 and sys.argv[1] == "remove":
    os.system("rm -d *.pcap")
    exit(0)
    
dict = fill_frames()

for name, pkt in dict.items():
    create_pkt(pkt, name)
