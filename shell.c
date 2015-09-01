#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include <linux/limits.h> // To include PATH_MAX


#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_cd(tok_t arg[]){ // PART1

  if(chdir(arg[0]) == -1){ //change directory function arg[0] for first bit of input stream
  
    printf("File Directory not found\n");

  } 
  else{
    chdir(arg[0]);
  }
  return 1;

}

void execute(char *s, char *temp[]){ //PART2
  int status;
  pid_t pid = fork();

  if(pid<0){
    printf("Error in creating child process: fork()");
    exit(0);
  }
  else if(pid == 0){
    if(execvp(s,temp)<0){
      printf("Error in executing exe within child process: execvp()");
      exit(1);
    }
  }
  else{
    while(wait(&status) != pid){} //wait for child to finish
  }
}

/*bool writeReadFiles(tok_t *t){
	tok_t *tokWork = t;
	int loop;
	char *fileName;


	for(loop =0; loop< MAXTOKS;loop++){
		if(tokWork[loop] != NULL && strcmp(tokWork[loop],">")==0){ //strcmp does comparison of strings, 0 being true
			tokWork[loop] = NULL;
			fileName = tokWork[loop+1];

			FILE *f = fopen(fileName,"a+"); //w for write, a to append, a+ open for append for update at EOF

			dup2(fileno(fileName),STDOUT_FILENO);
			fclose(fileName);
			return(true);
		}
		else if(strcmp(tokWork[loop],"<")==0){
			tokWork[loop] = NULL;
			fileName = tokWork[loop+1];
			FILE *f = fopen(fileName,"wt");
			
			dup2(fileno(fileName),STDIN_FILENO);
			fclose(fileName);
			return(true);
		}
	}
	return(false);
	
}*/


 exePath(char *s, tok_t *tokWork){ //PART3 - PART4

	char *pathTemp = getenv("PATH"); //Gets the path 
	tok_t *t = getToks(pathTemp); //Calls getToks from parse.c
	char work[PATH_MAX];
	int loop;
	int loop2;
	FILE *f;

	bool read = false, write = false;
	bool standard = true;

	char *fileName;

	bool flag = false;


	for(loop2=0;loop2<MAXTOKS;loop2++){
		if(strcmp(tokWork[loop2],">") == 0) {
			tokWork[loop2] = NULL;
			fileName = tokWork[loop2+1];


			write = true;
			standard = false;

			break; 
		}
		else if (strcmp(tokWork[loop2],"<") == 0){
			tokWork[loop2] = NULL;
			fileName = tokWork[loop2+1];

			f = fopen(fileName,"a+");

			read = true;
			standard = false;
		
			break;
		}
	}
	//printf("%d",standard);
	if(standard == true){
		EXECCALL: for(loop = 2; loop<MAXTOKS ;loop++){ //MAXTOKS assumed to be a set max token length
								if(execv(work,tokWork)<0){
									strcpy(work,t[loop]); // copies the part of path
									strcat(work,"/");//concates a "/" onto the string to complete path
									strcat(work,s); //conacates exe file name
								}
							}	
	}
	else{
		if(write == true){
      
      f = fopen(fileName,"a+");
      
			dup2(fileno(f),STDOUT_FILENO);
			goto EXECCALL;
			fclose(fileName);
		}
		else if(read == true){
			dup2(fileno(f),STDIN_FILENO);
			goto EXECCALL;
			fclose(f);

	}
	

}
}



int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_help(tok_t arg[]);


/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_cd,"cd","c change directory"}
};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

void init_shell()
{
  /* Check if we are running interactively */
  shell_terminal = STDIN_FILENO;

  /** Note that we cannot take control of the terminal if the shell
      is not interactive */
  shell_is_interactive = isatty(shell_terminal);

  if(shell_is_interactive){

    /* force into foreground */
    while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
      kill( - shell_pgid, SIGTTIN);

    shell_pgid = getpid();
    /* Put shell in its own process group */
    if(setpgid(shell_pgid, shell_pgid) < 0){
      perror("Couldn't put the shell in its own process group");
      exit(1);
    }

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }
  /** YOUR CODE HERE */
}

/**
 * Add a process to our process list
 */
void add_process(process* p)
{
  /** YOUR CODE HERE */
}

/**
 * Creates a process given the inputString from stdin
 */
process* create_process(char* inputString)
{
  /** YOUR CODE HERE */
  return NULL;
}



int shell (int argc, char *argv[]) {
  char *s = malloc(INPUT_STRING_SIZE+1);	
			/* user input string */

	bool* part4 = false;

	
	
	

  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid, cpgid;

  char cwd[1024];

  init_shell();

  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);

  lineNum=0;
  fprintf(stdout, "%d: ", lineNum);
  fprintf(stdout,"%s ", getcwd(cwd,sizeof(cwd)));//current working directory
  while ((s = freadln(stdin))){
		if(strstr(s,">") != NULL){
			part4 = true;		
		}
	else if(strstr(s,"<") != NULL){
		part4 = true;	
	}
    t = getToks(s); /* break the line into tokens */
    fundex = lookup(t[0]); /* Is first token a shell literal */
    if(fundex >= 0) cmd_table[fundex].fun(&t[1]);
    else {	
      //char *temp[] = {s,t[1],t[2]};//whole input stream - part 2
			
			
			//fprintf(stdout,"%d \n\n", part4);

			if(part4 == false){
				char *temp[] = {s,t[1],t[2]};
				execute(s,temp);	
			}
		else if (part4 == true){
			pid_t cpid = fork();

			if(cpid < 0){
				fprintf(stdout,"Failure in creating child proccess: fork()");
				exit(0);
			}
			else if(cpid == 0){
				exePath(t[0],t);
				
				exit(1);
			}
			wait(cpid); //wait for child proccess to finish	 
      //execute(s,temp); // commented out because of part 3
      //fprintf(stdout, "This shell only supports built-ins. Replace this to run programs as commands.\n");
			part4 = false;
		}
}
    fprintf(stdout, "%d: ", lineNum++);
    fprintf(stdout,"%s ", getcwd(cwd,sizeof(cwd)));
  }
  return 0;
}
