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
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <ftw.h>
#include <stdint.h>

#define MAX_FDS 1024
#define SEPARATORS " \t\n"                     // token sparators
#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                             //max # args
extern char **environ;                   // environment array
const char**dstpath;

int mkdirz(char*fpath){
	return(mkdir(fpath,07777));
}

int mimic(char* srcstr,char* dststr){
	char buf[MAX_BUFFER];
	buf[0]='\0';
	if(!access(srcstr,F_OK)&&srcstr&&dststr){
		FILE* src = fopen(srcstr,"r");
		FILE* dst = fopen(dststr,"w+");
		while(fgets(buf,MAX_BUFFER,src));
		fputs(buf,dst);
		return 0;	
	}else
		return 1;
}

static int mimic_nftw_wrapper(const char*fpath,const struct stat*sb
		,int typeflag, struct FTW *ftwbuf)

{
	if(typeflag = FTW_D)
		mkdirz(strcat(dstpath,fpath+ftwbuf->base));

	else{ 
		mimic(fpath,fpath+ftwbuf->base);
	}
	return 0;
}


int main (int argc, char **argv)
{
	setbuf(stdout, NULL);
	if(argv[1]&& !access(argv[1],F_OK))
		freopen(argv[1],"r",stdin);
	int status;
	int tokenindex=0;
	char buf[MAX_BUFFER];                    //   line buffer
	buf[0]='\0';
	char *args[MAX_ARGS];               //       pointers to arg strings
	char **arg;                           //     working pointer thru args
	char *prompt = "==>" ;                  //   shell prompt
	dstpath = (char*)malloc(MAX_BUFFER*sizeof(char));

	//keep reading input until "quit" command or eof of redirected input

	while (!feof(stdin)) { 

		// get command line from input

		getcwd(buf,MAX_BUFFER);
		strcat(buf,prompt);	
		fputs (buf, stdout);                // write prompt
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
					if(args[1]&&!strcmp(args[1],"-r")){
						dstpath=strdup(args[3]);
						nftw(args[2],mimic_nftw_wrapper,MAX_FDS,0);
						continue;

					}
				}
				if (!strcmp(args[0],"morph")){ 
					if(!access(args[1],F_OK)&&args[1]&&args[2]){
						FILE* src = fopen(args[1],"r");
						FILE* dst = fopen(args[2],"w+");
						while(fgets(buf,MAX_BUFFER,src));
						fputs(buf,dst);	

					}

					continue;

				}


				if (!strcmp(args[0],"help")){


					if(!fork()){
						execlp("cat", "cat", "/projects/2/README",NULL);
						_exit(EXIT_SUCCESS);
					}else sleep(1);
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
						if(!fork()){
							execlp("ls","ls","-l",args[1],NULL);
							_exit(EXIT_SUCCESS);
						}else sleep(1);
						continue;

					}
					else{
						if(!fork()){
							execlp("ls","ls","-l",NULL);
							_exit(EXIT_SUCCESS);
						}else sleep(1);	
					}			
					continue;
				}

				// check for internal/external command

				if (!strcmp(args[0],"wipe")) { // "clear" command
					if(!fork()){
						execlp("clear","clear",NULL);
						_exit(EXIT_SUCCESS);
					}else sleep(1);
					continue;
				}

				if (!strcmp(args[0],"chdir")) {

					if(args[1]&&!access(args[1],F_OK)){
						chdir(args[1]);
						setenv("PWD",args[1],1);

					}
					else{

						if(!fork()){
							execlp("pwd","pwd",NULL);
							_exit(EXIT_SUCCESS);
						}else sleep(1);
						fputs("\n",stdout);
					}
					continue;

				}


				if (!strcmp(args[0],"esc"))   // "quit" command
					break;                     // break out of 'while' loop

				// else pass command onto OS (or in this instance, print them out)


				arg = args;
				if(!fork()){
					execvp(args[0],args);
					_exit(EXIT_SUCCESS);
				}else sleep(1);	
			}
		}
	}
}
