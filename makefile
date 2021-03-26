#** Description: AeConversion http-daemon
#******************************************************************


aeclogger: main.o      
	gcc -O2 -o aeclogger main.o  -lz 
	strip  --strip-unneeded  aeclogger
	rm *.o

main.o: main.c
	gcc -O2 -c -Wno-unused-result -Wno-format-truncation -o main.o main.c


.PHONY: clean

clean:
	rm*.o
