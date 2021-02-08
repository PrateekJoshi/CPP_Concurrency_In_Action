#####################################################
# Compile : make
####################################################
CC=g++	
FLAGS+= -lpthread
SRC= two_mutex_with_deadlock_free.cc two_mutex_with_deadlock.cc

build: 
	@for file in $(SRC); do \
		bin="$$(basename -s .cc $${file})"; \
		$(CC) $${file} $(FLAGS) -o $${bin}.bin ; \
	done
	
clean: 
	rm -rf *.o
