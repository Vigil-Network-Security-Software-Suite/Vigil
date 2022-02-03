#include "rule.h"
#include "../../../globals.h"
#include <stdio.h>
#include "../../engine/firewall/forward.h"

/*
  *-*-*-*- rule.c -*-*-*-*
  @purpose Apply rules located in "rules" in /globals.h
  void rulemgr(const struct rule_data * __rule_data);
    ==> Apply the rules
    TODO: Add support for ports
*/
void rulemgr(const struct rule_data * __rule_data){
  for(int i = 0; i < num_rules + 1;){
    struct rule * temp_rule = &rules[i++];
    if(((temp_rule->protocol == R_ALL) || ( __rule_data->__protocol == temp_rule->protocol)) &&
      ((temp_rule->port == -1) || (__rule_data->src_port == temp_rule->port || __rule_data->dest_port == temp_rule->port))){
      if(temp_rule->pkt_parser(__rule_data,temp_rule)){
        temp_rule->action(__rule_data,temp_rule,0);
        temp_rule->times_matched++;
      } else {
        if(NPSI_MODE == IPS_ACTIVE) forward_packet(__rule_data);
      }
    }
  }
}