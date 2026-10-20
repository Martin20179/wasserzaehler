#ifndef SERIAL_HEADER
#define SERIAL_HEADER

// Abfrageintervalle (in Millisekunden)
#define MIN_LOOP_TIME_MS					(60*1000)	// 1 Minute: Takt der Durchflussabfrage
#define LOOP_TIME_ALLGEMEINE_COMMANDOS_MS	(5*60*1000)	// 5 Minuten: Takt der allgemeinen Abfrage aller uebrigen Werte

// Oeffnet die serielle USB-Verbindung zur AquaPerla und konfiguriert den Port
int openConnection(int *USBCon);
// Schliesst die serielle Verbindung
int closeConnection(int *USBCon);
// Sendet die faelligen Kommandos, liest die Antworten und sendet die Werte per HTTP
char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);
// Prueft Start-/Stop-Byte sowie die Pruefsumme (CRC) einer empfangenen Antwort
int checkInputBuf(char *inBuf, int length);
// (nicht implementiert) Auswertung eines kompletten Telegramms
int frameAnalyse(char *frame, int size, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);

#else
#endif