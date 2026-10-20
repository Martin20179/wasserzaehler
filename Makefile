# Build-Konfiguration fuer die Wasserablesung
# Erzeugt die ausfuehrbare Datei "wasser" aus den C++-Quellen.
#   make        -> uebersetzt und linkt das Programm
#   make clean  -> entfernt Objektdateien und die Binary

VERSION = 0.01
CC      = g++

# Objektdateien der einzelnen Uebersetzungseinheiten
OBJ_WASSER = wasserzaehler/sourcen/wasser.o wasserzaehler/sourcen/serial.o wasserzaehler/sourcen/http.o

# Linkt die Objektdateien zur fertigen Binary "wasser"
wasser: $(OBJ_WASSER)
	$(CC) $(CFLAGS) -o wasser $(OBJ_WASSER) $(LDFLAGS)

# Regel zum Uebersetzen der C++-Quelldateien (.cpp) in Objektdateien (.o)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Entfernt alle erzeugten Objektdateien und die Binary
clean:
	rm -f $(OBJ_WASSER)
	rm -f wasser
