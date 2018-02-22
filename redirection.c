#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/wait.h>

char str[129];
enum {NOT_FOUND=0,FOUND};
enum {false=0,true};
static char *ptr;
const char *del;
int ReadLine(char *, int , FILE *);







char *mystrtok(char* string,const char *delim)
{
  int j,flag=NOT_FOUND;
  char *p;
  if(string != NULL)
    {
      ptr=string;
      p=string;
    }
  else
    {
      if(*ptr == '\0')
	return NULL;

      p=ptr;
    }

  while(*ptr != '\0')
    {
      del=delim;
      while(*del != '\0')
        {
	  if(*ptr == *del)
            {
	      if(ptr == p)
                {
		  p++;
		  ptr++;
                }
	      else
                {
		  *ptr='\0';
		  ptr++;

		  return p;
                }
            }
	  else
            {
	      del++;
            }
        }
      ptr++;
    }
  return p;
}


int splice()
{
  pid_t  pid;
  /* fork another process */
  pid = fork();
  if (pid < 0) { /* error occurred */
    fprintf(stderr, "Fork Failed");
    exit(-1);
  }
  else if (pid == 0) { /* child process */
    execlp("bin/ls", "ls", NULL);
  }
  else { /* parent process */
    /* parent will wait for the child to complete */
    printf ("\n Child Made\n");
    wait (NULL);
    printf ("\nChild Complete\n");
    exit(0);
  }
}


int main()
{
  int i;
  char *p_str,*token;
  char delim[10];
    delim[0] = ' ';
    delim[1] = '\t';
    delim[2] = '\n';
    delim[3] = '\0';
  char cwd[1024];
  char *tok[129];

  while(1)
  {
    tok[0] = NULL;
    fflush(stdin);
    fflush(stdout);
    printf("\n Enter a string to tokenize: ");
    //    printf("\n before scan");
    fflush(stdin);
    //    printf("\n fflush");
    ReadLine(str, 128, stdin);
    /*    scanf("%[^\n]",str); */
    printf("\n after scan");

    for (i = 1, p_str = str; ; i++, p_str = NULL)
    {
      token = mystrtok(p_str,delim);
      if (token == NULL)
	break;
      printf("%d: %s\n",i,token);
      tok[i-1] = token;
      printf("%s\n",tok[i-1]);
    }
    if(tok[0] != NULL)
    {
      if(strcmp(tok[0],"cd") == 0)
      {
        if (chdir(tok[1]) != 0)
          perror("chdir() error()");

        getcwd(cwd, sizeof(cwd));
        printf("current working directory is: %s\n", cwd);
      }

      else if(strcmp(tok[0],"pwd") == 0)
        if (getcwd(cwd, sizeof(cwd)) == NULL)
          perror("getcwd() error");
        else
          printf("current working directory is: %s\n", cwd);

      else if(strcmp(tok[0], "fork") == 0)
      {
        splice();
        printf ("\nSplice Performed\n");
      }


      else if(strcmp(tok[0],"exit") == 0)
        exit(3);

      else
        {
            splice();

            printf("other placeholder\n");
        }

    }
  }
}


int ReadLine(char *buff, int size, FILE *fp)
{
  buff[0] = '\0';
  buff[size - 1] = '\0';             /* mark end of buffer */
  char *tmp;

  if (fgets(buff, size, fp) == NULL) {
    *buff = '\0';                   /* EOF */
    return false;
  }
  else {
    /* remove newline */
    if ((tmp = strrchr(buff, '\n')) != NULL) {
      *tmp = '\0';
    }
  }
  return true;
}
