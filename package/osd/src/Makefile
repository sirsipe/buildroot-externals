CFLAGS = -lraylib -lgps -lpthread

.PHONY: clean

osd: osd.c
	$(CC) $(CFLAGS) -o '$@' '$<'

clean:
	rm osd
