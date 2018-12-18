binaries = myfirst  fleas yourcat sqrt ourfunc limits hexoct convert exceed chartype morechar floatnum
clean: 
	rm  ${binaries} *.o
myfirst: myfirst.o
	g++ -o myfirst myfirst.o
myfirst.o: myfirst.cpp
	g++ -c -g myfirst.cpp
fleas: fleas.o
	g++ -o fleas fleas.o
fleas.o: fleas.cpp
	g++ -c -g fleas.cpp
yourcat: yourcat.o
	g++ -o $@ $^
yourcat.o: yourcat.cpp
	g++ -c -g $^
sqrt: sqrt.o
	g++ -o $@ $^
sqrt.o: sqrt.cpp
	g++ -c -g $^
ourfunc: ourfunc.o
	g++ -o $@ $^
ourfunc.o: ourfunc.cpp
	g++ -c -g $^
limits: limits.o
	g++ -o $@ $^
limits.o: limits.cpp
	g++ -c -g $^
hexoct: hexoct.o
	g++ -o $@ $^
hexoct.o: hexoct.cpp
	g++ -c -g $^
convert: convert.o
	g++ -o $@ $^
convert.o: convert.cpp
	g++ -c -g $^
exceed: exceed.o
	g++ -o $@ $^
exceed.o: exceed.cpp
	g++ -c -g $^
chartype: chartype.o
	g++ -o $@ $^
chartype.o: chartype.cpp
	g++ -c -g $^
morechar: morechar.o
	g++ -o $@ $^
morechar.o: morechar.cpp
	g++ -c -g $^
floatnum: floatnum.o
	g++ -o $@ $^
floatnum.o: floatnum.cpp
	g++ -c -g $^
