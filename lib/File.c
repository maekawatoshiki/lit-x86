#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

typedef struct {
	FILE *fp;
	int size;
	char *name;
} file_t;

struct files_t {
	file_t **file;
	int cur_files;
} files;

int File_size(char *name) {
	FILE *fp = fopen(name, "rb");
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return sz;
}
int File_size_by_id(int id) {
	if(id < files.cur_files)
		return files.file[id]->size;
	return -1;
}
int File_open(char *name, char *mode) {
	file_t *f = malloc(sizeof(file_t));
	f->fp = fopen(name, mode);
	if(!f->fp) { free(f); return 0; }
	f->size = File_size(name);
	f->name = name;
	if(files.file == NULL) { // initialize
		files.file = calloc(2, sizeof(file_t));
		files.cur_files = 1;
	} else files.file = realloc(files.file, (1+files.cur_files)*sizeof(file_t));
	files.file[files.cur_files++] = f;
	return files.cur_files - 1;
}
char *File_read(int id) {
	if(id < files.cur_files) {
		file_t *f = files.file[id];
		char *str = (char *)calloc(f->size+1, 1);
		fread(str, f->size, 1, f->fp);
		return str;
	} return NULL;
}
void File_write(int id, char *str) {
	if(id < files.cur_files) 
		fwrite(str, strlen(str), 1, files.file[id]->fp);
}
DIR *dir;
char *File_dir_list(char *cur_dir_name) {
	struct dirent *dp;
	char **dirs = calloc(1, sizeof(char *));
	int dir_count = 0;

	if(dir == NULL) {
		dir = opendir(cur_dir_name);
		if(dir == NULL) return NULL;
	}

	dp = readdir(dir);
	if(dp == NULL) { 
		closedir(dir); dir = NULL; 
		return NULL; 
	}
	return dp->d_name;
}
void File_close(int id) {
	if(id < files.cur_files) {
		file_t *f = files.file[id];
		fclose(f->fp);
		free(f);
	}
}
