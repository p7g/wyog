#include <stdio.h>
#include <string.h>

#include "wyog.h"
#include "cmd_init.h"

const char *USAGE = "\
USAGE: wyog COMMAND [ARGS]\n\
\n\
Where COMMAND is one of:\n\
	add, cat-file, checkout, commit, hash-object, init, log, ls-tree, merge,\n\
	rebase, rev-parse, rm, show-ref, tag";

struct { char *name; command_fn *cmd; } COMMANDS[] = {
	{ "init", wyog_cmd_init },
};

int main(int argc, char **argv)
{
	int i;
	const char *cmd;
	int (*fn)(int, char **);

	if (argc < 2 || !*(cmd = argv[1])) {
		fprintf(stderr, "%s\n", USAGE);
		return 1;
	}

	fn = NULL;
	for (i = 0; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); i += 1) {
		if (!strcmp(COMMANDS[i].name, cmd)) {
			fn = COMMANDS[i].cmd;
			break;
		}
	}

	if (fn)
		return fn(argc - 2, argv + 2);

	fprintf(stderr, "'%s' is not recognized as a wyog command\n", cmd);
	return 2;
}
