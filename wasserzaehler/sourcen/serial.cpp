#include <iostream>
#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iomanip>		// cout hex into string

#include <sys/time.h>		// time
#include <sys/select.h>

#include "../header/serial.h"		// eigener Header
#include "../header/aquaperla.h"	// Zähler Eigenschaften
#include "../header/http.h"

using namespace std;
using std::string;
using std::ostringstream;
//using std::stringstream

//struct termios tty;
//struct termios tty_old;
//memset (&tty, 0, sizeof tty);

fd_set readFds, writeFds, exceptFds;
struct timeval timeout;
bool tryReconnect=false;
int cntConnections=0;

/*
        Öffne  Serielle USB Verbindung
*/
int openConnection(int *USBCon)
{
	struct termios tty;
	string serialPort = "/dev/ttyACM0";
	
	*USBCon = open("/dev/ttyACM0", O_RDWR| O_NONBLOCK | O_NDELAY );
	if (*USBCon == -1)
	{
		perror("Fehler: Serieller Port </dev/ttyACM0>konnte nicht geöffnet werden\n");
		exit(EXIT_FAILURE);
	}
	cntConnections++;
	fcntl(*USBCon, F_SETFL, 0);
	
	FD_ZERO(&readFds); /* clear the set */
	FD_ZERO(&writeFds); /* clear the set */
	FD_ZERO(&exceptFds); /* clear the set */
	FD_SET(*USBCon, &readFds); /* add our file descriptor to the set */
	
	timeout.tv_sec = 3;			// 3s
	timeout.tv_usec = 0;	
	
	cout << "Serieller Port <" << serialPort << "> wurde erfolgreich geöffnet. File description ist <" << *USBCon << ">" << endl << endl;
	
	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B19200);
	cfsetispeed (&tty, (speed_t)B19200);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;		// Parity none
	tty.c_cflag     &=  ~CSTOPB;		// one StopBit
	tty.c_cflag     &=  ~CSIZE;			// clear out the current word size
	tty.c_cflag     |=  CS8;			// 8Bits (plus 1 Parity Bit)

	tty.c_cflag     &=  ~CRTSCTS;		// no flow control
	//tty.c_cflag     |=  CRTSCTS;		// flow control
	tty.c_cc[VMIN]	=   0;//1;			// read doesn't block
	tty.c_cc[VTIME]	=   5;				// Timeout (Einheit: 100ms)
	tty.c_cflag     |=  CREAD | CLOCAL;	// turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);
	
	/* Flush Port, then applies attributes */
	printf("Flush Port...\n");
	tcflush( *USBCon, TCIFLUSH );
	if ( tcsetattr ( *USBCon, TCSANOW, &tty ) != 0)
	{
		cout << "Error " << errno << " from tcsetattr" << endl;
	}
	
	/* Flush Port, then applies attributes */
	tcflush( *USBCon, TCIFLUSH );

}

double  curTimeMs = 0.0;
double  lastTimeMs_LoopDurchfluss = 0.0;
double  difTimeMs_LoopDurchfluss = 0.0; 
double  timeMs_LoopDurchfluss = MIN_LOOP_TIME_MS;
uint durchflussLiterProStunde = 0;

double  lastTimeMs_LoopAllgemein = 0.0;
double  difTimeMs_LoopAllgemein = 0.0;
double  timeMs_LoopAllgemein = LOOP_TIME_ALLGEMEINE_COMMANDOS_MS;

uint curLiter = 0;
uint lastLiter = 0;
double timeMs_LastLiter = 0.0;

bool nurDurchfluss = false;

