all: calendar calendar2
calendar: calendar.cpp calendar2.cpp
	g++ calendar.cpp -O2 -o calendar
	g++ calendar2.cpp -O2 -o calendar2
clean:
	rm -f calendar
	rm -f calendar2
install: calendar calendar2 today
	cp ./calendar /usr/local/bin/calendar
	cp ./calendar2 /usr/local/bin/calendar2
	cp ./today /usr/local/bin/today
uninstall:
	rm -f /usr/local/bin/calendar
	rm -f /usr/local/bin/calendar2
	rm -f /usr/local/bin/today
