/*
 * Tech Shell
 * Nicholas Jones
 * 2/20/2018
 */

#define STR_BUFFER 129
#define TOK_BUFFER 4
#define CWD_BUFFER 1024

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Globals
char cwd[CWD_BUFFER];
char str[STR_BUFFER];

/*
 * Helpers
 */

// Return array length
int argl(char** args) {
	char* 	arg = args[0];
	int 	len = 0;

	while (arg != NULL) {
		arg = args[++len];
	}

	return len;
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
	printf("%s", prompt);

	fgets(str, STR_BUFFER, stdin);
	str[strcspn(str, "\n")] = 0; // remove newline

	return str;
}

// Split line by whitespace
char** tokenize(char* line) {
	int 	buffer 	= TOK_BUFFER;
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
			buffer += TOK_BUFFER;
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

/*
 * Fork child process
 */
int startproc(char** args) {
	pid_t 	pid;
	pid_t 	wpid;

	int 	status;
	int	oindex;
	int	iindex;

	pid = fork();
	if (pid == 0) {
		// child process

		// check for redirect
		if ((oindex = contains(args, ">")) != -1) {
			oredirect(args, oindex);
		}

		if ((iindex = contains(args, "<")) != -1) {
			iredirect(args, iindex);
		}

		// run command
		if (execvp(args[0], args) == -1)
			perror("child proc");

		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// fork failed
		perror("fork");
		exit(EXIT_FAILURE);
	} else {
		// parent process
		/*do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));*/

		if ((wpid = waitpid(pid, &status, WUNTRACED)) == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
	}

	return 1;
}

/*
 * Built-ins
 */

// Print cwd
int pwd(void) {
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd()");
		return -1;
	} else {
		printf("%s\n", cwd);
		return 1;
	}
}

// Change cwd
int cd(char* arg) {
	if (chdir(arg) != 0) {
		perror("chdir()");
		return -1;
	} else {
		pwd();
		return 1;
	}
}

/*
 * Execute command
 */
int execute(char** args) {
	// empty command
	if (args[0] == NULL)
		return -1;

	// built-ins
	if (strcmp(args[0], "exit") == 0)
		return 0;

	if (strcmp(args[0], "pwd") == 0)
		return pwd();

	if (strcmp(args[0], "cd") == 0)
		if (args[1] == NULL)
			return cd("/home");
		else
			return cd(args[1]);

	// new process
	return startproc(args);
}

int main(void) {
	char** 	args;
	char*	line;
	int	status;

	do {
		line = input(">> ");
		args = tokenize(line);

		//printf("%d\n", argl(args));

		status = execute(args);

	} while (status);
}
