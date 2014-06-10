all: send codesend RFSniffer

SQLITE = -l sqlite3
CFLAG = -Wall -W
send: RCSwitch.o send.o
	$(CXX) $(CFLAG) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi $(SQLITE)
	
codesend: RCSwitch.o codesend.o
	$(CXX) $(CFLAG) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi $(SQLITE)
	
RFSniffer: RCSwitch.o RFSniffer.o
	$(CXX) $(CFLAG) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi $(SQLITE)
	

clean:
	$(RM) *.o send codesend servo RFSniffer