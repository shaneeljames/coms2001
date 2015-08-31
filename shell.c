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
#include <fcntl.h>
#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_help(tok_t arg[]);
int cmd_cd(tok_t arg[]);
int cmd_chOut(tok_t arg[]);
int cmd_chIn(tok_t arg[]);

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
  {cmd_cd, "cd", "change working directory"},
  {cmd_chOut,"[process]>[file]", "instructs process to write output to a file"},
  {cmd_chIn,"[process]<[file]","instructs process to read input from a file"}	
};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;

}

int cmd_cd(tok_t arg[]) {
 chdir(arg[0]); 
   return 1;
}
 
int cmd_chOut(tok_t arg[]){
	char* path=malloc(INPUT_STRING_SIZE) ;
	fprintf(stdout,"Enter file path: ");
	path=freadln(stdin);
	/*
	FILE* file=fopen(path,"a+");
	if(file!=NULL){
			
		if((dup2(fileno(file),fileno(stdout)))==-1);
			fprintf(stdout,"Failed\n ");
		fclose(file);
	
	}
	else*/
		fprintf(stdout,"Failed\n");
 
}

int cmd_chIn(tok_t arg[]){
	char* path=malloc(INPUT_STRING_SIZE) ;
	fprintf(stdout,"Enter file path: ");
	path=freadln(stdin);
	
	FILE* file=fopen(path,"r");
	if(file!=NULL){
			
		dup2(fileno(file),fileno(stdin));
		fclose(file);
	
	}
	else
		fprintf(stdout,"failed\n", path );
 
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
}

int shell (int argc, char *argv[]) {
  char *s = malloc(INPUT_STRING_SIZE+1);			/* user input string */
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid, cpgid;

  init_shell();

  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);
  	
  lineNum=0;int callP;

 	fprintf(stdout, "%d %s: ", lineNum,get_current_dir_name());
	s = freadln(stdin); 
  do{
    t = getToks(s); /* break the line into tokens */
    fundex = lookup(t[0]); /* Is first token a shell literal */
      if(fundex >= 0){ 
	
		cmd_table[fundex].fun(&t[1]);			
		lineNum++;
      }	
      else{	
		
		
		cpid=fork();
		if(cpid>0){	

  			int status;
			wait(&status);
			lineNum++;
		}else if(cpid==0){
			char *checkP=strchr(t[0],'/');
			if(checkP!=NULL){
			
			  if((execve(t[0],t,NULL)==-1)||(s==NULL))	
  			    fprintf(stdout, "Not a known UNIX command\n");
			
			}
			else{ 	
			  char* ePath=getenv("PATH");
			  if(ePath!=NULL)
				printf("current path: %s",ePath);
	
			}
			lineNum++;

		}
		else{
			fprintf(stdout,"Error forking");
			return -1;
		}
     }  
  fprintf(stdout, "%d %s: ", lineNum,get_current_dir_name());
  } while ((s = freadln(stdin)));


}



