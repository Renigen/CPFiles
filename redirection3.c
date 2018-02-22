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
            printf("%s\n", cwd);
            return 1;
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
