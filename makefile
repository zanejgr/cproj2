all:
	gcc project2.c -o project2
clean:

	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
	gcc project2.c -o project2

zformat: gcc zformat.c -o zformat
zinspect: gcc zinspect.c -o zinspect individual blocks on the virtual disk
zfilez: gcc zfilez.c -o zfilez
zmkdir: gcc zmkdir.c -o zmkdir
zrmdir: gcc zrmdir.c -o zrmdir