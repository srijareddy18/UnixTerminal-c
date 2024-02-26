final: mytoolkit.c mytimeout.c
	gcc -o mytoolkit.x mytoolkit.c
	gcc -o mytimeout.x mytimeout.c

clean:
	rm mytimeout.x mytoolkit.x

mytimeout: mytimeout.c
	gcc -o mytimeout.x mytimeout.c

mytoolkit: mytoolkit.c mytimeout.c
	gcc -o mytoolkit.x mytoolkit.c
	gcc -o mytimeout.x mytimeout.c

mytree: mytree.c
	gcc -o mytree.x mytree.c

mytime: mytime.c
	gcc -o mytime.x mytime.c

mymtimes: mymtimes.c
	gcc -o mymtimes.x mymtimes.c

