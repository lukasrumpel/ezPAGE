#ifndef DAPNET_H
#define DAPNET_H

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <ctime>

#define PORT 43434 //DAPNET 43434 lokal 8080
#define MASTER_IP "137.226.79.100"

using namespace std;

int sock = 0, valread;
struct sockaddr_in serv_addr;
char *auth = "[UniPager-Dummy v1.0.3 CALL VerygoodAuthKey]\n";
char buffer[1024] = {0};
char buffer_msg;


char buffer2[1024] = {0};
char buffer3[1024] = {0};
char buffer4[1024] = {0};
char buffer5[1024] = {0};
char buff_end[1024] ;
char buff_ts[1024] ;
char *akn = "+\n";
char *akn2 = "#02 +\n";
char *akn_puff;
char akn_msg[8];
char time1[1024];
char time2[1024];
char time3[1024];
char time4[1024];
char time5[1024];
char *callback_buff ;
int callback_val;
int ts1, ts2, ts3;
int sek1_s, sek2_s, sek3_s;
int sek1_e, sek2_e, sek3_e;
struct tm *tm_now;
time_t time_now;
int stunde_voll = 0;

int dapnet_ric;
int dapnet_tx_flag;
char msg_puff[1024];
char* subric;


void time_dummy(void);
void get_timeslot(void);

int con_open(void);
int con_close(void);
int hex_to_dec(char *hex);
char* get_msg(void);
void msg_proc(char* msg);
void akn_incr(char* msg);

char *get_subric(void);
char *get_Nachricht(void);
int get_ric(void);
int get_tx_flag(void);
void timeslot(void);
void wait_until_timeslot(void);


#endif // DAPNET_H

