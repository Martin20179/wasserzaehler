# Strom und Wasser Zeitversetzt (10 Minuten) starten
sleep 300

# Wasser starten
nohup /etc/openhab2/c/wasser </dev/null >/dev/null 2>&1 &
