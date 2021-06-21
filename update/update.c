#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FILE_INDEX_TEMPLATE "main/blog.html"
#define DEBUG_0(fmt,args...) printf("%s(%d)-%s->"fmt, \
                __FILE__, __LINE__, __func__, ##args)
#define DEBUG_ERR(fmt,args...) printf("[ERROR]%s(%d)-%s->"fmt, \
                __FILE__, __LINE__, __func__, ##args)
typedef void (*func_file_code)(const char *fine);

char *file_index;
void get_file_path(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

bool is_special_dir(const char *path)
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}
void process_file(const char *path, func_file_code ffc, void *ptr)
{
        DIR *dir;
        struct dirent *entry;
        char file_path[PATH_MAX], filename[PATH_MAX], dest[PATH_MAX], 
	     *src = NULL, buf[255] = {0}, ch;
	struct stat statbuf;
	int type = 0, i = 0;
	FILE *Fdest = NULL;
	FILE *Fsrc = NULL;

	sprintf(dest, "%s/index.htm", path);
	Fdest = fopen(dest, "a+");
	if (ptr != NULL) {
		src = (char *)ptr;
		Fsrc = fopen(src, "r");
		if (NULL != Fsrc) {
			while ((ch = fgetc(Fsrc)) != EOF) {
				fputc(ch, Fdest);
			}
			fclose(Fsrc);
		}
	} else {
	}
	if((dir = opendir(path)) == NULL)
		return;
	while((entry = readdir(dir)) != NULL) {
		get_file_path(path, entry->d_name, file_path);
		if (strncmp("index.htm", entry->d_name, 9) == 0)
			continue;
		if(is_special_dir(entry->d_name))
			continue;
		if(lstat(file_path, &statbuf) != 0) {
			continue;
		}
		type = statbuf.st_mode & 0xE000;
		if (type == S_IFDIR) {
			process_file(file_path, ffc, NULL);
			continue;
		}
		if (type == S_IFREG) {
			if (i == 0) {
				sprintf(buf,
						"<br><a href='%s'>%.2d.%s</a><div style='float:right'>%s</div>",
						entry->d_name,
					      i++, entry->d_name,
					      ctime(&statbuf.st_mtime)
					      );
			} else {
				sprintf(buf,
						"<a href='%s'>%.2d.%s</a><div style='float:right'>%s</div>",
						entry->d_name,
					      i++, entry->d_name,
					      ctime(&statbuf.st_mtime)
					      );
			}
			DEBUG_0("file :reg\n");
			fwrite(buf, strlen(buf), 1, Fdest);
			fwrite("<br>\n", 5, 1, Fdest);
		}
	}
	fwrite("</body>\n", 8, 1, Fdest);
	closedir(dir);
	fclose(Fdest);
}
void copy_file(char* src, char *des)
{
	char buffer[1024];
	FILE *in,*out;
	int len;

	if((in=fopen(src, "r"))==NULL){
		printf("源文件打开失败！\n");
		exit(1);
	}
	if((out=fopen(des, "w+"))==NULL){
		printf("目标文件创建失败！\n");
		exit(1);
	}
	while((len=fread(buffer,1,1024,in))>0){
		fwrite(buffer,1,len,out);
	}
	fclose(out);
	fclose(in);
}
void process_dir(const char *path, func_file_code ffc, void *ptr)
{
        DIR *dir;
        struct dirent *entry;
        char file_path[PATH_MAX], filename[PATH_MAX], dest[PATH_MAX], 
	     *src = NULL, buf[255] = {0}, ch;
	struct stat statbuf;
	int type = 0, i = 0;
	FILE *Fdest = NULL;
	FILE *Fsrc = NULL;

	sprintf(dest, "%s/index.html", path);
	copy_file(file_index, dest);
	sprintf(dest, "%s/index.htm", path);
	Fdest = fopen(dest, "w");
	if (ptr != NULL) {
		src = (char *)ptr;
		Fsrc = fopen(src, "r");
		if (NULL != Fsrc) {
			while ((ch = fgetc(Fsrc)) != EOF) {
				fputc(ch, Fdest);
			}
			fclose(Fsrc);
		}
	} else {
	}
	if((dir = opendir(path)) == NULL)
		return;
	while((entry = readdir(dir)) != NULL) {
		get_file_path(path, entry->d_name, file_path);
		if(is_special_dir(entry->d_name))
			continue;
		if(lstat(file_path, &statbuf) != 0) {
			continue;
		}
		type = statbuf.st_mode & 0xE000;
		if (type == S_IFREG)
			continue;
		if (type == S_IFDIR) {
			process_dir(file_path, ffc, ptr);
			sprintf(buf, 
		"<a href='%s/index.htm'>DIR:%s</a><div style='float:right'>%s</div>",
					entry->d_name,
				      entry->d_name,
				      ctime(&statbuf.st_mtime)
				      );
			DEBUG_0("file :dir\n");
			fwrite(buf, strlen(buf), 1, Fdest);
			fwrite("<br>\n", 5, 1, Fdest);
		}
	}
	//fwrite("</body>\n", 8, 1, Fdest);
	closedir(dir);
	fclose(Fdest);
}
void main(int argc, char **argv)
{
	//char *p = "pre.html";
	char *p = "main/pre_cn.html";
	//	p = NULL;
	if (argc > 2) {
		file_index = argv[2];
		process_dir(argv[1], NULL, p);
		process_file(argv[1], NULL, NULL);
	} else {
		printf("Usage: update [PATH] [index path]\n");
	}
}
