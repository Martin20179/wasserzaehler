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
*/
int main()
{
	int USBCon = -1;
	float fWirkarbeitBezugTotal_kWh = -1.0;
	float fLeistung_W = -1.0;
	float fLeistungMin_W = 9999.0;
	float fLeistungMax_W = 0.0;

	cout << "*** Starte Wasserablesung ***\n\n";
	
	// Probiere USB/Seriell Verbindung zu öffnen
	printf("Open Connection...\n");
	openConnection(&USBCon);
		
	while(true)
	{
		//printf("Serial Read...\n");
		serialRead(&USBCon, &fWirkarbeitBezugTotal_kWh, &fLeistung_W, &fLeistungMin_W, &fLeistungMax_W);
		// wurden plausible Werte geschrieben?
		//if ( (fWirkarbeitBezugTotal_kWh != -1.0) && (fLeistung_W != -1.0) )
		//	http(&fWirkarbeitBezugTotal_kWh, &fLeistung_W);

		if(tryReconnect)
		{
			tryReconnect=false;
			// Ein Fehler ist aufgetreten (Timeout während Selection)
			// Neu Verbindung durchführen
			closeConnection(&USBCon);
			// Probiere USB/Seriell Verbindung zu öffnen
			printf("Open Connection...\n");
			openConnection(&USBCon);

		}
			
		usleep(MIN_LOOP_TIME_MS*1000);	// damit die CPU etwas entlasstet ist
	}
	
	closeConnection(&USBCon);
	
}
