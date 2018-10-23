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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

#define SEPARATORS " \t\n"                     // token sparators
#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                             //max # args

extern char **environ;                   // environment array
int main (int argc, char **argv)
{
	setbuf(stdout, NULL);
	if(argv[1]&& !access(argv[1],F_OK))
		freopen(argv[1],"r",stdin);
	char buf[MAX_BUFFER];                    //   line buffer
	buf[0]='\0';
	char *args[MAX_ARGS];               //       pointers to arg strings
	char **arg;                           //     working pointer thru args
	char *prompt = "==>" ;                  //   shell prompt
	int fd;

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

					}

					continue;

				}


				if (!strcmp(args[0],"help")){


					if(!fork()){
						execlp("cat", "cat", "/projects/1/README",NULL);
						_exit(EXIT_SUCCESS);
					}else wait(0);
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
						}else wait(0);
						continue;

					}
					else{
						if(!fork()){
							execlp("ls","ls","-l",NULL);
							_exit(EXIT_SUCCESS);
						}else wait(0);	
					}			
					continue;
				}

				// check for internal/external command

				if (!strcmp(args[0],"wipe")) { // "clear" command
					if(!fork()){
						execlp("clear","clear",NULL);
						_exit(EXIT_SUCCESS);
					}else wait(0);
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


				if(!fork()){
					arg = &args[0];
					while(*arg++){
						if(*arg&&**arg&&!strcmp(*arg,"<")){
							*arg=NULL;
							fd=open(*(++arg),O_SYNC|O_RDONLY,S_IRWXU|S_IXUSR|S_IRWXO);
							dup2(fd,STDIN_FILENO);
							close(fd);

						}
						if(*arg&&**arg&&!strcmp(*arg,">")){
							*arg=NULL;
							fd=open(*(++arg),O_SYNC|O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU|S_IXUSR|S_IRWXO);
							dup2(fd,STDOUT_FILENO);
							close(fd);
						}
						if(*arg&&**arg&&!strcmp(*arg,">>")){
							*arg=NULL;
							fd=open(*(++arg),O_SYNC|O_WRONLY|O_CREAT|O_APPEND,S_IRWXU|S_IXUSR|S_IRWXO);
							dup2(fd,STDOUT_FILENO);
							close(fd);
						}
					}
					arg = &args[0];
					execvp(args[0],args);
				}else wait(0);	
				fflush(stdin);
				continue;
			}
		}
	}
}
