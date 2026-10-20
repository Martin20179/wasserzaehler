#include <iostream>
#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iomanip>		// cout hex into string

#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../header/serial.h"
#include "../header/http.h"

using namespace std;
using std::string;

extern bool tryReconnect;

/*
	Hauptfunktion

	Baut die serielle Verbindung zur AquaPerla auf und liest in einer Endlosschleife
	zyklisch die Zaehlerdaten aus. Die ausgelesenen Werte werden innerhalb von
	serialRead() per HTTP an openHAB gesendet. Tritt bei der Kommunikation ein
	Timeout auf, wird die Verbindung geschlossen und neu aufgebaut.

	Die float-Parameter fWirkarbeit.../fLeistung... stammen aus einem verwandten
	Projekt (Stromablesung) und werden hier nicht ausgewertet.
*/
int main()
{
	int USBCon = -1;
	float fWirkarbeitBezugTotal_kWh = -1.0;
	float fLeistung_W = -1.0;
	float fLeistungMin_W = 9999.0;
	float fLeistungMax_W = 0.0;

	cout << "*** Starte Wasserablesung ***\n\n";
	
	// Serielle USB-Verbindung zur AquaPerla oeffnen
	printf("Open Connection...\n");
	openConnection(&USBCon);
		
	while(true)
	{
		// Faellige Kommandos senden, Antworten lesen und Werte per HTTP uebertragen
		serialRead(&USBCon, &fWirkarbeitBezugTotal_kWh, &fLeistung_W, &fLeistungMin_W, &fLeistungMax_W);

		if(tryReconnect)
		{
			tryReconnect=false;
			// Es ist ein Fehler aufgetreten (Timeout waehrend select)
			// -> Verbindung schliessen und neu aufbauen
			closeConnection(&USBCon);
			printf("Open Connection...\n");
			openConnection(&USBCon);

		}
			
		usleep(MIN_LOOP_TIME_MS*1000);	// kurze Pause, um die CPU zu entlasten
	}
	
	closeConnection(&USBCon);
	
}
