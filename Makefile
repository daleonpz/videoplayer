CC  := gcc
OBJECTS	:= main.o parser.o video_selector.o
CFLAGS	:= -g -Wall -W
LFLAGS	:=-lavutil -lavformat -lavcodec -lswscale -lz -lm  `sdl-config --cflags --libs` 
HEADERS	:= headers
VPATH	:= $(HEADERS)

.PHONY: all
all: main.o parser.o video_selector.o
	$(CC) $(CFLAGS) -o  main $^ $(LFLAGS) 

main.o: main.c parser.h video_selector.h
	$(CC) $(CFLAGS) -c $^ $(LFLAGS) 

parser.o: parser.c parser.h video_selector.h
	$(CC) $(CFLAGS) -c $^ $(LFLAGS)  

video_selector.o : video_selector.c video_selector.h 
	$(CC) $(CFLAGS) -c $^  $(LFLAGS) 

.PHONY: clean
clean:
	rm -rf *~
	rm -rf $(OBJECTS)
	rm main
	rm headers/*.gch

