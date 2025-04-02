/*
CS 415 - Proj 2
Robert Wilson
Pt.4
				*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


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
			sigaddset(&set, SIGCONT);
			sigaddset(&set, SIGALRM);
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
					//printf("Child %d waiting\n", pid_count);
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
			/*
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
			*/
			
			//while process left {
				//alarm(1);
				//sigwait(); //with SIGALRM
				//swap processes
			//}


			/*
			//how to check if child is still running
			if(waitpid(pid_ary[i], &wstatus, WNOHANG) != 0)
			{
				//if the process has changed state
				if(WIFEXITED(wstatus))
				{
					//if the process changed state is exited
				}
				else
				{
					//if the process changed state is not exited
				}
			}
			*/

			//PART 3
			//create a bool array to see which processes are running still
			int active_processes_arr[pid_count];
			for (int i = 0; i < pid_count; i++){
				//set 1 equal to active
				active_processes_arr[i] = 1;
			}
			//track number of processes running
			int num_processes_running = pid_count;
			//track current process
			int current_process = 0;
			//track status of processes with flag
			int wstatus;
			//stat buffer for later (part 4)
			command_line stats_buff; 

			while (num_processes_running > 0)
			{
				//printf("Readying process %d (Checking if alive)\n", current_process);
				//if process alive
				if (active_processes_arr[current_process] == 1){

					//printf("Process %d alive\n", current_process);
					//set alarm & wait

					kill(pid_ary[current_process], SIGCONT);
					alarm(1);
					sigwait(&set, &sig);

					//stop process
					kill(pid_ary[current_process], SIGSTOP);

					//check if child is still alive
					if(waitpid(pid_ary[current_process], &wstatus, WNOHANG) != 0)
					{
					
						//if the process has changed state (aka has exited)
						if(WIFEXITED(wstatus))
						{
							//make array index false
							if (active_processes_arr[current_process] == 1){
								printf("FINISHED: Process %d finished, removing from queue\n", current_process);
								active_processes_arr[current_process] = 0;
								//processes_running--
								num_processes_running--;
							}
						}
						/* 
						else
						{
							//if the process changed state is not exited
							//change to process
						}*/
					}
				}
				//change to next process
				current_process = (current_process + 1) % pid_count;

				//PART 4
				if (current_process == (pid_count - 1))
				{
					//formatting output
					printf("PID\t utime\t stime\t time\t nice\t virt mem\n");

					for (int i = 0; i < pid_count; i++)
					{
						//open stats file for process
						FILE *proc;
						char proc_buffer[1024];
						char proc_name[50];
						sprintf(proc_name, "/proc/%d/stat", pid_ary[i]);

						if (active_processes_arr[i]){
							proc = fopen(proc_name, "r");

							//if file open fails give error
							if (proc == NULL){
								fprintf(stderr, "Failed to open %s\n", proc_name);
								return 1;
							}

							while(fgets(proc_buffer, sizeof(proc_buffer), proc) != NULL)
							{
								//utime - 14, stime - 15, time (utime + stime) , nice - 19, virt mem - 23
								//printf("%s\n", proc_buffer);
								stats_buff = str_filler(proc_buffer, " ");
								printf("%s - ", stats_buff.command_list[0]);
								printf("%s\t", stats_buff.command_list[13]);
								printf("%s\t", stats_buff.command_list[14]);
								int utime = atoi(stats_buff.command_list[13]);
								int stime = atoi(stats_buff.command_list[14]);

								printf("%d\t", (utime + stime));
								printf("%s\t", stats_buff.command_list[18]);
								printf("%s\n", stats_buff.command_list[22]);
								free_command_line(&stats_buff);
							}

							fclose(proc);
						}
						
					}
					
				}
			}
			

			while (wait(NULL) > 0);
			
			//free_command_line(&stats_buff);
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