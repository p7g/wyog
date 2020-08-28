#ifndef WYOG_CONFIG_H
#define WYOG_CONFIG_H

#include <stdio.h>

struct wyog_config {
	struct {
		int repositoryformatversion;
	} core;
};

void wyog_config_init(struct wyog_config *cfg);
void wyog_config_parse(struct wyog_config *cfg, FILE *f);

#endif
