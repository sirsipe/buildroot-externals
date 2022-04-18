CFLAGS = -lraylib -lgps -lpthread -lstdc++

.PHONY: clean

osd: osd.cpp channels.cpp
	$(CC) $(CFLAGS) -o '$@' $^

clean:
	rm osd
