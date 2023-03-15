#ifndef STRTOK_H
#define STRTOK_H

char * strtok(char *s, const char *delim);
char * strtok_r(char *s, const char *delim, char **last);

#endif