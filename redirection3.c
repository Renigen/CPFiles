/*
** Splice Shell
** Kristian Leopold
** 2/21/2018
*/

//Global Variables
//String, Tokenize, and Directory respectively
#define S_BUFFER 129
#define T_BUFFER 4
#define D_BUFFER 1024

//Header Files
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//Global Arrays
char cwd[D_BUFFER];
char string[S_BUFFER];



//returns length of array
int lenArgs(char** args) {
	char* 	arg = args[0];
	int 	length = 0;

	while (arg != NULL) {
		arg = args[++length];
	}

	return length;
}


// Scan array for str and return index
int contains(char** args, char* str) {
	char* 	arg = args[0];
	int	pos = 0;

	while (arg != NULL) {
		if (strcmp(arg, str) == 0)
			return pos;

		arg = args[++pos];
	}

	return -1;
}

// Display prompt then read input line
char* input(char* prompt) {
	printf("%s ~ ", cwd);

	fgets(str, S_BUFFER, stdin);
	str[strcspn(str, "\n")] = 0; // remove newline

	return str;
}

char** tokenize(char* line) {
	int 	buffer 	= T_BUFFER;
	int 	pos 	= 0;
	char** 	tokens	= malloc(buffer * sizeof(char*)); // allocate new array
	char* 	token;

	if (!tokens) {
		fprintf(stderr, "malloc error\n");
		exit(EXIT_FAILURE);
	}

	// split string by whitespace
	token = strtok(line, " ");
	while (token != NULL) {
		tokens[pos] = token;
		pos++;

		// array is bigger than buffer, reallocate
		if (pos >= buffer) {
			buffer += T_BUFFER;
			tokens = realloc(tokens, buffer * sizeof(char*));

			if (!tokens) {
				fprintf(stderr, "malloc error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " ");
	}

	tokens[pos] = NULL;
	return tokens;
}

// Redirect output
void oredirect(char** args, int oindex) {
	int fp = open(args[oindex + 1],
			O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

	if (dup2(fp, STDOUT_FILENO) == -1)
		perror("dup2 out");

	close(fp);

	args[oindex] = NULL;
}

// Redirect input
void iredirect(char** args, int iindex) {
	int fp = open(args[iindex + 1], O_RDONLY);

	if (dup2(fp, STDIN_FILENO) == -1)
		perror("dup2 in");

	close(fp);

	args[iindex] = NULL;
}

//recomment and revisit
int splice(char** args)
{
    pid_t   pid1;
    pid_t   pid2;

    int status;
    int oindex; //change to outIndex, just an o is confusing
    int iindex; //change to inIndex, just an i is confusing

    pid1 = fork();
    if (pid1 == 0)
    {
		// child process

		// check for redirect
		if ((oindex = contains(args, ">")) != -1)
        {
			oredirect(args, oindex);
		}

		if ((iindex = contains(args, "<")) != -1)
        {
			iredirect(args, iindex);
		}

		// run command
		if (execvp(args[0], args) == -1)
			perror("child proc");

		exit(EXIT_FAILURE);
	}
    else if (pid1 < 0)
    {
		//fork failed if pid1<0
		printf("Your splice has failed.");
        //Friend informed me that exit(1) might not port properly
		exit(EXIT_FAILURE);
	}
    else
    {
		// parent process
		if ((pid2 = waitpid(pid1, &status, WUNTRACED)) == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
	}

	return 1;
}

//if doesn't work, try printcwd(void)
int printcwd()
{
    if(getcwd(cwd, sizeof(cwd)) == NULL)
    {
        printf("Current working directory does not exist.");
        return -1;
    }

    else
    {
        printf("%s\n", cwd);
        return 1;
    }
}

//runs chdir, then either prints the new directory or responds to the error
int cd(char* arg)
{
    //if chdir returns an error
    if (chdir(arg) != 0)
    {
        printf("Directory not found.")
        return -1
    }
    else
    {
        if(getcwd(cwd, sizeof(cwd)) == NULL)
        {
            printf("Current working directory does not exist.");
            return -1;
        }

        else
        {
            printcwd();
            return 1;
        }
    }
}

//change to exec, execute seems a bit long
int execute(char** args)
{
    //If no arguments are provided
    if (args[0] == NULL)
    //Not zero, does not end main loop
        return -1;

    //Required built-in commands
    //Exiting the program, just return an int that will cause the main loop to stop (so 0)
    if (strcmp(args[0], "exit") == 0)
        return 0;

    if (strcmp(args[0], "cd") == 0)
        //If there is no directory given after cd, you return to home
        //There should be a directory given otherwise
        if (args[1] == NULL)
            return cd("/home");
        else
            return cd(args[1]);

    if (strcmp(args[0], "pwd") == 0)
    {
        //If current working directory does not exist
        if(getcwd(cwd, sizeof(cwd)) == NULL)
        {
            printf("Current working directory does not exist.");
            return -1;
        }

        else
        {
            return printcwd()
        }
    }
    //Start a new process
    return splice(args);

}

//Someone told me int main(void) is better, but I don't think it matters here.
//I also don't know the difference.
int main()
{
    char**  args;
    char*   line;
    int     status;
    getcwd(cwd, sizeof(cwd));

    do
    {

        //Want to get the current directory before and during the loop.
        getcwd(cwd, sizeof(cwd));
        line = input(" ~ ");
        args = tokenize(line);
        status = execute(args);

    } while(status);
}
