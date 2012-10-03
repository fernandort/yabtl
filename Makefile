all:
	gcc -o obj/yabtl_compare.o -Wall -Werror -fPIC -c src/yabtl_compare.c -I./src -O3
	gcc -o obj/yabtl_delete.o -Wall -Werror -fPIC -c src/yabtl_delete.c -I./src -O3
	gcc -o obj/yabtl_insert.o -Wall -Werror -fPIC -c src/yabtl_insert.c -I./src -O3
	gcc -o obj/yabtl_search.o -Wall -Werror -fPIC -c src/yabtl_search.c -I./src -O3
	gcc -shared -o obj/libyabtl.so obj/*.o -O3

install:
	cp obj/libyabtl.so /usr/local/lib/
	cp src/yabtl.h /usr/local/include/

clean:
	rm -f obj/*
	rm -f yabtl

test:
	gcc -o yabtl src/yabtl.c -lyabtl -O3
