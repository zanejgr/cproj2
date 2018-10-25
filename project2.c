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

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 500

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdint.h>

#define MAX_FDS 1024
#define SEPARATORS " \t\n"                     // token sparators
#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                             //max # args

extern char **environ;                   // environment array
char*srcpath;
char*dstpath;
int erase_nftw_wrapper(const char*fpath,const struct stat*sb
		,int typeflag,struct FTW *ftwbuf){
	return(remove(fpath));
}
int mkdirz(char*fpath){
	return(mkdir(fpath,07777));
}

int mimic(char* srcstr,char* dststr){
	char buff[MAX_BUFFER];
	buff[0]='\0';
	if(!access(srcstr,F_OK)){
		FILE* src = fopen(srcstr,"r");
		FILE* dst = fopen(dststr,"w+");
		while(fgets(buff,MAX_BUFFER,src))
			fputs(buff,dst);
		return 0;	
	}else
		perror("invalid file in mimic");
		return 1;

}
static int mimic_nftw_wrapper(const char*fpath,const struct stat*sb
		,int typeflag, struct FTW *ftwbuf)

{
	char buf[MAX_BUFFER];
	char tmp[MAX_BUFFER];
	strncpy(tmp,dstpath,MAX_BUFFER);
	strcat(tmp,fpath+strlen(srcpath));
	strncpy(buf,fpath,MAX_BUFFER);
	switch(typeflag){	
		case FTW_D:
		mkdirz(tmp);
		puts("Directory in nftw\ntmp=");
		puts(tmp);
		puts("buf=");
		puts(buf);
		puts("fpath");
		puts(fpath);
		break;

		case FTW_F:
		mimic(buf,tmp);
		
		puts(tmp);
		puts("buf=");
		puts(buf);
		puts("File in nftw");
		break;

		default:
		perror("invalid file in directory");
		
		return 1;

	}
	return 0;
}


int main (int argc, char **argv)
{
	int isBatchFile = 0;
	setbuf(stdout, NULL);
	if(argv[1]&& !access(argv[1],F_OK)){
		freopen(argv[1],"r",stdin);
		isBatchFile = 1;
	}

	int status;
	int tokenindex=0;

	char buf[MAX_BUFFER];                    //   line buffer
	buf[0]='\0';
	char *args[MAX_ARGS];               //       pointers to arg strings
	char **arg;                           //     working pointer thru args
	char *prompt = "==>" ;                  //   shell prompt

	int fd;

	dstpath = (char*)malloc(MAX_BUFFER*sizeof(char));
	srcpath = (char*)malloc(MAX_BUFFER*sizeof(char));

	//keep reading input until "quit" command or eof of redirected input

	while (!feof(stdin)) { 


		// get command line from input


		getcwd(buf,MAX_BUFFER);
		strcat(buf,prompt);	
		fputs (buf, stdout);                // write prompt
		
		if (fgets (buf, MAX_BUFFER, stdin )) { // read a line

			if(isBatchFile){puts(buf);}
			// tokenize the input into args array

			arg = args;
			*arg++ = strtok(buf,SEPARATORS);   // tokenize input
			while ((*arg++ = strtok(NULL,SEPARATORS)));
			// last entry will be NULL 

			if (args[0]) {                     // if there's anything ther
				//erase
				if (!strcmp(args[0],"erase")||!strcmp(args[0],"rmdirz")){
					if(args[1])	remove(args[1]);
					continue;
				}
				if (!strcmp(args[0],"mkdirz")){
					if(args[1])	mkdirz(args[1]);
					continue;
				}
				if (!strcmp(args[0],"mimic")){
					if(args[1]&&!strcmp(args[1],"-r")&&args[2]&&args[3]){
						dstpath=strncpy(dstpath,args[3],MAX_BUFFER);
						srcpath=strncpy(srcpath,args[2],MAX_BUFFER);
						nftw(args[2],mimic_nftw_wrapper,1,0);

					}
					else if(args[1]&&args[2]){
						char tmp[MAX_BUFFER];
						strncpy(tmp,args[1],MAX_BUFFER);
						strncpy(buf,args[2],MAX_BUFFER);
						mimic(tmp,buf);
					}
					continue;
				}
				if (!strcmp(args[0],"morph")){ 

					if(args[1]&&!strcmp(args[1],"-r")){
						dstpath=strncpy(dstpath,args[3],MAX_BUFFER);
						srcpath=strncpy(srcpath,args[2],MAX_BUFFER);
						nftw(args[2],mimic_nftw_wrapper,1,0);

	nftw(args[2],mimic_nftw_wrapper,MAX_FDS,0);

						nftw(args[2],erase_nftw_wrapper,MAX_FDS,FTW_DEPTH);
					}
					else if(args[1]&&args[2]){
						char tmp[MAX_BUFFER];
						strncpy(tmp,args[1],MAX_BUFFER);
						strncpy(buf,args[2],MAX_BUFFER);
						mimic(tmp,buf);
						if(!access(args[1],F_OK)) remove(args[1]);
						else	perror("something's really really wrong!");
					}
					continue;	
				}



				if (!strcmp(args[0],"help")){


					if(!fork()){
						execlp("cat", "cat", "/projects/2/README",NULL);
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
						}else wait(0);
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
