#include <iostream>		// Standard-Ein-/Ausgabe
#include <stdio.h>      // standard input / output functions
#include <string.h>     // string function definitions
#include <stdlib.h>

#include "../header/http.h"

using namespace std;
using std::string;

int http(char *httpString)
{
	char strToSend[256];
	printf("     Try to send via http...");
	sprintf(strToSend, HTTP_HEAD "%s\n", httpString);
	system(strToSend);
	printf("done\n");
	
	return 0;
}