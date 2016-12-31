#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CMD_LEN  516
#define HISTORY_COUNT 20
#define TOK_BUFSIZE 64

// global data members
int is_redir, bak;
int is_batch = 0;
char line[MAX_CMD_LEN];
char *hist[HISTORY_COUNT];
int current = 0;
signed long history_count = 0;
FILE * infile = NULL;

void throw_error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void switch_stdio()
{
	fflush(stdout);
	dup2(bak, 1);
	close(bak);
}

//split the lines read from std input into tokens
char **split_line(char *line, int *argc) 
{
	int bufsize = TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;
	if (!tokens)
	 {
		throw_error();
	}
	token = strtok(line, " \t\n") ;
	while (token != NULL)
	{
		tokens[position] = token;
		position++;
		if (position >= bufsize)
		{
			bufsize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens)
			{
				throw_error();
			}
		}
		token = strtok(NULL, " \t\n");
	}
	tokens[position] = NULL;
	if (argc)
	{
		*argc = position;
	}
	return tokens;
}

void store_history(char* line)
{
	// free the buffer element before overriding 
	free(hist[current]);
	
	// does malloc internally to allocate
	hist[current] = strdup(line);

	// current indexes into history buffer                
	current = (current + 1) % HISTORY_COUNT;
	
	// increment the history counter
	history_count++;
}

// history is implemented using circular buffer of 20 elements
void print_history(char *hist[], int current, int hist_counter)
{
	int i = current;
	int hist_num;
	if (hist_counter < HISTORY_COUNT)
		hist_num = 1;
	 else
		hist_num = hist_counter-(HISTORY_COUNT-1);
	// print the elements of the circular buffer to indicate the lastest history
	do {
		if (hist[i])
		{
			printf("%d %s", hist_num, hist[i]);// use write instead
			//write (STDOUT_FILENO, &hist_num, sizeof(hist_num));
			hist_num++;
		}
		i = (i + 1) % HISTORY_COUNT;
	}
	while (i != current);
	printf("\n");//use write
	//write(STDOUT_FILENO, "\n", strlen("\n"));
	if(is_redir)
	{	
		switch_stdio();
	}
}

// clear the buffer before exiting shell
int clear_history(char *hist[]) 
{
	int i;
	for (i = 0; i < HISTORY_COUNT; i++) 
	{
		free(hist[i]);
		hist[i] = NULL;
	}
	return 0;
}

// function declarations for builtin shell commands:
int mysh_history(char **args);
int mysh_exit(char **args);

// list of builtin commands, followed by their corresponding functions.
char *builtin_str[] = { "history", "exit"};

int (*builtin_func[]) (char **) = {&mysh_history, &mysh_exit};

//  builtin function implementations.
int mysh_history(char **args)
{
	store_history("history");
  	print_history(hist, current, history_count);
}

// exit from mysh
int mysh_exit(char **args)
{
	store_history("exit");
  	exit(0);
}

int history_operator(char* inp_str)
{
  int hist_index;
  if (inp_str)
  {
  	if (atoi(inp_str)< history_count - HISTORY_COUNT+1 || atoi(inp_str) > history_count || atoi(inp_str)<1)
  	{
		throw_error();
		if(is_redir)
		{	
			switch_stdio();
		}
		return 1;
	}
	else
	{	
		if (history_count<= HISTORY_COUNT)
			hist_index = atoi(inp_str)-1;
		else
			hist_index = (atoi(inp_str)%HISTORY_COUNT)-1;
		strncpy(line,hist[hist_index],MAX_CMD_LEN);
		

		if (redirection_check()==1)
		return 1;

		char** args = split_line(strdup(line),NULL);
		if (strcmp(args[0],"history")==0 && args[1] == NULL)
		{
			mysh_history(args);
			return 1;			
		}
		else 
		{
			mysh_launch(args);
			return 1;
		}
				
	}
  }
  return 0;
}

