
all:	main

main:	main.c ../../renderer/libsnis-renderer.so
	$(CC) -o main -I../../renderer -L../../renderer -lsnis-renderer main.c

../../renderer/libsnis-renderer.so:
	(cd ../../renderer && make)

clean:
	rm -f main

