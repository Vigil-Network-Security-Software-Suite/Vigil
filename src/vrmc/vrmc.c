#include "vrmc.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include "../../globals.h"
#include <netdb.h>
#include <sys/types.h>
#include "base64.h"
#include <unistd.h>
#include "../utils.h"

unsigned char * pubkey;
unsigned char * privkey;

void read_pub_key(){
  pubkey = (unsigned char *)malloc(1024);
  memset(&pubkey,0,sizeof(pubkey));
  char temp_pubkey[1024] = {0};

  
  FILE * f_pubkey = fopen("/etc/vigil/keys/pubkey.crt","r");
  size_t pos,len;
  char * line = NULL;

  while((pos = getline(&line,&len,f_pubkey)) != -1){
    if(strloc(line,'-') != -1) continue;
    line[strcspn(line,"\n")] = 0;
    strcat(&temp_pubkey,line);
  }
  pubkey = base64_decode(temp_pubkey,strlen(temp_pubkey));
  printf("%02x",pubkey);
  fclose(f_pubkey);

}

void read_private_key(){
  privkey = (unsigned char *)malloc(2048);
  memset(&privkey,0,sizeof(privkey));
  char temp_privkey[2048] = {0};
  FILE * f_privkey = fopen("/etc/vigil/keys/privkey.pem","r");
  char * line = NULL;
  size_t pos, len;
  while((pos = getline(&line,&len,f_privkey)) != -1){
    if(strloc(line,'-') != -1) continue;
    line[strcspn(line,"\n")] = 0;
    strcat(&temp_privkey,line);
  }
  privkey = base64_decode(temp_privkey,strlen(temp_privkey));
  fclose(f_privkey);
}


void start_vrmc_server(){
  read_pub_key();
  pthread_t vthrd;
  pthread_create(&vthrd,NULL,&__start_vrmc_server,NULL);
  
}

void __start_vrmc_server(){
  int fd, sock, input;

  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if((fd = socket(AF_INET,SOCK_STREAM, 0)) == 0){
    perror("vrmc: failed to create socket");
    exit(EXIT_FAILURE);
  }

  if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, &addrlen,sizeof(addrlen))){
    perror("vrmc.c: failed setsockopt");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(641);

  if(bind(fd, (struct sockaddr*)&addr,sizeof(addr)) < 0){
    perror("vrmc.c: failed socket binding");
    exit(EXIT_FAILURE);
  }

  if(listen(fd,2) < 0){
    perror("vrmc.c: listen");
    exit(EXIT_FAILURE);  
  }

  while(true){
    int csock;
    if((csock = accept(fd,(struct sockaddr*)&addr,(socklen_t *)&addrlen)) < 0){
      printf("Failed to accept client at %s\n",get_formated_time());

    } else {

      connect_t cnct_ptr;
      cnct_ptr.fd = csock;
      pthread_t client_thread;
      pthread_create(&client_thread,NULL,&handle_client,&cnct_ptr);
      // pthread_join(&client_thread,NULL);
    }
  }

}

void handle_client(void * cptr){
  connect_t * cnx = (connect_t*)cptr;
  unsigned int stage = STAGE_VERSION;
  
  while(true){
    if(stage == STAGE_VERSION){
      version_exchange(cnx);
    }
  }
  close(cnx->fd);
}

void version_exchange(connect_t * cnx){
  send(cnx->fd,VERSION,strlen(VERSION),0);

  char buffer[1024];
  int len_read = read(cnx->fd,buffer,1024);
  if(len_read <= 0){
    close(cnx->fd);
  } else{

    return;
  }
}