

all: main.o parser.o video_selector.o
	gcc -g -Wall -W -o  main main.o parser.o video_selector.o -lavutil -lavformat -lavcodec -lswscale -lz -lm  `sdl-config --cflags --libs` 

main.o: main.c headers/parser.h headers/video_selector.h
	gcc -g -Wall -W -c main.c headers/parser.h headers/video_selector.h -lavutil -lavformat -lavcodec -lswscale -lz -lm  `sdl-config --cflags --libs` 

parser.o: headers/parser.c headers/parser.h headers/video_selector.h
	gcc -g -Wall -W -c headers/parser.c headers/parser.h headers/video_selector.h -lavutil -lavformat -lavcodec -lswscale -lz -lm  `sdl-config --cflags --libs`  

video_selector.o : headers/video_selector.c headers/video_selector.h 
	gcc -g -Wall -W -c headers/video_selector.c headers/video_selector.h  -lavutil -lavformat -lavcodec -lswscale -lz -lm  `sdl-config --cflags --libs` 

clean:
	rm -rf *~
	rm -rf *.o
	rm main
	rm headers/*.gch

