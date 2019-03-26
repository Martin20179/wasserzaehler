#ifndef SERIAL_HEADER
#define SERIAL_HEADER

#define MIN_LOOP_TIME_MS					(60*1000)	// 1Minute // alle 200ms (abfrage Durchfluss)
#define LOOP_TIME_ALLGEMEINE_COMMANDOS_MS	(5*60*1000)	// alle 5Minuten // alle 5s (generelle Abfrage aller Befehle)

int openConnection(int *USBCon);
int closeConnection(int *USBCon);
char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);
int checkInputBuf(char *inBuf, int length);
int frameAnalyse(char *frame, int size, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);

#else
#endif