char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W)
{
	// erst senden dann empfangen
	//char sendCmds[] = SEND_COMMANDS;
	char outBuf[16];
	char inBuf[256];
	int result = -1;

	// generell normale Abfrage
	nurDurchfluss = false;

	char cmd[CMD_Anzahl];//[CMD_Length];
	int cmdPos = 0;
	// get current time
	timeval tv;
	gettimeofday(&tv, NULL);
	curTimeMs = (double)(tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);

	difTimeMs_LoopDurchfluss = curTimeMs - lastTimeMs_LoopDurchfluss;
	difTimeMs_LoopAllgemein = curTimeMs - lastTimeMs_LoopAllgemein;
	//printf("dif %f - cur %f - last %f\n", difTimeMs_LoopDurchfluss, curTimeMs_LoopDurchfluss, lastTimeMs_LoopDurchfluss);
	if (difTimeMs_LoopDurchfluss < 0.0)
	{
		// bei Zählerüberlauf einfach neu Setzen;
		printf("Zählerüberlauf (Loop Durchlauf) ... dif %f cur %f last %f\n", difTimeMs_LoopDurchfluss, curTimeMs, lastTimeMs_LoopDurchfluss);
		lastTimeMs_LoopDurchfluss = curTimeMs;
	}
	if (difTimeMs_LoopAllgemein < 0.0)
	{
		// bei Zählerüberlauf einfach neu Setzen;
		printf("Zählerüberlauf (Loop Allgemein) ... dif %f cur %f last %f\n", difTimeMs_LoopAllgemein, curTimeMs, lastTimeMs_LoopAllgemein);
		lastTimeMs_LoopAllgemein = curTimeMs;
	}

	if (difTimeMs_LoopDurchfluss >= timeMs_LoopDurchfluss)
	{
		// nur abfragen, wenn LoopTime abgelaufen ist
		cmd[cmdPos++] = CMD_VERBRAUCH_SEIT_IBN;
		//printf("Dif %d Cur %d last %d\n", (int)difTimeMs_LoopDurchfluss, (int)curTimeMs, (int) lastTimeMs_LoopDurchfluss);
		lastTimeMs_LoopDurchfluss = curTimeMs;
		nurDurchfluss = true;
	}
	
	if (difTimeMs_LoopAllgemein >= timeMs_LoopAllgemein)
	{
		if (!nurDurchfluss) // falls in diesem Zyklus auch LoopDurchfluss zutrifft, dann wurde diese Abfrage bereits angefügt
			cmd[cmdPos++] = CMD_VERBRAUCH_SEIT_IBN;
		cmd[cmdPos++] = CMD_SAEULE1_RESTKAPAZITAET;
		cmd[cmdPos++] = CMD_SAEULE2_RESTKAPAZITAET;
		cmd[cmdPos++] = CMD_MAX_DURCHFLUSS_HEUTE_LITER;
		cmd[cmdPos++] = CMD_MAX_DURCHFLUSS_GESTERN_LITER;
		cmd[cmdPos++] = CMD_MAX_DURCHFLUSS_SEIT_IBN_LITER;
		cmd[cmdPos++] = CMD_REGENERATIONEN_SEIT_IBN; 
		cmd[cmdPos++] = CMD_SALZVERBRAUCH_GRAMM_SEIT_IBN;
		
		/* Alarmabfrage kann zum Absturz der AquaPerla führen 
		   Strom und USB ziehe
		   Strom anschließen
		   anschließend auch wieder USB*/
		//cmd[cmdPos++] = CMD_ALARM;
	
		//printf("Dif %d Cur %d last %d\n", (int)difTimeMs_LoopAllgemein, (int)curTimeMs, (int)lastTimeMs_LoopAllgemein);
		lastTimeMs_LoopAllgemein = curTimeMs;
		nurDurchfluss = false;
	}
	//cmd[0][1] = CMD_VERBRAUCH_SEIT_IBN_LENGTH;


	for (int i = 0; i < cmdPos; i++)
	{
		int pos = 0;
		int crc = 0;
		outBuf[pos++] = START_BYTE;					// START
		crc = outBuf[pos - 1];
		outBuf[pos++] = cmd[i];						// COMMANDO
		crc += outBuf[pos - 1];
		if (cmd[i] == CMD_ALARM)
			outBuf[pos++] = 1;						// ANZAHL PARAMETER
		else
			outBuf[pos++] = 0;						// ANZAHL PARAMETER
		crc += outBuf[pos - 1];
		if (cmd[i] == CMD_ALARM)
		{
			outBuf[pos++] = 1;						// Zähler
			crc += outBuf[pos - 1];
		}
		outBuf[pos++] = crc;						// CRC
		outBuf[pos++] = STOP_BYTE;					// STOP
		
		// senden...
		//printf("Write... (%d)\n", *USBCon);
		result = write(*USBCon, outBuf, pos);
		
		if (0)
		{
			printf("Gesendet: %dBytes - Buffer: ", result);
			for (int j = 0; j < result; j++)
			{
				printf("%02x", 0xFF & outBuf[j]);
			}
			cout << endl << endl;
		}
		
		// Antwort empfangen...
		//printf("Select...\n");
		result = select(*USBCon + 1, &readFds, &writeFds, &exceptFds, &timeout);
		if(result == -1)
		{
			printf("Fehler: Select-Funktion (Kommando 0x%02X)\n", cmd[i]);
		}
		else if(result == 0)
		{
			printf("Warnung: Timeout Select-Funktion (Kommando 0x%02X)\n", cmd[i]);
			tryReconnect=true;
		}
		else
		{
			result = read(*USBCon, inBuf, sizeof(inBuf));//cmd[i][1]);
			//printf("Read...(Erg = %d)\n", result);
			if (0)
			{
				printf("Gelesen: %dBytes - Buffer: ", result);
				for (int j = 0; j < result; j++)
				{
					printf("%02x", 0xFF & inBuf[j]);
				}
				printf("\n");
			}
			result = checkInputBuf(inBuf, result);
			//printf("Check...(Erg = %d)\n", result);
			
			if (result == 0)
			{
				char strhttp[200];
				uint uErg = 0;
				// Im Falle folgender Abfragen benötigt es keiner integer Berechnung
				// ...CMD_ALARM
				if ((inBuf[1] & 0xFF) != CMD_ALARM)
				{
					for (int j = 0; j < inBuf[2]; j++)
						uErg += (inBuf[3 + j] & 0xFF) << (8 * j);
				}
				
				//printf("Switch...(Erg = 0x%X)\n", inBuf[1] & 0xFF);
				switch (inBuf[1] & 0xFF)
				{
				case CMD_VERBRAUCH_SEIT_IBN:
					//printf("CMD_VERBRAUCH_SEIT_IBN\n");
					if (1) // Durchfluss bei jeder Möglichkeit überprüfen
					{
						// aktueller Verbrauch (Liter)
						curLiter = uErg;
						//printf("Liter %d\n", curLiter);
						bool sendDurchfluss = false;

						if (((curLiter > lastLiter) && (lastLiter > 0)) || (durchflussLiterProStunde > 0))
						{
							// Durchfluss berechnen
							if ((curLiter > lastLiter) && (lastLiter > 0))
							{
								durchflussLiterProStunde = (uint)((double)(curLiter - lastLiter) / (curTimeMs - timeMs_LastLiter)* 1000.0 * 60.0 * 60.0);
								sendDurchfluss = true;
							}
							else
							{
								// Kurve abfallen lassen
								// Annahme 100ml wurde Verbraucht
								uint neuDurchflussLiterProStunde = (uint)(0.1 / (curTimeMs - timeMs_LastLiter)* 1000.0 * 60.0 * 60.0);
								if (neuDurchflussLiterProStunde < durchflussLiterProStunde)
								{
									durchflussLiterProStunde = neuDurchflussLiterProStunde;
									sendDurchfluss = true;
								}
							}

							// nur wenn min. 1Liter verbraucht wurde
							// neuer letzter Verbrauch (Liter) und Zeitstempel
							// ...dadurch wird eine Abfallende Flanke bis auf 0 Liter/Stunde erzeugt
							if ((curLiter > lastLiter) && (lastLiter > 0))
							{
								lastLiter = curLiter;
								timeMs_LastLiter = curTimeMs;
							}
						}

						// für den initalen Fall
						if (lastLiter == 0)
						{
							// einmaliges initales setzen
							// letzter Verbrauch (Liter) und Zeitstempel
							lastLiter = curLiter;
							timeMs_LastLiter = curTimeMs;
							durchflussLiterProStunde = 0;
							sendDurchfluss = true;
						}

						// über http verschicken...
						if (sendDurchfluss)
						{
							printf("Durchfluss %d l/h\n", durchflussLiterProStunde);
							sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_AKTUELLER_DURCHFLUSS_LITER_PRO_STUNDE, durchflussLiterProStunde);
							http(strhttp);
						}
					}
					// wird nicht benötigt, da nur Durchfluss berechnet werden soll
					if (!nurDurchfluss)
					{
						printf("Wasserverbrauch: %u Liter\n", uErg);
						// über http verschicken...
						sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_GESAMTVERBRAUCH_LITER, uErg);
						http(strhttp);
					}
				break;
				case CMD_SAEULE1_RESTKAPAZITAET:
					printf("Säule1 Restkapiztät: %u Liter\n", uErg);
					// über http verschicken...
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_SAEULE1_RESTKAPZIAET, uErg);
					http(strhttp);
					break;
				case CMD_SAEULE2_RESTKAPAZITAET:
					printf("Säule2 Restkapiztät: %u Liter\n", uErg);
					// über http verschicken...
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_SAEULE2_RESTKAPZIAET, uErg);
					http(strhttp);
					break;
				case CMD_MAX_DURCHFLUSS_HEUTE_LITER:
					printf("Max. Druchfluss heute: %u Liter\n", uErg);
					// über http verschicken...
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_MAX_DURCHFLUSS_HEUTE_LITER, uErg);
					http(strhttp);
					break;
				case CMD_MAX_DURCHFLUSS_GESTERN_LITER:
					printf("Max. Durchfluss gestern: %u Liter\n", uErg);
					// über http verschicken...
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_MAX_DURCHFLUSS_GESTERN_LITER, uErg);
					http(strhttp);
					break;
				case CMD_MAX_DURCHFLUSS_SEIT_IBN_LITER:
					printf("Max. Durchfluss seit IBN: %u Liter\n", uErg);
					// über http verschicken...
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_WASSER_MAX_DURCHFLUSS_SEIT_IBN_LITER, uErg);
					http(strhttp);
					break;
				case CMD_REGENERATIONEN_SEIT_IBN:
					printf("Regenerationen seit IBN %u\n", uErg);
					sprintf(strhttp, "%d" HTTP_TARGET ITEM_REGENERATIONEN_SEIT_IBN, uErg);
					http(strhttp);
					break;
				case CMD_SALZVERBRAUCH_GRAMM_SEIT_IBN:
				{
					float salzKg = (float)uErg / 1000.0;
					printf("Salzverbrauch seit IBN %.3f kg\n", salzKg);
					printf("Anzahl der USB-Verbindungsaubauten %d\n",cntConnections);
					// über http verschicken...
					sprintf(strhttp, "%.3f" HTTP_TARGET ITEM_WASSER_SALZVERBRAUCH_KG_SEIT_IBN, salzKg);
					http(strhttp);
				}
					break;
				case CMD_ALARM:
					printf("Alarm ...\n  ");
					for (int j = 0; j < inBuf[2]; j++)
					{
						// gehe alle Zeichen durch
						printf("%c", inBuf[3 + j] & 0xFF);
					}
					printf("\n");
					sleep(1); // sicherheitshalber drin, damit sich die AquaPerla nicht verhändert und abstürtzt
					break;
				default:
					printf("Fehler: Unbekanntes Kommando");
					break;
				}

				//usleep(10);	// 10us sleep - Sicherheitshalber wegen Instabilität von AquaPerla
				//printf("sleep 100ms\n");
				usleep(100*1000);	// 100ms
			}
		}
		
	}
	
	return 0;
}

int checkInputBuf(char *inBuf, int length)
{
	int res = 0;
	// prüfe Start und Stop Byte
	if ((inBuf[0] != START_BYTE) && (inBuf[length] != STOP_BYTE))
	{
		printf("Fehler: Start oder Stop Byte nicht vorhanden\n");
		res--;
	}

	// prüfe CRC
	int crc = 0;
	// CRC errechnen (ohne letztes zwei Bytes)
	for (int i = 0; i < (length - 2); i++)
	{
		crc += inBuf[i];
	}
	// Vergleichen
	if ((crc & 0xFF) != (inBuf[length - 2] & 0xFF))
	{
		printf("Fehler CRC: Erhalten 0x%x - Errechnet 0x%x\n", (inBuf[length - 2] & 0xFF), (crc & 0xFF));
		res--;
	}

	if (res != 0)
	{
		printf("Fehlerhafter Eingangsbuffer: %dBytes - Buffer: ", length);
		for (int j = 0; j < length; j++)
		{
			printf("%02x", 0xFF & inBuf[j]);
		}
		printf("\n");
	}

	return res;
}

int closeConnection(int * USBCon)
{
	close(*USBCon);
	return 0;
}
