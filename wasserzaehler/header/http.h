#ifndef HTTP_HEADER
#define HTTP_HEADER

#define HTTP_HEAD						"curl --header \"Content-Type: text/plain\" --request POST --data \""
#define HTTP_TARGET						"\" http://192.168.0.11:8080/rest/items/"
#define ITEM_WASSER_GESAMTVERBRAUCH_LITER					"UG_TECHNIK_Wasser_Gesamtverbrauch_Liter"
#define ITEM_WASSER_SAEULE1_RESTKAPZIAET					"UG_TECHNIK_Wasser_Saeule1_Restkapazitaet_Liter"
#define ITEM_WASSER_SAEULE2_RESTKAPZIAET					"UG_TECHNIK_Wasser_Saeule2_Restkapazitaet_Liter"
#define ITEM_WASSER_MAX_DURCHFLUSS_HEUTE_LITER				"UG_TECHNIK_Wasser_MaxDurchlfuss_Heute_Liter"
#define ITEM_WASSER_MAX_DURCHFLUSS_GESTERN_LITER			"UG_TECHNIK_Wasser_MaxDurchlfuss_Gestern_Liter"
#define ITEM_WASSER_MAX_DURCHFLUSS_SEIT_IBN_LITER			"UG_TECHNIK_Wasser_MaxDurchlfuss_Seit_IBN_Liter"
#define ITEM_WASSER_AKTUELLER_DURCHFLUSS_LITER_PRO_STUNDE	"UG_TECHNIK_Wasser_AktuellerDurchfluss_LiterProStunde"
#define ITEM_REGENERATIONEN_SEIT_IBN						"UG_TECHNIK_Wasser_Regenerationen_Seit_IBN"
#define ITEM_WASSER_SALZVERBRAUCH_KG_SEIT_IBN				"UG_TECHNIK_Wasser_Salzverbrauch_Seit_IBN_kg"


int http(char *httpString);

#else
#endif
