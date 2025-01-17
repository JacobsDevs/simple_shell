#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "main.h"

#define MAX_STRING_LENGTH 20


/**
 * main - Main shell process.  Prompts the user for input and then
 * builds an argument list, forks and uses execve to execute the desired
 * command.
 *
 * Return: 0 on successful exit.
 */

int main(void)
{
	pid_t child;
	char **argv = NULL;
	stat_t sb;
	int count = 10;
	int running = 1;
	int exit_status = 0;
	int exitp;

	while (running == 1)
	{
		argv = malloc_argv(count);
		if (get_input(argv) == 1)
			run_exit(argv, exit_status);
		if (argv[0] == NULL)
		{
			clean_argv(argv);
			continue;
		}
		if (strcmp(argv[0], "exit") == 0)
			run_exit(argv, exit_status);
		if (strcmp(argv[0], "env") == 0)
		{
			print_environ(argv);
			continue;
		}
		if (stat(argv[0], &sb) == -1)
			search_for_function(argv, sb);
		child = fork();
		if (child == -1)
			printf("Failed to Fork");
		if (child == 0)
			execve(argv[0], argv, environ);
		else
		{
			waitpid(child, &exitp, 0);
			exit_status = WEXITSTATUS(exitp);
		}
		clean_argv(argv);
	}
	return (0);
}

/**
 * malloc_argv - Allocates memory to the argv pointer pointer
 * @count: The amount of strings to be stored in argv
 *
 * Return: Pointer to a pointer to argv
 */

char **malloc_argv(int count)
{
	char **args = NULL;
	int i;

	args = (char **)malloc(count * sizeof(char *));
	for (i = 0; i < count; i++)
		args[i] = NULL;
	if (args == NULL)
	{
		printf("Failed to malloc argv\n");
		exit(1);
	}
	return (args);
}

/**
 * get_input - Gets input from the user and stores it in argv
 * @argv: Pointer to pointer of argv
 *
 * Return: 0 for success 1 for failure to read line.
 */

int get_input(char **argv)
{
	char *buffer = NULL;
	size_t bsize = 0;
	char *token = NULL;
	int i = 0;
	stat_t sb;

	if (isatty(0) != 0)
		printf("$ ");
	if (getline(&buffer, &bsize, stdin) == -1)
	{
		free(buffer);
		return (1);
	}
	token = strtok(buffer, "\n");
	token = strtok(token, " ");
	while (token != NULL)
	{
		argv[i++] = strdup(token);
		token = strtok(NULL, " ");
	}
	if (argv[0] != NULL && stat(argv[0], &sb) == -1)
		check_valid_env(argv);
	free(buffer);
	return (0);
}

/**
 * clean_argv - Frees all memory held by argv
 * @argv: Pointer to a pointer of argv
 */

void clean_argv(char **argv)
{
	int i = 0;

	while (argv[i] != NULL)
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}
