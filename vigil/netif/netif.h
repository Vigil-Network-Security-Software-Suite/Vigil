/*     
    This file is part of Vigil.

    Vigil is free software: you can redistribute it and/or modify it under the terms of the 
    GNU General Public License as published by the Free Software Foundation, either version 3 
    of the License, or (at your option) any later version.

    Vigil is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Vigil. 
    If not, see <https://www.gnu.org/licenses/>. 

*/
#ifndef NET_IF_H
#define NET_IF_H
#include <pthread.h>
#include <stdbool.h>
#include <linux/if.h>
#include <stdint.h>


typedef struct{
  char if_name[256];
  // pthread_t * thrd_id;
  unsigned long thrd_id;
  bool active;
  unsigned int a_family;
  char address[100];
  uint8_t mac_addr[6];
  int fd;
  int addr_set;
} v_netif;

extern v_netif * net_interfaces;
extern int iface_detected;
void start_interface_cap(const char *);
void start_interface_cap_ex();
void detect_interfaces();
void free_iface();
bool interface_operational(const char *);
int iface_exists(const char *);
void gather_iface_addrs();
void gather_iface_hw_addrs();
int num_f_in_dir(const char *);
void print_iface_summary();

int set_active_interface(const char *);

int start_global_cap();

#endif /* NET_IF_H */