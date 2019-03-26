VERSION = 0.01
CC      = g++

OBJ_WASSER = wasserzaehler/sourcen/wasser.o wasserzaehler/sourcen/serial.o wasserzaehler/sourcen/http.o
	
wasser: $(OBJ_WASSER)
	$(CC) $(CFLAGS) -o wasser $(OBJ_WASSER) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm $(OBJ_WASSER)
	rm wasser
