EXECUTABLE = main
CFLAGS = -std=gnu++17 -c -Wall -g3 -DDEBUG
LDFLAGS =
LIBS =





# main
build: $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE) < input > output


re: clean $(EXECUTABLE)

rerun: clean $(EXECUTABLE)
	$(EXECUTABLE)



$(EXECUTABLE): main.o
	g++ $(LDFLAGS) -o $(EXECUTABLE) main.o $(LIBS)

main.o: main.cpp
	g++ $(CFLAGS) -o main.o main.cpp





# clean
clean:
	-rm *.o $(EXECUTABLE)





# end
