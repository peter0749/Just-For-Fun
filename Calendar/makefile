all: calendar
calendar: calendar.cpp
	g++ calendar.cpp -O2 -o calendar
clean:
	rm -f calendar
install: calendar
	cp ./calendar /usr/local/bin/calendar
uninstall:
	rm -f /usr/local/bin/calendar
