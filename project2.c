/*strtokeg - skeleton shell using strtok to parse command line

usage:

./a.out

reads in a line of keyboard input at a time, parsing it into
tokens that are separated by white spaces (set by #define
SEPARATORS).

can use redirected input

if the first token is a recognized internal command, then that
command is executed. otherwise the tokens are printed on the
display.

internal commands:

wipe - clears the screen

esc - exits from the program
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>


#define SEPARATORS " \t\n"                     // token sparators
#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                             //max # args

extern char **environ;                   // environment array
int main (int argc, char *argv)
{
	setbuf(stdout, NULL);
	if(argv[1]&& !access(argv[1],F_OK))
		freopen(argv[1],"r",stdin);


	char buf[MAX_BUFFER];                    //   line buffer
	buf[0]='\0';
	char *args[MAX_ARGS];               //       pointers to arg strings
	char **arg;                           //     working pointer thru args
	char *prompt = "==>" ;                  //   shell prompt

	//keep reading input until "quit" command or eof of redirected input

	while (!feof(stdin)) { 

		// get command line from input

		fputs (prompt, stdout);                // write prompt
		if (fgets (buf, MAX_BUFFER, stdin )) { // read a line

			// tokenize the input into args array

			arg = args;
			*arg++ = strtok(buf,SEPARATORS);   // tokenize input
			while ((*arg++ = strtok(NULL,SEPARATORS)));
			// last entry will be NULL 

			if (args[0]) {                     // if there's anything ther
				//erase
				if (!strcmp(args[0],"erase")){
					if(args[1])	remove(args[1]);
					continue;
				}
				if (!strcmp(args[0],"mimic")){ 
					if(!access(args[1],F_OK)&&args[1]&&args[2]){
						FILE* src = fopen(args[1],"r");
						FILE* dst = fopen(args[2],"w+");
						while(fgets(buf,MAX_BUFFER,src));
						fputs(buf,dst);	

					}

					continue;

				}
				if (!strcmp(args[0],"morph")){ 
					if(!access(args[1],F_OK)&&args[1]&&args[2]){
						FILE* src = fopen(args[1],"r");
						FILE* dst = fopen(args[2],"w+");
						while(fgets(buf,MAX_BUFFER,src));
						fputs(buf,dst);	

						remove(args[1]);
					}

					continue;

				}


				if (!strcmp(args[0],"help")){

					system("cat /projects/1/README");
					continue;
				}
				//environ
				//
				if (!strcmp(args[0],"environ")){
					char ** env = environ;

					while (*env) printf("%s\n",*env++);  
					// step through environment

					continue;
				}
				if(!strcmp(args[0],"ditto")){//ditto command
					arg = &args[1];
					while (*arg) printf("%s ",*arg++);	

					continue;
				}
				if (!strcmp(args[0],"filez"))
				{
					if(args[1]&&!access(args[1],F_OK))
					{
						system(strcat(args[1],"ls -l "));
					}
					else{
						system("ls -l");
					}			
					continue;
				}

				// check for internal/external command

				if (!strcmp(args[0],"wipe")) { // "clear" command
					system("clear");
					continue;
				}

				if (!strcmp(args[0],"chdir")) {

					if(args[1]&&!access(args[1],F_OK)){
						chdir(args[1]);
						setenv("PWD",args[1],1);

					}
					else{

						system("pwd");	
						fputs("\n",stdout);
					}
					continue;

				}


				if (!strcmp(args[0],"esc"))   // "quit" command
					break;                     // break out of 'while' loop

				// else pass command onto OS (or in this instance, print them out)


				arg = args;
				while (*arg) system(*arg++);
			}
		}
	}
}
