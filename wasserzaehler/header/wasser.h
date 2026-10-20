#ifndef WASSER_HEADER
#define WASSER_HEADER

/*
	Einstiegspunkt der Anwendung.
	Baut die serielle Verbindung zur AquaPerla auf und startet die Hauptschleife,
	in der zyklisch die Zählerdaten ausgelesen und per HTTP an openHAB gesendet werden.
*/
int main();

#else
#endif
