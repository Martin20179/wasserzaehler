# Wasserzähler – Auslesung der BWT AquaPerla Wasserenthärtungsanlage

Dieses Projekt liest über eine serielle USB-Verbindung die Betriebs- und Verbrauchsdaten
einer **BWT AquaPerla** Wasserenthärtungsanlage aus und überträgt die ermittelten Werte
per HTTP an eine [openHAB](https://www.openhab.org/)-Instanz. Dort können die Werte
gespeichert, visualisiert und für die Hausautomatisierung verwendet werden.

## Was das Programm macht

Das Programm `wasser` ist eine kleine C++-Anwendung für Linux (z. B. einen Raspberry Pi),
die dauerhaft im Hintergrund läuft. In einer Endlosschleife werden folgende Schritte
ausgeführt:

1. **Serielle Verbindung öffnen** zum Gerät unter `/dev/ttyACM0`
   (19200 Baud, 8N1, kein Flow-Control).
2. **Kommandos an die AquaPerla senden** und die Antworten einlesen. Jedes Telegramm
   besteht aus Start-Byte, Kommando, Parameteranzahl, optionalen Parametern, Prüfsumme
   (CRC) und Stop-Byte.
3. **Antworten prüfen** (Start-/Stop-Byte und CRC) und die enthaltenen Messwerte in
   Ganzzahlen umrechnen.
4. **Werte per HTTP** mittels `curl` (POST) an die openHAB-REST-Schnittstelle übertragen.
5. Bei einem Timeout während der Kommunikation wird die Verbindung automatisch
   geschlossen und neu aufgebaut.

### Ausgelesene bzw. berechnete Werte

| Wert | Beschreibung |
|------|--------------|
| Gesamtverbrauch (Liter) | Verbrauch seit Inbetriebnahme (IBN) |
| Aktueller Durchfluss (Liter/Stunde) | Aus der Verbrauchsänderung berechnet; fällt bei Stillstand auf 0 ab |
| Restkapazität Säule 1 / Säule 2 (Liter) | Verbleibende Weichwasserkapazität der beiden Austauschersäulen |
| Max. Durchfluss heute / gestern / seit IBN (Liter) | Spitzendurchflusswerte |
| Regenerationen seit IBN | Anzahl der Regenerationszyklen |
| Salzverbrauch seit IBN (kg) | Verbrauchtes Regeneriersalz |

Die Abfrage erfolgt in zwei Takten (siehe `wasserzaehler/header/serial.h`):

- **Durchflussabfrage**: standardmäßig jede Minute (`MIN_LOOP_TIME_MS`).
- **Allgemeine Abfrage** aller übrigen Werte: standardmäßig alle 5 Minuten
  (`LOOP_TIME_ALLGEMEINE_COMMANDOS_MS`).

## Projektstruktur

```
.
├── Makefile                      # Build-Konfiguration (erzeugt die Binary "wasser")
├── wasser_start.sh               # Startskript (z. B. für Autostart)
└── wasserzaehler
    ├── header
    │   ├── aquaperla.h           # Protokoll-Konstanten der AquaPerla (Kommandos, Telegrammaufbau)
    │   ├── http.h                # HTTP-/openHAB-Konfiguration (URL, Item-Namen)
    │   ├── serial.h              # Funktionsprototypen und Timing-Konstanten
    │   └── wasser.h              # Prototyp der main()
    └── sourcen
        ├── wasser.cpp            # Einstiegspunkt: Hauptschleife, Verbindungssteuerung
        ├── serial.cpp            # Serielle Kommunikation, Protokollauswertung, Durchflussberechnung
        └── http.cpp              # Übertragung der Werte per curl an openHAB
```

## Voraussetzungen

- Linux-System mit Zugriff auf die serielle Schnittstelle (`/dev/ttyACM0`).
- **g++** (C++-Compiler) und **make** zum Übersetzen.
- **curl** zur Laufzeit (wird vom Programm für die HTTP-Übertragung aufgerufen).
- Eine erreichbare **openHAB**-Instanz mit angelegten Items (siehe unten).

Installation der Werkzeuge unter Debian/Ubuntu/Raspberry Pi OS:

```bash
sudo apt-get update
sudo apt-get install build-essential curl
```

## Konfiguration

Vor dem Übersetzen sollten die Zielangaben in `wasserzaehler/header/http.h` an die eigene
Umgebung angepasst werden:

- **`HTTP_TARGET`** enthält die Basis-URL der openHAB-REST-Schnittstelle
  (Standard: `http://192.168.0.11:8080/rest/items/`). IP-Adresse und Port anpassen.
- Die **`ITEM_...`-Konstanten** sind die Namen der openHAB-Items, in die die Werte
  geschrieben werden. Diese Items müssen in openHAB vorhanden sein.

Der serielle Port ist in `wasserzaehler/sourcen/serial.cpp` fest auf `/dev/ttyACM0`
eingestellt. Falls das Gerät unter einem anderen Pfad erscheint, muss der Wert dort
angepasst werden.

Die Abfrageintervalle lassen sich über die Konstanten in
`wasserzaehler/header/serial.h` einstellen.

## Übersetzen (Build)

Im Projektverzeichnis:

```bash
make
```

Dadurch entsteht die ausführbare Datei `wasser` im Projektverzeichnis.

Zum Aufräumen der erzeugten Objekt- und Programmdateien:

```bash
make clean
```

## Ausführen

Das Programm direkt starten:

```bash
./wasser
```

Es gibt laufend Statusmeldungen auf der Konsole aus (z. B. aktueller Durchfluss,
Verbrauch, Restkapazitäten) und überträgt die Werte parallel an openHAB.

### Autostart im Hintergrund

Das mitgelieferte Skript `wasser_start.sh` startet das Programm zeitversetzt und im
Hintergrund (ohne Konsolenausgabe). Es wartet zunächst 5 Minuten und startet dann die
Binary per `nohup`:

```bash
# Auszug aus wasser_start.sh
sleep 300
nohup /etc/openhab2/c/wasser </dev/null >/dev/null 2>&1 &
```

Der Pfad `/etc/openhab2/c/wasser` ist beispielhaft und auf die eigene Installation
anzupassen (dorthin die übersetzte Binary kopieren oder den Pfad im Skript ändern).

## Hinweise

- Die Abfrage des Alarmstatus (`CMD_ALARM`) ist im Quellcode auskommentiert, da sie bei
  der AquaPerla zu einem Absturz führen kann. Zur Wiederherstellung muss in diesem Fall
  die Strom- und USB-Verbindung getrennt und neu hergestellt werden.
- Nach jeder Antwort wird eine kurze Pause eingelegt, um die Stabilität der Kommunikation
  mit der AquaPerla zu erhöhen.

## Protokolldetails

Eine ausführliche Dokumentation des seriellen Protokolls (Kommandos, Beispieltelegramme,
Prüfsummenberechnung) befindet sich in der Header-Datei
[`wasserzaehler/header/aquaperla.h`](wasserzaehler/header/aquaperla.h).
