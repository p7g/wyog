#include <stdio.h>

#include "cmd_init.h"
#include "repo.h"

int wyog_cmd_init(int argc, char **argv)
{
	int retval;
	const char *dest;
	struct wyog_repo repo;

	if (argc > 1) {
		fprintf(stderr, "USAGE: wyog init [PATH]\n");
		return 1;
	} else if (argc == 1) {
		dest = argv[0];
	} else {
		dest = ".";
	}

	retval = wyog_repo_new(&repo, dest);
	wyog_repo_deinit(&repo);
	return retval;
}
