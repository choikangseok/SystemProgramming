#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define EOL       1
#define ARG       2
#define AMPERSAND 3
#define SEMICOLON 4
#define PIPE      5
#define PONDKEY   6

#define MAXARG    512
#define MAXBUF    512

#define FOREGROUND  0
#define BACKGROUND  1

