objects=main.o seperator.o gspan_utils.o gspan_init.o gspan_mining.o database.o gspan_extend.o \
		gspan_count.o history.o multi_thread.o output.o\

all : gspan
CXXFLAGS= -g -O2 -pthread

gspan : $(objects)
	g++ -o gspan -pthread -g $(objects) -O2

main.o : gspan_utils.h gspan.h database.h
seperator.o : seperator.h common.h
gspan_utils.o : gspan_utils.h
gspan_init.o : gspan.h seperator.h database.h
gspan_mining.o : gspan.h database.h 
gspan_extend.o : gspan.h history.h
gspan_count.o : gspan.h history.h
database.o : database.h 
history.o : history.h
multi_thread.o : gspan.h
multi_thread_average.o : gspan.h
output.o : output.h

.PHONY : clean all

clean :
	rm -rf $(objects) gspan
