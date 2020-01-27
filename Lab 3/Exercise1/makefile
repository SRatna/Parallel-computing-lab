#LEVEL = sasquatch-III-1.txt
LEVEL = level.txt
#LEVEL = sasquatch-IV-4.txt
#LEVEL = sasquatch-III-5.txt
#LEVEL = sasquatch-III-3.txt
#LEVEL = sasquatch-IV-7.txt
#LEVEL = original-13.txt

COPTS   = -g -O4 -fopenmp
GPP     = g++

HEADERS = converter.h playfield.h config.h bfsqueue.h dfsstack.h \
		  dfsdepthmap.h
SOURCES = sokoban.cpp $(HEADERS:.h=.cpp)

all: sokoban

sokoban: $(SOURCES) $(HEADERS) makefile
	$(GPP) $(COPTS) -o sokoban $(SOURCES)

run: sokoban
	./sokoban LEVELS/$(LEVEL) $(DEPTH)

test: sokoban
	./sokoban LEVELS/$(LEVEL) $(DEPTH) 2> /tmp/sokoban.out
	@diff LEVELS/$(LEVEL:.txt=.out.txt) /tmp/sokoban.out > /tmp/sokoban.diffs;\
	if [ "$$?" = "0" ];\
	then \
		echo;\
		echo OK;\
	else \
		echo;\
		echo '!!! FAILED !!!';\
		echo 'Differences:';\
		cat /tmp/sokoban.diffs;\
	fi

clean:
	rm -f sokoban *.o *~ LEVELS/*~
