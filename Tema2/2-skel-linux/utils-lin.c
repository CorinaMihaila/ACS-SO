/**
 * Operating Sytems 2013 - Assignment 2
 *
 */

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#include "debug.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	/* TODO execute cd */

	return 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit()
{
	/* TODO execute exit/quit */

	return SHELL_EXIT;
}

/**
 * Concatenate parts of the word to obtain the command
 */
static char *get_word(word_t *s)
{
	int string_length = 0;
	int substring_length = 0;

	char *string = NULL;
	char *substring = NULL;

	while (s != NULL) {
		substring = strdup(s->string);

		if (substring == NULL) {
			return NULL;
		}

		if (s->expand == true) {
			char *aux = substring;
			substring = getenv(substring);

			/* prevents strlen from failing */
			if (substring == NULL) {
				substring = calloc(1, sizeof(char));
				if (substring == NULL) {
					free(aux);
					return NULL;
				}
			}

			free(aux);
		}

		substring_length = strlen(substring);

		string = realloc(string, string_length + substring_length + 1);
		if (string == NULL) {
			if (substring != NULL)
				free(substring);
			return NULL;
		}

		memset(string + string_length, 0, substring_length + 1);

		strcat(string, substring);
		string_length += substring_length;

		if (s->expand == false) {
			free(substring);
		}

		s = s->next_part;
	}

	return string;
}

/**
 * Concatenate command arguments in a NULL terminated list in order to pass
 * them directly to execv.
 */
static char **get_argv(simple_command_t *command, int *size)
{
	char **argv;
	word_t *param;

	int argc = 0;
	argv = calloc(argc + 1, sizeof(char *));
	assert(argv != NULL);

	argv[argc] = get_word(command->verb);
	assert(argv[argc] != NULL);

	argc++;

	param = command->params;
	while (param != NULL) {
		argv = realloc(argv, (argc + 1) * sizeof(char *));
		assert(argv != NULL);

		argv[argc] = get_word(param);
		assert(argv[argc] != NULL);

		param = param->next_word;
		argc++;
	}

	argv = realloc(argv, (argc + 1) * sizeof(char *));
	assert(argv != NULL);

	argv[argc] = NULL;
	*size = argc;

	return argv;
}

/*
 * @filedes  - file descriptor to be redirected
 * @filename - filename used for redirection
 * from lab 3 SO 2014
 */
static void do_redirect(int filedes, const char *filename)
{
	int rc;
	int fd;

	/* TODO 3 - Redirect filedes into fd representing filename */
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	DIE(fd < 0, "open");

	rc = dup2(fd, filedes);
	DIE(rc < 0, "dup2");

	close(fd);
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* Init aux variables */
	int pid,wait_ret,status,size,i,r;
	/* Init command string list*/
	char **command = get_argv(s,&size);

	/* sanity checks */
	DIE(s == NULL,"Parse_simple: NULL command");
	DIE(s->up != father,"Parse_command: Invalid father");

	/* TODO if builtin command, execute the command */

	/* Check exit/quit */
	if((strcmp(get_word(s->verb),"exit") == 0) ||
		(strcmp(get_word(s->verb),"quit") == 0)){
		r = shell_exit();
		DIE(r != SHELL_EXIT,"Error: shell_exit");
		return r;
	}

	/* TODO if variable assignment, execute the assignment and return
         * the exit status */

	/* TODO if external command:
         *   1. fork new process
	 *     2c. perform redirections in child
         *     3c. load executable in child
         *   2. wait for child
         *   3. return exit status
	 */
    pid = fork();
	switch (pid) {
    case -1:
        DIE(pid == -1, "Error: fork");
    case 0:

    	if(s->out != NULL){
    		do_redirect(STDOUT_FILENO, get_word(s->out));
    	}

        execvp(command[0], (char *const *) command);
        fprintf(stderr, "Execution failed for '%s'\n", command[0]);
        fflush(stdout);

        // free command
        i = 0;
        while(command[i] != NULL){
        	free(command[i]);
        	i++;
        }
       	free(command);

        exit(EXIT_FAILURE);
        break;
    default:
        // free command
        i = 0;
        while(command[i] != NULL){
        	free(command[i]);
        	i++;
        }
       	free(command);

        // asteapta procesul copil sa termine
        wait_ret = waitpid(pid, &status, 0);
        DIE(wait_ret < 0, "Error: waitpid");
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else
            return EXIT_FAILURE;
        break;
    }

	return EXIT_SUCCESS; /* TODO replace with actual exit status */
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool do_in_parallel(command_t *cmd1, command_t *cmd2, int level, command_t *father)
{
	/* TODO execute cmd1 and cmd2 simultaneously */

	return true; /* TODO replace with actual exit status */
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2)
 */
static bool do_on_pipe(command_t *cmd1, command_t *cmd2, int level, command_t *father)
{
	/* TODO redirect the output of cmd1 to the input of cmd2 */

	return true; /* TODO replace with actual exit status */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* sanity checks */
	int r=0;
	DIE(c == NULL,"Parse_command: NULL command");
	DIE(c->up != father,"Parse_command: Invalid father");

	if (c->op == OP_NONE) {
		/* execute a simple command */
		r = parse_simple(c->scmd, level + 1, c);

		return r;
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO execute the commands one after the other */
		break;

	case OP_PARALLEL:
		/* TODO execute the commands simultaneously */
		break;

	case OP_CONDITIONAL_NZERO:
		/* TODO execute the second command only if the first one
                 * returns non zero */
		break;

	case OP_CONDITIONAL_ZERO:
		/* TODO execute the second command only if the first one
                 * returns zero */
		break;

	case OP_PIPE:
		/* TODO redirect the output of the first command to the
		 * input of the second */
		break;

	default:
		assert(false);
	}

	return 0; /* TODO replace with actual exit code of command */
}

/**
 * Readline from mini-shell.
 */
char *read_line()
{
	char *instr;
	char *chunk;
	char *ret;

	int instr_length;
	int chunk_length;

	int endline = 0;

	instr = NULL;
	instr_length = 0;

	chunk = calloc(CHUNK_SIZE, sizeof(char));
	if (chunk == NULL) {
		fprintf(stderr, ERR_ALLOCATION);
		return instr;
	}

	while (!endline) {
		ret = fgets(chunk, CHUNK_SIZE, stdin);
		if (ret == NULL) {
			break;
		}

		chunk_length = strlen(chunk);
		if (chunk[chunk_length - 1] == '\n') {
			chunk[chunk_length - 1] = 0;
			endline = 1;
		}

		ret = instr;
		instr = realloc(instr, instr_length + CHUNK_SIZE + 1);
		if (instr == NULL) {
			free(ret);
			return instr;
		}
		memset(instr + instr_length, 0, CHUNK_SIZE);
		strcat(instr, chunk);
		instr_length += chunk_length;
	}

	free(chunk);

	return instr;
}

