#include <stdlib.h>
#include <stdio.h>
#include "parse.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>


int laststatus = 0;

int main()
{
    char buf[1000], **p;
    extern void execute(char **argv);

    while (printf("$ "), fgets(buf, sizeof buf, stdin))
        if ((p = parse(buf)))
            execute(p);

    return(laststatus);
}

void execute(char **argv) {
	int hasSlash = 0;
	char path[strlen(argv[0]) + 10];

	for (int i = 0; argv[0][i] != '\0'; i++) {
		if (argv[0][i] == '/') {
			hasSlash = 1;
			break;
		}
	}


	int found = 0;
	if (hasSlash == 0) {
		struct stat sb;
		char searchPaths[3][10];
		strcpy(searchPaths[0], "/bin/");
		strcpy(searchPaths[1], "/usr/bin/");
		strcpy(searchPaths[2], "./");

		for (int j = 0; j < 3; j++) {
			strcpy(path, searchPaths[j]);
			strcat(path, argv[0]);
			if (stat(path, &sb) == -1) {
				continue;
			}

			if (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode)) {
				found = 1;
				break;
			}
		}

		if (found == 0) {
			fprintf(stderr, "%s: Command not found\n", argv[0]);
			laststatus = 1;
		}
	} else {
		strcpy(path, argv[0]);
		found = 1;
	}

	extern char **environ;

	if (found == 1) {
		int x = fork();
		if (x == -1) {
			laststatus = 1;
			perror("fork");
			exit(1);
		} else if (x == 0) {
			/* child */
			execve(path, argv, environ);
			exit(1);
		} else {
			/* parent */
			int status;
			if (wait(&status) == -1) {
				laststatus = 1;
				perror("wait");
				exit(1);
			}

			laststatus = WEXITSTATUS(status);
			if (laststatus != 0) {
				fprintf(stderr, "exit status %d\n", laststatus);
			}
		}
	}
}
