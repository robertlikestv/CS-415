/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
	//Check for NULL string
	if (buf == NULL)
	{
		return 0;
	}
	char* saveptr = NULL;
	char* token = strtok_r(buf, delim, &saveptr);
	int count = 0;
	while (token != NULL){
	    token = strtok_r(NULL, delim, &saveptr);
		count++;
	}

	//return number of tokens
	return count;
}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/

	//1
	command_line result;

	//2
	char* newline_saveptr = NULL;
	char* tokenizer_saveptr = NULL;
	char* token;
	buf = strtok_r(buf, "\n", &newline_saveptr);
	char* buf_cp = strdup(buf);

	result.num_token = count_token(buf_cp, delim);
	free(buf_cp);

	//3
	result.command_list =(char**) malloc(sizeof(char*) * (result.num_token+1));

	token = strtok_r(buf, delim, &tokenizer_saveptr);
	int tokenlen;
	//4 & 5
	for (int i = 0; i < result.num_token; i++){
		tokenlen = strlen(token);
		result.command_list[i] = (char*) malloc(sizeof(char) * (tokenlen+1));
		strcpy(result.command_list[i], token);
		token = strtok_r(NULL, delim, &tokenizer_saveptr);
	}
	result.command_list[result.num_token] = NULL;

	//6
	return result;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	
	for (int i = 0; i < command->num_token; i++){
		free(command->command_list[i]);
	}
	free(command->command_list);
}
