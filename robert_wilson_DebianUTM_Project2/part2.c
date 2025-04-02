/*
CS 415 - Proj 2
Robert Wilson
Pt.2
				*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"
#include <unistd.h>
#include <signal.h>


int main(int argc, char* const argv[])
{
	
	//if -f, open in file mode
	int flag = 0;
	if (argc > 1)
	{
		if (strcmp(argv[1], "-f") == 0)
		{
			//must accept file name
			flag++;

			//checking for command line argument
			if (argc != 3)
			{
				fprintf(stderr, "Usage ./part1 -f input.txt\n");
			}
			

			/*NOTE: THIS IS LAB 1'S MAIN FUNCTION
			EDITED FOR THIS PROJECT*/

			//opening file to read
			FILE *inFPtr;
			inFPtr = fopen(argv[2], "r");

			//declear line_buffer
			size_t len = 0;
			char* line_buf = NULL;

			command_line large_token_buffer;

			//PART 2
			//initialize signal set
			sigset_t set;
			int sig;

			sigemptyset(&set);
			//add sigusr1 to signal set
			sigaddset(&set, SIGUSR1);
			sigprocmask(SIG_BLOCK, &set, NULL);
			
			int pid_ary_len = 30;
			pid_t *pid_ary = (pid_t *) malloc(sizeof(*pid_ary) * pid_ary_len);
			int pid_count = 0;

			//loop until the file is over
			while (getline (&line_buf, &len, inFPtr) != -1)	
			{
				//tokenize line buffer
				//large token is seperated by " "
				large_token_buffer = str_filler (line_buf, " ");

				pid_ary[pid_count] = fork();
				

				if (pid_ary[pid_count] < 0)
				{
					fprintf(stderr, "Fork failed\n");
					return -1;
				} 
				else if (pid_ary[pid_count] == 0) 
				{
					//child process
					//run given command
					//printf("Child process running\n");
					sigwait(&set, &sig);
					if (execvp(large_token_buffer.command_list[0], large_token_buffer.command_list) == -1){
						fprintf(stderr, "Execvp: No such file or directory\n");
						free(pid_ary);
						free(line_buf);
						fclose(inFPtr);
						free_command_line(&large_token_buffer);
						exit(-1);
					}

				} 			
				free_command_line(&large_token_buffer);
				pid_count++;
			}
			printf("All children waiting\n");
			sleep(3);
			for (int i = 0; i < pid_count; i++){
				kill(pid_ary[i], SIGUSR1);
			}
			printf("All children started\n");
			sleep(3);
			for (int i = 0; i < pid_count; i++){
				kill(pid_ary[i], SIGSTOP);
			}
			printf("All children stopped\n");
			sleep(3);
			for (int i = 0; i < pid_count; i++){
				kill(pid_ary[i], SIGCONT);
			}
			printf("All children continued\n");

			while (wait(NULL) > 0);
			
			free(pid_ary);
			free(line_buf);
			fclose(inFPtr);

			exit(0);

		/*
		END OF LAB 1 MAIN
		*/

		}
	}

	//if no flag, give arg error
	if (flag == 0){
		fprintf(stderr, "Error: No \"-f\" flag\n");
	}

	return 0;
}