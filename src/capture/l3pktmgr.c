#include <stdio.h>
#include <pcap.h>
#include <netinet/ip6.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include "../packets/ip_hdr.h"
#include "l3pktmgr.h"
#include "../colors.h"
#include "protocols/igmp_dsct.h"
#include "protocols.h"
#include "../packets/ip6hdr.h"
#include "../utils.h"
#include "tcpmgr.h"
#include "udpmgr.h"
#include "../filter/parsing/rule.h"
#include "../packets/icmp4.h"
#include "icmpdsct.h"
#include "../print_utils.h"
#include "../filter/parsing/rule.h"
#include "../filter/parsing/packet_parser.h"
#include "../../globals.h"

void ipv6pktmgr(const unsigned char * pkt,const  struct pcap_pkthdr * pkt_hdr){
  struct ip6hdr * ipv6_hdr = (struct ip6hdr *)(pkt + sizeof(struct ethhdr) );
  char dest_ip6[134];
  char src_ip6[134];
  strncpy(src_ip6,inet6_ntoa(ipv6_hdr->srcaddr),sizeof(src_ip6));
  strncpy(dest_ip6,inet6_ntoa(ipv6_hdr->dstaddr),sizeof(dest_ip6));
  // printf("IPv6 ");
  // printf("src=%p dst=%p",ipv6_hdr->srcaddr,ipv6_hdr->dstaddr);
  // printf("src=%s dst=%s\n", 
                      // src_ip6,dest_ip6);
  switch(ipv6_hdr->n_hdr){
    case 58:
    case 0:{
      ip6_icmp_decode(pkt,src_ip6,dest_ip6);
      break;
    }
    default:
      printf("IPv6 %s -> %s Protocol Number = %d\n",src_ip6,dest_ip6,ipv6_hdr->n_hdr);
      break;
  }
}

void ipv4pktmgr(const unsigned char * pkt, const struct pcap_pkthdr * pkt_hdr){
  struct ip_hdr * ip_header = (struct ip_hdr * )(pkt + ETH_HDR_SZ);
  struct sockaddr_in src, dest;
  struct rule_data rdata;
  char src_ip[32];
  char dest_ip[32];
  
  


  memset(&src,0,sizeof(src));
  memset(&dest,0,sizeof(dest));
  
  dest.sin_addr.s_addr = ip_header->daddr;
  src.sin_addr.s_addr = ip_header->saddr;
  rdata.src_socket = &src;
  rdata.dest_socket = &dest;
  memset(&rdata,0,sizeof(rdata));
  // memset(rdata->pkt,0,sizeof(rdata->pkt));
  // rdata->pkt = (unsigned char **)&pkt;
  rdata.pkt_len = pkt_hdr->len;
  
  int base_data_size = pkt_hdr->len 
                     - ETH_HDR_SZ
                     - (((struct iphdr *)(pkt + ETH_HDR_SZ))->ihl * 4);

  strncpy(dest_ip, inet_ntoa(dest.sin_addr),sizeof(dest_ip));
  strncpy(src_ip, inet_ntoa(src.sin_addr),sizeof(src_ip));
  // init both addresses;
  init_member((const char *)&dest_ip);
  init_member((const char *)&src_ip);
  
  if(is_blocked_ipv4((const char *)&dest_ip)){
    printf("%s Found blocked ipv4 address(dest): %s%s\n",BLOCKED_IP_FOUND,dest_ip,__END_COLOR_STREAM);
  }
  if(is_blocked_ipv4((const char *)&src_ip)){
    printf("%s Found blocked ipv4 address(src): %s%s\n",BLOCKED_IP_FOUND,src_ip,__END_COLOR_STREAM);
  }
  rdata.src_ip_addr = (char *)&src_ip;
  rdata.dest_ip_addr = (char *)&dest_ip;
      


  if(ip_header->flags == 0x0020 || ip_header->flags == 0x0102){
    printf("%sFragmented ",__FRAGMENTED);
    if(packet_print)
      printf("%s",__END_COLOR_STREAM);
    else 
      printf(" %s -> %s%s\n",src_ip,dest_ip,__END_COLOR_STREAM);
  }
  int data_size;
  switch(ip_header->protocol){
    case 1:{
      rdata.__protocol = R_ICMP;
      ip4_icmp_decode(pkt,rdata.src_ip_addr,rdata.dest_ip_addr);
      break;
    }
    case 2:
      // printf("IPv4 IGMP %s -> %s\n",src_ip,dest_ip);
      rdata.__protocol = R_ALL;
      ip4_igmp_decode(pkt,rdata.src_ip_addr,rdata.dest_ip_addr);
      
      break;
    
    case 6:
      rdata.__protocol = R_TCP; 
      ip4_tcp_decode(pkt,&rdata,pkt_hdr);
      // data_size = base_data_size - sizeof(struct tcphdr);
      // ascii_hexdump((pkt + data_size),pkt_hdr->len - data_size);
      break;
    case 17:
      rdata.__protocol = R_UDP;
      ip4_udp_decode(pkt,rdata.src_ip_addr,rdata.dest_ip_addr,pkt_hdr);
      // data_size = base_data_size - sizeof(struct udphdr);
      // ascii_hexdump((pkt + data_size),pkt_hdr->len - data_size);
      break;
    default:
      printf("IPv4 %s -> %s Protocol Number = %d\n",rdata.src_ip_addr,rdata.dest_ip_addr,ip_header->protocol);
      break;
  }
  // free(rdata.pkt);
  // free(rdata);
}
