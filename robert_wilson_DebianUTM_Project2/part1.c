/*
CS 415 - Proj 2
Robert Wilson
Pt.1 
				*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"
#include <unistd.h>


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

			//loop until the file is over
			while (getline (&line_buf, &len, inFPtr) != -1)	
			{
				//tokenize line buffer
				//large token is seperated by " "
				large_token_buffer = str_filler (line_buf, " ");
				
				//create child to run command
				pid_t pid;

				pid = fork();

				if (pid < 0)
				{
					fprintf(stderr, "Fork failed\n");
					return -1;
				} 
				else if (pid == 0) 
				{
					//child process
					//run given command
					//printf("Child process running\n");
					if (execvp(large_token_buffer.command_list[0], large_token_buffer.command_list) == -1){
						fprintf(stderr, "Execvp: No such file or directory\n");
						free(line_buf);
						fclose(inFPtr);
						free_command_line(&large_token_buffer);
						exit(-1);
					}

				} 
				
				free_command_line(&large_token_buffer);

			}
			while (wait(NULL) > 0);
			
			free (line_buf);
			fclose(inFPtr);
			//free line buffer
			//free (line_buf);
			//free_command_line(&large_token_buffer);

			exit(0);
			//fprintf(stdout, "End of file\n");
			//fprintf(stdout, "Bye bye!\n");

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