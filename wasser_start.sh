# Autostart-Skript fuer die Wasserablesung
#
# Startet die Binary "wasser" zeitversetzt und im Hintergrund.
# Die Verzoegerung (sleep) gibt dem System / der AquaPerla Zeit, vollstaendig
# hochzufahren, bevor die serielle Kommunikation beginnt.
#
# Hinweis: Der Pfad zur Binary ist beispielhaft und auf die eigene Installation
#          anzupassen (uebersetzte Datei dorthin kopieren oder Pfad aendern).

# 5 Minuten (300s) warten, bevor gestartet wird
sleep 300

# Wasserablesung im Hintergrund starten (ohne Konsolenausgabe, ueberlebt das Abmelden)
nohup /etc/openhab2/c/wasser </dev/null >/dev/null 2>&1 &
