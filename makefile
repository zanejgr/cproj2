all:
	gcc project2.c -o project2
clean:

	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
	gcc project2.c -o project2

