CFLAGS = -lraylib -lgps -lpthread -lstdc++

.PHONY: clean

osd: osd.cpp channelsservice.o gpsservice.o rotaccelservice.o threadedservice.o i2cbus.o
	$(CC) $(CFLAGS) -o '$@' $^

gpsservice.o: gpsservice.cpp
	$(CC) $(CFLAGS) -c -o '$@' $^

channelsservice.o: channelsservice.cpp
	$(CC) $(CFLAGS) -c -o '$@' $^

rotaccelservice.o: rotaccelservice.cpp
	$(CC) $(CFLAGS) -c -o '$@' $^

threadedservice.o: threadedservice.cpp
	$(CC) $(CFLAGS) -c -o '$@' $^

i2cbus.o: i2cbus.cpp
	$(CC) $(CFLAGS) -c -o '$@' $^

clean:
	rm osd
	rm *.o
