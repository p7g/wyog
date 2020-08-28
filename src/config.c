#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

enum section {
	SEC_NONE,
	SEC_CORE,
	SEC_UNKNOWN
};

void wyog_config_init(struct wyog_config *cfg)
{
	cfg->core.repositoryformatversion = 0;
}

void wyog_config_parse(struct wyog_config *cfg, FILE *f)
{
	ssize_t num;
	char *line, *key, *value;
	size_t line_size;
	enum section sec;

	line = NULL;
	sec = SEC_NONE;

	while ((num = getline(&line, &line_size, f)) != -1) {
		if (num <= 1)
			continue;

		switch (line[0]) {
		case ';': /* comment */
			continue;

		case '[': /* section start */
			if (!strncmp("core]", line + 1, 5))
				sec = SEC_CORE;
			else
				sec = SEC_UNKNOWN;
			continue;
		}

		switch (sec) {
		case SEC_CORE:
			key = strtok(line, "= \t");
			value = strtok(NULL, "= \t\n");
			if (!strcmp("repositoryformatversion", key))
				cfg->core.repositoryformatversion = atoi(value);
			break;

		default: break;
		}
	}

	if (line != NULL)
		free(line);
}
