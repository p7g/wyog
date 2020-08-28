#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "repo.h"

const char *DEFAULT_DESC = "\
Unnamed repository; edit this file 'description' to name the repository.\n";
const char *DEFAULT_HEAD = "ref: refs/heads/master\n";
const char *DEFAULT_CONFIG = "\
[core]\n\
repositoryformatversion=0\n\
filemode=false\n\
bare=false\n";

void wyog_repo_deinit(struct wyog_repo *repo)
{
	free(repo->work_tree);
	free(repo->git_dir);
}

void wyog_repo_init(struct wyog_repo *repo, const char *path)
{
	size_t git_dir_size;

	repo->work_tree = strdup(path);
	git_dir_size = strlen(path) + sizeof("/.git");
	repo->git_dir = malloc(git_dir_size);
	snprintf(repo->git_dir, git_dir_size, "%s/.git", path);
	wyog_config_init(&repo->config);
}

int wyog_repo_open(struct wyog_repo *repo, const char *path)
{
	int fd;
	FILE *f;
	DIR *dir;
	int retval;

	retval = 0;
	wyog_repo_init(repo, path);
	dir = opendir(repo->git_dir);
	f = NULL;

	if (!dir) {
		/* assume there is only one possible error */
		fprintf(stderr, "'%s' is not a git repository\n", path);
		retval = 1;
		goto end;
	}

	fd = openat(dirfd(dir), "config", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Configuration file is missing\n");
		retval = 1;
		goto end;
	}

	f = fdopen(fd, "r");
	wyog_config_parse(&repo->config, f);

	if (repo->config.core.repositoryformatversion != 0) {
		fprintf(stderr, "Unsupported repositoryformatversion: %d\n",
				repo->config.core.repositoryformatversion);
		retval = 1;
		goto end;
	}

end:
	if (f)
		fclose(f);
	if (dir)
		closedir(dir);
	return retval;
}

int wyog_repo_new(struct wyog_repo *repo, const char *path)
{
#define X(expr, name) do { \
		if (!(expr)) { \
			retval = 1; \
			perror(name); \
			goto end; \
		} \
	} while (0)

	int i;
	int retval;
	int fd;
	DIR *dir;
	FILE *f;

	retval = 0;
	f = NULL;
	wyog_repo_init(repo, path);

	dir = opendir(path);
	if (!dir) {
		X(errno == ENOENT, "opendir");
		X(0 == mkdir(path, 0777), "mkdir");
		X((dir = opendir(path)), "opendir");
	} else {
		/* make sure the directory is empty */
		i = 0;
		while (readdir(dir) != NULL) {
			if (i++ >= 2)
				break;
		}
		if (i > 2) {
			fprintf(stderr, "Directory '%s' is not empty\n", path);
			retval = 1;
			goto end;
		}
	}
	closedir(dir);
	dir = NULL;

	X(0 == mkdir(repo->git_dir, 0777), "mkdir");
	X((dir = opendir(repo->git_dir)), "opendir");
	X(0 == mkdirat(dirfd(dir), "branches", 0777), "mkdirat");
	X(0 == mkdirat(dirfd(dir), "objects", 0777), "mkdirat");
	X(0 == mkdirat(dirfd(dir), "refs", 0777), "mkdirat");
	X(0 == mkdirat(dirfd(dir), "refs/tags", 0777), "mkdirat");
	X(0 == mkdirat(dirfd(dir), "refs/heads", 0777), "mkdirat");

#define WRITE_FILE(N, C) do { \
		X(-1 != (fd = openat(dirfd(dir), (N), O_CREAT | O_WRONLY, 0666)), "openat"); \
		f = fdopen(fd, "w"); \
		X(-1 != fputs((C), f), "write"); \
		fclose(f); \
		f = NULL; \
	} while (0)

	WRITE_FILE("description", DEFAULT_DESC);
	WRITE_FILE("HEAD", DEFAULT_HEAD);
	WRITE_FILE("config", DEFAULT_CONFIG);

#undef WRITE_FILE

end:
	if (dir)
		closedir(dir);
	if (f)
		fclose(f);
	return retval;
#undef X
}
