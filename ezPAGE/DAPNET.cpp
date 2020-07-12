#include "DAPNET.h"

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int con_open(void){
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, MASTER_IP, &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock , auth , strlen(auth) , 0 );
    printf("Auth message sent [%s]\n", auth);
    return 0;
}

int con_close(void){
    return close(sock);
}

void time_dummy(void){
	valread = read( sock , buffer, 1024);
	
    printf("%s\n",buffer );
    buffer[strlen(buffer)-1] = '\0';
    strcpy(time1, buffer);
    strcat(time1, ":0000\n");
    strcat(time1,"+\n");
    send(sock , time1 , strlen(time1) , 0 );
    printf("String gesendet: %s\n", time1);
    valread = read( sock , buffer2, 1024);
    printf("%s\n",buffer2 );

    buffer2[strlen(buffer2)-1] = '\0';
    strcpy(time2, buffer2);
    strcat(time2, ":0000\n");
    strcat(time2,"+\n");

    send(sock , time2 , strlen(time2) , 0 );
    printf("String gesendet: %s\n", time2);
    valread = read( sock , buffer3, 1024);
    printf("%s\n",buffer3 );

    buffer3[strlen(buffer3)-1] = '\0';
    strcpy(time3, buffer3);
    strcat(time3, ":0000\n");
    strcat(time3,"+\n");

    send(sock , time3 , strlen(time3) , 0 );
    printf("String gesendet: %s\n", time3);
    valread = read( sock , buffer4, 1024);
    printf("%s\n",buffer4 );

    buffer4[strlen(buffer4)-1] = '\0';
    strcpy(time4, buffer4);
    strcat(time4, ":0000\n");
    strcat(time4,"+\n");

    send(sock , time4 , strlen(time4) , 0 );
    printf("String gesendet: %s\n", time4);
    valread = read( sock , buffer5, 1024);
    printf("%s\n",buffer5);

    buffer5[strlen(buffer5)-1] = '\0';
    strcpy(time5, buffer5);
    strcat(time5, ":0000\n");
    strcat(time5,"+\n");

    send(sock , time5 , strlen(time5) , 0 );
    printf("String gesendet: %s\n", time5);
    //nach dem 5. vergleich kommt eine Nachricht mit den Timeslots
    valread = read( sock , buff_end, 1024);
    printf("Zeit: %s\n",buff_end);
    send(sock, akn, strlen(akn), 0);
}



void get_timeslot(){
	valread = read( sock , buff_ts, 1024);
	
    if (valread > 0){
		printf("Timeslot: %s \n",buff_ts );
    }
    
	send(sock, akn, strlen(akn), 0);
}

char* get_msg(void){
    char *msg;
    
    buffer[0] = '\0';
    valread = read(sock, buffer, 1024);

    if(valread >0){
        msg = strtok(buffer, "\n");
        //printf("Nachricht: %s\n", msg);
        return msg;
    }

}

void msg_proc(char* msg){
    int ric;
    char *strptr;
    int msg_type;
    char *meldung;
    char *msg_ret;
    
    strptr = strtok(buffer, " :");
    strptr = strtok(NULL, " :");
    msg_type = hex_to_dec(strptr);
    dapnet_tx_flag = msg_type;
    strptr = strtok(NULL, ":");
    strptr = strtok(NULL, ":");

    ric = hex_to_dec(strptr);
    dapnet_ric = ric;
    strptr = strtok(NULL, ":");
    subric = strptr;
    meldung = strtok(NULL, "\n");
    printf("Meldung: %s\n", meldung);
    sprintf(msg_puff, "%s", meldung);

    printf("RIC: %d\n TX_FLAG: %d\n\n", ric, msg_type);
    akn_incr(msg);
    
    send(sock, akn_msg, strlen(akn_msg), 0);
    printf("%s \n", akn_msg);
    msg_type = 0;
    free(akn_puff);

}


int hex_to_dec(char *hex){
	int i;
	i = (int)strtol(hex, NULL, 16);
	return i;
}


void akn_incr(char* msg){
    char *buff;


    char buff1[4], buff2[4];
    int dig1, dig2;
    buff = strtok(msg, " ");

    akn_puff = (char*)malloc(10);

    strncpy(buff1, buff, 2);
    strncpy(buff2, buff, 3);
    dig1 = strtol(buff1+1, NULL, 16);
    dig2 = strtol(buff2+2, NULL, 16);

    if(dig1 < 1 && dig2 < 15){
    dig2++;
    sprintf(akn_puff, "#%x%x +\n", dig1, dig2);
    }
    
    else if(dig1 >= 1 && dig2 < 15){
        dig2++;
        sprintf(akn_puff, "#%x%x +\n", dig1, dig2);
    }
    
    else if(dig1 < 15 && dig2 == 15){
        dig1++;
        dig2 = 0;
        sprintf(akn_puff, "#%x%x +\n", dig1, dig2);
    }
    
    else{
        dig1 = 0;
        dig2 = 0;
        sprintf(akn_puff, "#%x%x +\n", dig1, dig2);
    }
    
    strcpy(akn_msg, akn_puff);

}

int get_ric(void){
	return dapnet_ric;
}

int get_tx_flag(void){
	return dapnet_tx_flag;
}

char *get_Nachricht(void){
	return msg_puff;
}

char *get_subric(void){
	return subric;
}

void timeslot(void){

    char buff_ts[] = "4:2AF\n";
    char* buffer = (char*)malloc(1);

    sprintf(buffer, "%c", buff_ts[2]);
    ts1 = hex_to_dec(buffer);
    sprintf(buffer, "%c", buff_ts[3]);
    ts2 = hex_to_dec(buffer);
    sprintf(buffer, "%c", buff_ts[4]);
    ts3 = hex_to_dec(buffer);

    sek1_s = (ts1) * 3.75;
    sek2_s = (ts2) * 3.75;
    sek3_s = (ts3) * 3.75;

    sek1_e = (ts1+1) * 3.75;
    sek2_e = (ts2+1) * 3.75;
    sek3_e = (ts3+1) * 3.75;
}

void wait_until_timeslot(void){
	
	while(1){
		time(&time_now);
		tm_now = localtime(&time_now);
		if((tm_now->tm_sec >= sek1_s && tm_now->tm_sec <= sek1_e) || (tm_now->tm_sec >= sek2_s && tm_now->tm_sec <= sek2_e) || (tm_now->tm_sec >= sek3_s && tm_now->tm_sec <= sek3_e)){
			return;
		}
		system("sleep 0.5");
	}

	if(tm_now->tm_min == 0){
		stunde_voll = 1;
	}
	
	if(tm_now->tm_min >= 3){
		stunde_voll = 0;
	}

}
