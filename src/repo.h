#ifndef WYOG_REPO_H
#define WYOG_REPO_H

#include "config.h"

struct wyog_repo {
	char *work_tree,
	     *git_dir;
	struct wyog_config config;
};

int wyog_repo_new(struct wyog_repo *repo, const char *path);
int wyog_repo_open(struct wyog_repo *repo, const char *path);
void wyog_repo_deinit(struct wyog_repo *repo);

#endif
