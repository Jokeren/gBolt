objects=main.o seperator.o gspan_utils.o gspan_init.o gspan_mining.o database.o gspan_extend.o \
		gspan_count.o history.o\

all : gspan

gspan : $(objects)
	g++ -o gspan -g $(objects) -O2

main.o : gspan_utils.h gspan.h database.h
	g++ -c -g main.cc
seperator.o : seperator.h common.h
	g++ -c -g seperator.cc
gspan_utils.o : gspan_utils.h
	g++ -c -g gspan_utils.cc
gspan_init.o : gspan.h seperator.h database.h
	g++ -c -g gspan_init.cc
gspan_mining.o : gspan.h database.h 
	g++ -c -g gspan_mining.cc
gspan_extend.o : gspan.h history.h
	g++ -c -g gspan_extend.cc
gspan_count.o : gspan.h history.h
	g++ -c -g gspan_count.cc
database.o : database.h 
	g++ -c -g database.cc
history.o : history.h
	g++ -c -g history.cc

.PHONY : clean all

clean :
	rm -rf $(objects) gspan
