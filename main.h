#ifndef MAIN_H
#define MAIN_H

extern char **environ;

char **malloc_argv(int count);
void clean_argv(char **argv, int count);
void get_input(char **argv);

#endif