// execute the command given to shell
void mysh_execute(char **args, int argc)
{
  int i,hist_index;
  // return if no argument is provided
  if (args[0]== NULL)
	return;

  if (argc == 1	) 
  {
  	// check for builtin functions  
	for (i = 0; i < (sizeof(builtin_str)/sizeof(char*)); i++)
  	{
  		if (strcmp(args[0], builtin_str[i]) == 0)
		{
    			(*builtin_func[i])(args);
			return;
		}
  	}

		

	// check for ! history operator
	if (strcmp(args[0],"!")==0) //case when !
	{
		if (history_count == 0)
		{
			throw_error();
			return;
		}
		if (current == 0)
			hist_index = HISTORY_COUNT-1;
		else		
			hist_index = current-1;
	
		strncpy(line,hist[hist_index],MAX_CMD_LEN);
		args = split_line(strdup(line),NULL);
		if (strcmp(args[0],"history")==0 && args[1] == NULL)
		{
			mysh_history(args);
			return;			
		}
	}
	else if (memcmp(args[0], "!", 1)==0) //case when[!][number]
	{
		args[0] =  strtok(args[0],"!");
		if (history_operator(args[0]))
		{
			return;
		}
	}
  }
  else if (argc == 2)
  {
	if (strcmp(args[0],"history") == 0  || strcmp(args[0],"exit") == 0)
	{	throw_error();
		return;
	}
	if (strcmp(args[0],"!")==0 && atoi(args[1]) < 1 )
	{
		throw_error();
		return;
	}

  	//case when [!][space][number]
	if (strcmp(args[0],"!")==0 && atoi(args[1])>=1 )
	{
		if (history_operator(args[1]))
		{
			return;
		} 		
	}	
  }
store_history(line);
mysh_launch(args);
}

// launching the child process to executing the command
int mysh_launch(char **args)
{
	int pid;
	pid = fork();
	if (pid == 0)
	{
     		if (execvp(args[0], args) < 0)
		{
			//printf("%s: command not found\n", args[0]);
			throw_error();
			if(is_redir)
			{	
				switch_stdio();
			}			
			exit(1);
		}
	}
	else if (pid < 0)
	{	
		// error forking
		throw_error();
	}
	else
	{
 		// parent process
     		wait(&pid);
		if(is_redir)
		{	
			switch_stdio();
		}
	}
 	return 1;
}

int redirection_check()
{
	char* pre_token;
	char* post_token;
	char** pre_args;
	char** post_args;
	int pre_arg_count=0;
	int post_arg_count=0;
	int new, err=0;
	
	pre_token = strtok(strdup(line), ">");
    	if (strlen(pre_token) != strlen(line)) 
	{
      		is_redir = 1;
      		post_token = strtok(NULL, ">");
      		if (strtok(NULL, ">")) 
		{
        		// too many redirects
        		throw_error();
			return 0;
      		}
		else
		{
			post_args = split_line(strdup(post_token),&post_arg_count);
			if (post_arg_count != 1)
			{
				throw_error();
				return 0;
			}
			else
			{
				fflush(stdout);
				bak = dup(1);
				new = open(post_args[0], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
				if (new < 0) 
				{
					throw_error();
					return 0;
			 	}
				dup2(new, 1);
				close(new);
			}			
			//if (!err)
			//{
				// splitting the line into tokens
   				pre_args = split_line(strdup(pre_token),&pre_arg_count);

				// execute the command
				if (strcmp(pre_args[0],"!")==0 )
					{
						throw_error();
						return 0;	
					}

    				mysh_execute(pre_args,pre_arg_count);
				return 1;
			//}	
		}
 	 }
}

// shell loop for constant prompt until exit
void mysh_loop(void)
{
  int arg_count;	
  char **args = NULL;
  char* status = NULL;
  do 
  {
	is_redir = 0;
  	// printing shell prompt
        if (!is_batch)
	{    	
		write(STDOUT_FILENO, "mysh # ", 7);
	}
    	
	// get the input string; from the std input
    	status = fgets(line, MAX_CMD_LEN, infile);

	if(status==NULL)
	exit(0);

	if (is_batch) { 
	     if(strlen(line) > 513)
		{
			throw_error();
			line[513]= '\n';
			write(STDOUT_FILENO, line  , 512);
			write(STDOUT_FILENO, "\n", 1);
			continue ;
		}
		else
		write(STDOUT_FILENO, line, strlen(line));

	}


	// check for redirection
	redirection_check();
    	
	if (!is_redir)
	{
		// splitting the line into tokens
   		args = split_line(strdup(line),&arg_count);
    		// execute the command
    		mysh_execute(args,arg_count);
	}
    	free(args);
  } while (status);
}

int main(int argc, char* argv[]) 
{
	int i;
	for (i = 0; i < HISTORY_COUNT; i++)
		hist[i] = NULL;
	if (argc == 1)
    		infile = stdin;	
	// enter batch mode  	
	else if (argc == 2) 
	{	
    		is_batch = 1;
    		infile = fopen(argv[1], "r");
    		if (infile == NULL) 
		{
        		throw_error();
      			exit(1);
    		}
       }
       else 
       {
    		throw_error();
    		exit(1);
       }

	mysh_loop();
	
	// clear history buffer
	clear_history(hist);
	return 0;
}
