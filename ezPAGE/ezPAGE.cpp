#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "rs232.h"
#include "rs232.c"
#include "DAPNET.h"
#include "DAPNET.cpp"
#include <ctime>

#define BAUDRATE 9600
#define BUFFSIZE 1000
#define com_timeout 3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


unsigned char buffer_e[1000];
struct tm *timeout;
time_t zeitp;
int start, stop;
using namespace std;


void kommando(char *adresse, char *subric, char* wdh, char *Nachricht, char *comport);
void kommando_dapnet(char *adresse, char *subric, char* wdh, char *Nachricht, char *comport);
void help();
int nachricht_laenge(char *adresse);
void nachrichten_bearbeitung(char *adresse, char *subric, char * wdh, char *Nachricht, char *comport);
void nachrichten_bearbeitung_dapnet(char *adresse, char *subric, char * wdh, char *Nachricht, char *comport);
void manueller_modus(void);
void abdeckungsmodus(void);
void DAPNET_modus(char *comport);
int comport_val(char *com);


int main(int argc, char *argv[])
{

    int zeichen = 0;
    string sges, s1, s2, s3, s4, s5;
    cout << ANSI_COLOR_GREEN <<"\n\n*****************************************\n";
    cout << "***ezPAGE - Steuersoftware fuer ezPOC8***" << endl;
    cout << "*****************************************\n\n\n" << ANSI_COLOR_RESET;
    
	if(argc > 1){
		if(strcmp(argv[1], "-h") == 0){
			help();
			return 0;
		}
		else if(strcmp(argv[1], "-m") == 0){
			cout << ANSI_COLOR_CYAN <<"**Manueller Modus**" << ANSI_COLOR_RESET << endl << endl;
			manueller_modus();
		}
		else if(strcmp(argv[1], "-r") == 0){
			cout << ANSI_COLOR_CYAN <<"**Netzabdeckungsmodus**" << ANSI_COLOR_CYAN << endl << endl;
			abdeckungsmodus();
		}
		else if(strcmp(argv[1], "-d") == 0){
			cout << ANSI_COLOR_CYAN <<"**DAPNET-Modus**" << ANSI_COLOR_CYAN << endl << endl;
			DAPNET_modus(argv[2]);
		}
		else if((argc > 3)&&(argv[5] == 0)){
			nachrichten_bearbeitung(argv[1], argv[2], argv[3], argv[4], "16");
		}

		else if (argc > 4){
			nachrichten_bearbeitung(argv[1], argv[2], argv[3], argv[4], argv[5]);
        }
		else{
			cout << ANSI_COLOR_RED <<"**Zu wenige Argumente!**" << endl;
			cout << "-h fuer Hilfe" << ANSI_COLOR_RESET << endl;
        }
	}
	else{
		cout << ANSI_COLOR_RED <<"**Zu wenige Argumente!**" << endl;
		cout << "-h fuer Hilfe" << ANSI_COLOR_RESET << endl;
}
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------
void kommando(char *adresse, char *subric, char* wdh, char *Nachricht, char *comport){
    char *cmd;
    char *s1;
    char *s2;
    int cp = atoi(comport);
    int rpt = atoi(wdh);
    char buff[BUFFSIZE];
    char eot[] = "*~*";
    
    cout << cp << endl;
    cmd = (char*)malloc(strlen(adresse) + strlen(subric) + strlen(wdh) + strlen(Nachricht) + 14);
    sprintf(cmd, "P %s %s %s %s \r", adresse, subric, wdh, Nachricht, comport);
    cout << cmd << endl;

    if(RS232_OpenComport(cp, BAUDRATE, "8N1", 0) != 0){
        cerr << "Fehler beim Oeffnen der seriellen Schnittstelle!";
        exit(-1);
    }
    else{
        RS232_cputs(cp, cmd);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        while(true){
        int c = RS232_PollComport(cp, buffer_e, BUFFSIZE);

            if(0 != c){
                buffer_e[c] = 0;
                cout << buffer_e;
                cout.flush();


                for (int i = 0; i < 1000; i++){
                    buff[i] = buffer_e[i];
                }
                if((strstr(buff, eot) || strstr(buff, "Format") || strstr(buff, "\0")) != 0){
                    cout << "\n***Uebertragung beendet!***" << endl;
                    break;
                }
            }

        }
        cout << "\n***Uebertragung beendet!***" << endl;
    }
    free(cmd);
    RS232_CloseComport(cp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------
void kommando_dapnet(char *adresse, char *subric, char* wdh, char *Nachricht, char *comport){
    char *cmd;
    char *s1;
    char *s2;
    int cp = atoi(comport);
    int rpt = atoi(wdh);
    char buff[BUFFSIZE];
    char eot[] = "*~*";
    cout << cp << endl;

    cmd = (char*)malloc(strlen(adresse) + strlen(subric) + strlen(wdh) + strlen(Nachricht) + 14);
    sprintf(cmd, "P %s %s %s %s \r", adresse, subric, wdh, Nachricht, comport);
    cout << cmd << endl;

    if(RS232_OpenComport(cp, BAUDRATE, "8N1", 0) != 0){
        cerr << "Fehler beim Oeffnen der seriellen Schnittstelle!";
    }
    else{
        wait_until_timeslot();
        RS232_cputs(cp, cmd);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        time(&zeitp);
        timeout = localtime(&zeitp);
        start = timeout->tm_sec;
         while(true){
			time(&zeitp);
			timeout = localtime(&zeitp);
			stop = timeout->tm_sec;
			if(stop-start <= com_timeout){
				int c = RS232_PollComport(cp, buffer_e, BUFFSIZE);

					if(0 != c){
						buffer_e[c] = 0;
						cout << buffer_e;
						cout.flush();
						
						for (int i = 0; i < 1000; i++){
							buff[i] = buffer_e[i];
						}
						if((strstr(buff, eot) || strstr(buff, "Format") || strstr(buff, "\0")) != 0){
						cout << "\n***Uebertragung beendet!***" << endl;
						break;
						}
					}

			}
			else{
				free(cmd);
				RS232_CloseComport(cp);
				return;
			}
		}
	}
    free(cmd);
    RS232_CloseComport(cp);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void help(){

	cout <<  ANSI_COLOR_CYAN <<"--------------------------------------------------------------------------------------\n";
	cout << "\n'ezPAGE aaaaaaa s w 'nachricht' p'\n" << endl;
	cout << "a -> Adresse 1 - 2.097.152" << endl;
	cout << "s -> Subric 1 - 4" << endl;	
	cout << "w -> Wiederholungen 0 - 9" << endl;
	cout << "n -> Nachricht 0 - 160 Zeichen" << endl;
	cout << "p -> serielle Schnittstelle wenn leer, dann ttyUSB0" << endl;
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "-h fuer Hilfe" << endl;
	cout << "-m fuer manueller Modus (Beenden durch Eingabe von 'x' fuer Adresse)" << endl;
	cout << "-r fuer Netzabdeckungsmodus (gibt periodische Nachricht aus)" << endl;
	cout << "-d cc (comport) fuer DAPNETmodus (in DAPNET.h eingestellte Zugangsdaten werden verwendet)" << endl;
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "2020 Lukas Rumpel DO4LR\n" << ANSI_COLOR_RESET;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
int nachricht_laenge(char *adresse){
	int val = atoi(adresse);
	int rest = val % 10;
	int anz;
	
	switch(rest){
		case 0:
			anz = 76;
		break;
		case 1:
			anz = 76;
		break;
		case 2:
			anz = 76;
		break;
		case 3:
			anz = 76;
		break;
		case 4:
			anz = 76;
		break;
		case 5:
			anz = 68;
		break;
		case 6:
			anz = 60;
		break;
		case 7:
			anz = 58;
		break;
		case 8:
			anz = 50;
		break;
		case 9:
			anz = 48;
		break;
		default:
			anz = 76;
		break;
	}
	return anz;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void nachrichten_bearbeitung(char *adresse, char *subric, char * wdh_n, char *Nachricht, char *comport){
	int zeichen = 0;
	string sges, s1, s2, s3, s4, s5;
	zeichen = nachricht_laenge(adresse);
	
	if(strlen(Nachricht) < zeichen){
        kommando(adresse, subric, wdh_n, Nachricht, comport);
    }
		else if(strlen(Nachricht) < 160){
			int wdh = atoi(wdh_n);
			int anz = strlen(Nachricht);
			sges = Nachricht;

			if((anz > 40)&&(anz <= 80)){
				s1 = sges.substr(0, 40);
				s2 = sges.substr(40, sges.length()-40);
				char *cs1 = new char[s1.length() + 1];
				strcpy(cs1, s1.c_str());
				char *cs2 = new char[s2.length() + 1];
				strcpy(cs2, s2.c_str());
			
				for(int i = 0; i < wdh+1; i++){
					kommando(adresse, subric, "0", cs1,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs2,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					cout << endl;
				}
			
				delete [] cs1;
				delete [] cs2;
			}

			else if((anz > 80)&&(anz <= 120)){
				s1 = sges.substr(0, 40);
				s2 = sges.substr(40, 40);
				s3 = sges.substr(80, sges.length()-80);
				char *cs1 = new char[s1.length() + 1];
				strcpy(cs1, s1.c_str());
				char *cs2 = new char[s2.length() + 1];
				strcpy(cs2, s2.c_str());
				char *cs3 = new char[s3.length() + 1];
				strcpy(cs3, s3.c_str());
			
				for(int i = 0; i < wdh+1; i++){
					kommando(adresse, subric, "0", cs1,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs2,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs3,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					cout << endl;
				}
			
				delete [] cs1;
				delete [] cs2;
				delete [] cs3;
			}

			else{
				s1 = sges.substr(0, 40);
				s2 = sges.substr(40, 40);
				s3 = sges.substr(80, 40);
				s4 = sges.substr(120, sges.length()-120);
				char *cs1 = new char[s1.length() + 1];
				strcpy(cs1, s1.c_str());
				char *cs2 = new char[s2.length() + 1];
				strcpy(cs2, s2.c_str());
				char *cs3 = new char[s3.length() + 1];
				strcpy(cs3, s3.c_str());
				char *cs4 = new char[s4.length() + 1];
				strcpy(cs4, s4.c_str());
				
				for(int i = 0; i < wdh+1; i++){
					kommando(adresse, subric, "0", cs1,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs2,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs3,  comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					kommando(adresse, subric, "0", cs4, comport);
					std::this_thread::sleep_for(std::chrono::seconds(5));
					cout << endl;
				}
				
				delete [] cs1;
				delete [] cs2;
				delete [] cs3;
				delete [] cs4;
			}
		}
		
	else{
        cout << "String > 160 Zeichen!" << endl;
    }

}
//---------------------------------------------------------------------------------------------------------------------------------------------
void nachrichten_bearbeitung_dapnet(char *adresse, char *subric, char * wdh_n, char *Nachricht, char *comport){
	int zeichen = 0;
	string sges, s1, s2, s3, s4, s5;
	zeichen = nachricht_laenge(adresse);
    
    if(strlen(Nachricht) < zeichen){
		kommando_dapnet(adresse, subric, wdh_n, Nachricht, comport);
    }
	else if(strlen(Nachricht) < 160){
        int wdh = atoi(wdh_n);
        int anz = strlen(Nachricht);
        sges = Nachricht;

        if((anz > 40)&&(anz <= 80)){
			s1 = sges.substr(0, 40);
			s2 = sges.substr(40, sges.length()-40);
			char *cs1 = new char[s1.length() + 1];
			strcpy(cs1, s1.c_str());
			char *cs2 = new char[s2.length() + 1];
			strcpy(cs2, s2.c_str());
			
			for(int i = 0; i < wdh+1; i++){
				kommando_dapnet(adresse, subric, "0", cs1,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs2,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				cout << endl;
			}
			
			delete [] cs1;
			delete [] cs2;
        }

        else if((anz > 80)&&(anz <= 120)){
			s1 = sges.substr(0, 40);
			s2 = sges.substr(40, 40);
			s3 = sges.substr(80, sges.length()-80);
			char *cs1 = new char[s1.length() + 1];
			strcpy(cs1, s1.c_str());
			char *cs2 = new char[s2.length() + 1];
			strcpy(cs2, s2.c_str());
			char *cs3 = new char[s3.length() + 1];
			strcpy(cs3, s3.c_str());
			
			for(int i = 0; i < wdh+1; i++){
				kommando_dapnet(adresse, subric, "0", cs1,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs2,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs3,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				cout << endl;
			}
			
			delete [] cs1;
			delete [] cs2;
			delete [] cs3;
        }

        else{
			s1 = sges.substr(0, 40);
			s2 = sges.substr(40, 40);
			s3 = sges.substr(80, 40);
			s4 = sges.substr(120, sges.length()-120);
			char *cs1 = new char[s1.length() + 1];
			strcpy(cs1, s1.c_str());
			char *cs2 = new char[s2.length() + 1];
			strcpy(cs2, s2.c_str());
			char *cs3 = new char[s3.length() + 1];
			strcpy(cs3, s3.c_str());
			char *cs4 = new char[s4.length() + 1];
			strcpy(cs4, s4.c_str());
			
			for(int i = 0; i < wdh+1; i++){
				kommando_dapnet(adresse, subric, "0", cs1,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs2,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs3,  comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				kommando_dapnet(adresse, subric, "0", cs4, comport);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				cout << endl;
			}
			
			delete [] cs1;
			delete [] cs2;
			delete [] cs3;
			delete [] cs4;
        }
	}
	
    else{
		cout << "String > 160 Zeichen!" << endl;
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------
void manueller_modus(void){
	string adr = "1", subr, wdh_n, Nachricht, comport;
	char *c_adr, *c_subr, *c_wdh_n, *c_Nachricht, *c_comport;

	cout << ANSI_COLOR_CYAN<<"Bitte geben Sie den seriellen Port an:";
	cin >> comport;
	c_comport = new char[comport.length()+1];
	strcpy(c_comport, comport.c_str());
	
    while(adr != "x"){
        cout << ANSI_COLOR_CYAN << "Adresse: ";
        cin >> adr;
        cout << "Subric: ";
        cin >>subr;
        cout << "Wiederholungen: ";
        cin >> wdh_n;
        cout << "Nachricht: "<< ANSI_COLOR_RESET;
        std::getline(std::cin >> std::ws, Nachricht);
        cout << Nachricht << endl << endl << endl;
        c_adr = new char[adr.length()+1];
        strcpy(c_adr, adr.c_str());
        c_subr = new char[subr.length()+1];
        strcpy(c_subr, subr.c_str());
        c_wdh_n = new char[wdh_n.length()+1];
        strcpy(c_wdh_n,wdh_n.c_str());
        c_Nachricht = new char[Nachricht.length()+1];
        strcpy(c_Nachricht, Nachricht.c_str());
            if(adr != "x"){
                nachrichten_bearbeitung(c_adr, c_subr, c_wdh_n, c_Nachricht, c_comport);
            }
        delete [] c_adr;
        delete [] c_subr;
        delete [] c_wdh_n;
        delete [] c_Nachricht;
        cout  << endl;
    }
	delete [] c_comport;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void abdeckungsmodus(void){
	string adr = "1", subr, Nachricht, comport;
	char *c_adr, *c_subr, *c_Nachricht, *c_comport;

	cout << ANSI_COLOR_CYAN <<"Bitte geben Sie den seriellen Port an:";
	cin >> comport;
	c_comport = new char[comport.length()+1];
	strcpy(c_comport, comport.c_str());
	cout << "Adresse: ";
	cin >> adr;
	cout << "Subric: ";
	cin >>subr;
	cout << "Nachricht: "<< ANSI_COLOR_RESET;
	std::getline(std::cin >> std::ws, Nachricht);
	cout << Nachricht << endl << endl << endl;
	c_adr = new char[adr.length()+1];
	strcpy(c_adr, adr.c_str());
	c_subr = new char[subr.length()+1];
	strcpy(c_subr, subr.c_str());

	c_Nachricht = new char[Nachricht.length()+1];
	strcpy(c_Nachricht, Nachricht.c_str());
	int periode;
	cout << "Alle wie viele Sekunden soll die Nachricht gesendet werden?(min. 60s): ";
	cin >> periode;
	
    if(periode >= 60){
        while(1){
            nachrichten_bearbeitung(c_adr, c_subr, "0", c_Nachricht, c_comport);
            std::this_thread::sleep_for(std::chrono::seconds(periode));
            cout<< endl ;
        }
    }
    
	delete [] c_adr;
	delete [] c_subr;
	delete [] c_Nachricht;
	delete [] c_comport;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
void DAPNET_modus(char *com){
    int con_err;
    char *message = "test";
    char *Nachricht;
    char *func;
    int adresse;
    int senden;
    char ric[9];
    
    con_err = con_open();
    if (con_err != 0){
		cerr << "Verbindung konnte nicht hergestellt werden!" << endl;
    }

    time_dummy();
    get_timeslot();
    timeslot();
    
    printf("Zugewiesene Timeslots: %d %d %d\n", ts1, ts2, ts3);
    printf("SEkunden zum senden: Slot1: %d - %d  Slot2: %d - %d  Slot3: %d - %d \n\n", sek1_s, sek1_e, sek2_s, sek2_e, sek3_s, sek3_e);
    
    while(1){
		printf("Nachrichten werden geholt!\n");
		message = get_msg();
		if(message != NULL){
			printf("Systemnachricht: %s \n", message);
			msg_proc(message);
			func = get_subric();
			Nachricht = get_Nachricht();
			adresse = get_ric();
			sprintf(ric, "%d", adresse);
			senden = get_tx_flag();
			printf("RIC: %d MSG_TYPE: %d\n\n", adresse, senden );
			
			if(senden == 6 && (adresse == 2504 || adresse == 200 ||adresse == 208 || adresse == 224 || adresse == 216)){
				//nachrichten_bearbeitung_dapnet(ric, func, "0", Nachricht, com);
			}
			
			else if((stunde_voll == 1) && (senden == 6 && (adresse == 2504 || adresse == 200 ||adresse == 208 || adresse == 224 || adresse == 216))){
				nachrichten_bearbeitung_dapnet(ric, func, "0", Nachricht, com);
			}
			
			else if(senden == 6){
				//wait_until_timeslot();
				nachrichten_bearbeitung_dapnet(ric, func, "0", Nachricht, com);
			}
			
			printf("+++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
			Nachricht[0] = '\0';
			system("sleep 1s");
		}
		
		else{
			con_close();
		}
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------
