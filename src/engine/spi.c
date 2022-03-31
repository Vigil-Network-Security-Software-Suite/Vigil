#include <stdlib.h>
#include <stdio.h>
#include "../../globals.h"

int conversation_exists(struct rule_data * rdata){
  for(int i = 0; i < total_conversations + 1; ++i){
    struct spi_members * sm = (struct spi_members *)&spi_table[i];
    if(sm->conversation_active != false){
      if(rdata->dest_port == sm->cli_port && rdata->src_port == sm->serv_port){
        if(strcmp(rdata->src_ip_addr, sm->serv_addr.netaddr) == 0 && strcmp(rdata->dest_ip_addr,sm->cli_addr.netaddr) == 0){
          return i;
        }
      } else if(rdata->src_port == sm->cli_port && rdata->dest_port == sm->serv_port){
        if(strcmp(rdata->dest_ip_addr, sm->serv_addr.netaddr) == 0 && strcmp(rdata->src_ip_addr,sm->cli_addr.netaddr) == 0){
          return i;
        }
      }
    }
    
  }
  return -1;
}


void add_new_conversation(struct rule_data * rdata){
  if(use_spi){
    int loc = conversation_exists(rdata);
    struct spi_members * sm;

    if(loc != -1){
      sm = &spi_table[loc];
      sm->possible_retransmissions++;
      printf("SPI TABLE RETRANS: %d: %s:%d -> %s:%d\n",loc,sm->client_addr.netaddr,sm->cli_port,sm->server_addr.netaddr,sm->serv_port);
    } else {
      sm = &spi_table[++total_conversations];
      memset(sm,0,sizeof(sm));
      
    }
  }
}