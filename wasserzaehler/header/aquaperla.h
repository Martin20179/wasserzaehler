#ifndef AQUA_PERLA_HEADER
#define AQUA_PERLA_HEADER
#define DEBUG 3

#define START_BYTE		0x0D
#define STOP_BYTE		0x0A

#define CMD_Anzahl		256		// Anzahl Kommandos (Zeilen)
//#define CMD_Length		2		// Kommandolänge (Spalten)
// {	Kommando, Antwortlänge }
#define CMD_VERBRAUCH_SEIT_IBN				0x10
#define CMD_SAEULE1_RESTKAPAZITAET			0x02	// Sollkapazität Säule 1
#define CMD_SAEULE2_RESTKAPAZITAET			0x03	// Sollkapazität Säule 2
#define CMD_MAX_DURCHFLUSS_HEUTE_LITER		0x05	// Spitzendurchfluss heute
#define CMD_MAX_DURCHFLUSS_GESTERN_LITER	0x06	// Spitzendurchfluss gestern
#define CMD_MAX_DURCHFLUSS_SEIT_IBN_LITER	0x07	// Spitzendurchfluss seit IBN 
#define CMD_REGENERATIONEN_SEIT_IBN			0x11	// Regenerationen seit IBN
#define CMD_SALZVERBRAUCH_GRAMM_SEIT_IBN	0x13	// Salztverbrauch seit IBN
#define CMD_ALARM							0x20	// Alarm
//#define CMD_VERBRAUCH_SEIT_IBN_LENGTH		9

#define SEND_COMMANDS	{	0x10, /*Verbrauchdaten seit IBN Verbrauch*/\
							0x02, 0x03, 0x08, 0x11, 0x13, 0x25, 0x26}

/*
Typ
0D,01,00,0E,0A,
0D 01 01 0C 1B 0A
0D 01 01 0C 1B 0A

Sollkapazität Säule 1
0D,25,00,32,0A,
0D 25 02 D8 01 0D 0A
0D 25 02 LSB HSB CRC STOP

Sollkapazität Säule 2
0D,26,00,33,0A,
0D 26 02 D8 01 0E 0A
0D 26 02 LSB HSB CRC STOP

Restkapazität Säule 1
0D,02,00,0F,0A,
0D 02 02 5B 01 6D 0A
0D 02 02 LSB HSB CRC STOP

Restkapazität Säule 2
0D,03,00,10,0A,
0D 03 02 3B 01 4E 0A
0D 03 02 LSB HSB CRC STOP

Regenerationsschritt
0D,04,00,11,0A,
0D 04 01 00 12 0A
0D 04 01 Reg-Schritt CRC STOP

Softwareversion
0D,19,00,26,0A,
0D 19 11 56 65 72 73 69 6F 6E 3A 20 33 2E 39 34 23 31 35 35 03 0A
<CR><EM><DC1>Version:<Space>3.94#155<ETX><LF>

Verbrauchdaten Allgemein Spitzendurchfluss
0D,05,00,12,0A,
0D 05 02 00 00 14 0A
0D 05 02 LSB HSB CRC STOP

Verbrauchdaten Allgemein Spitzendurchfluss 24h
0D,06,00,13,0A,
0D 06 02 70 03 88 0A
0D 06 02 LSB HSB CRC STOP

Verbrauchdaten Allgemein Verbrauch 24h
0D,08,00,15,0A,
0D 08 02 00 00 17 0A
0D 08 02 90 00 A7 0A!!!!!

Verbrauchdaten seit IBN Spitzendurchfluss
0D,07,00,14,0A,
0D 07 02 39 04 53 0A
0D 07 02 LSB HSB CRC STOP

Verbrauchdaten seit IBN Verbrauch 24h
0D,09,00,16,0A,
0D 09 02 A7 00 BF 0A
0D 09 02 LSB HSB CRC STOP

Verbrauchdaten seit IBN Salzverbrauch
0D,13,00,20,0A,
0D 13 04 FA 00 00 00 1E 0A
0D 13 04 LSB HSB 00 00 CRC STOP

Verbrauchdaten seit IBN Reg-Mitteleinsparung
0D,14,00,21,0A,
0D 14 02 00 00 23 0A
0D 14 02 LSB HSB CRC STOP

Verbrauchdaten seit IBN Verbrauch
0D,10,00,1D,0A,
0D 10 04 08 02 00 00 2B 0A
0D 10 04 LSB .SB .SB HSB CRC STOP

Regenerationen seit IBN
0D,11,00,1E,0A,
0D 11 02 01 00 21 0A
0D 11 02 LSB HSB CRC STOP

Regenerationen seit letztem Service
0D,12,00,1F,0A,
0D 12 02 01 00 22 0A
0D 12 02 LSB HSB CRC STOP

Abfrage KW (38d 26h, 14d 0Eh)
0D,24,02,26,0E,67,0A,
0D 24 1C 32 32 2D 30 39 2D 31 34 23 20 20 31 30 38 31 23 20 20 20 31 32 31 23 20 20 20 20 32 F2 0A
HEADER Reportdatum Trennzeichen LEER LEER max. Durchfluss (l/h) Trennzeichen LEER LEER LEER Wochenverbrauch (l) Trennzeichen LEER LEER LEER LEER Regenerationen Appendix

Abfrage Log
0D,21,02,00,09,39,0A, Monat Prüfsumme
0D 21 25 32 31 2D 30 39 2D 31 34 2D 2D 2D 31 36 3A 32 36 3A 30 38 2D 3E 3E 74 79 70 65 20 63 68 61 6E 67 65 3A 20 31 32 BE 0A
<CR>!%21-09-14---16:26:08->>type<Space>change:<Space>12<(BE)><LF>

Abfrage Alarm
0D,20,01,00,2E,0A, Zähler Prüfsumme
0D 20 1F 41 6C 61 72 6D 3A 20 32 34 20 3A 20 32 31 2D 30 39 2D 31 34 20 2D 20 31 36 3A 34 35 3A 34 35 18 0A
<CR><Space><US>Alarm:<Space>24<Space>:<Space>21-09-14<Space>-<Space>16:45:45<CAN><LF>

0D,20,01,01,2F,0A, Zähler Prüfsumme *
*/
#else
#endif