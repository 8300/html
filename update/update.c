#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

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

bool is_special_dir(const char *path);
void copy_file(char *des, char* src);
void get_file_path(const char *path, const char *file_name,  char *file_path);
int index_html_create(char *path, char *pre);
int main(int argc, char **argv)
{
	char *pre = "main/pre_cn.html";
	char *path = "tmp";
	if (argc > 2) {
		path = argv[1];
		pre = argv[2];
		index_html_create(path, pre);
	} else {
		index_html_create(path, pre);
		printf("%s,%s\n", path, pre);
	}
}
int index_html_create(char *path, char *pre)
{
	int rc = 0, type = 0, i = 0;
        DIR *dir;
        struct dirent *entry;
        char file_path[PATH_MAX], filename[PATH_MAX], dest[PATH_MAX], 
	     *src = NULL, buf[255] = {0}, ch;
	struct stat statbuf;
	FILE *Fdest = NULL;
	FILE *Fsrc = NULL;

	sprintf(dest, "%s/index.html", path);
	copy_file(dest, pre);
	Fdest = fopen(dest, "a+");
	if (Fdest == NULL) {
		rc = -ENOENT;
		goto err;
	}
	sprintf(buf,
		"<div style=\"margin:2%% 2%% 2%% 12%%; color: #009900\">\n");
	//fwrite(buf, strlen(buf), 1, Fdest);
	fwrite(buf, 1, strlen(buf), Fdest);
	if((dir = opendir(path)) == NULL) {
		rc = -ENOENT;
		goto err;
	}
	//create dir
	while((entry = readdir(dir)) != NULL) {
		get_file_path(path, entry->d_name, file_path);
		if(is_special_dir(entry->d_name))
			continue;
		if(lstat(file_path, &statbuf) != 0) {
			continue;
		}
		if (strncmp(".git", entry->d_name, 4) == 0)
			continue;
		type = statbuf.st_mode & 0xE000;
		if (type == S_IFREG)
			continue;
		if (type == S_IFDIR) {
			index_html_create(file_path, pre);
			sprintf(buf, 
		"<a style=\"color: #009900\" href='%s/index.html'>DIR:%s</a><div style='float:right'>%s</div>",
					entry->d_name,
				      entry->d_name,
				      ctime(&statbuf.st_mtime)
				      );
			DEBUG_0("file :dir\n");
			fwrite(buf, strlen(buf), 1, Fdest);
			fwrite("<br>\n", 5, 1, Fdest);
		}
	}
	closedir(dir);
	if((dir = opendir(path)) == NULL) {
		rc = -ENOENT;
		goto err;
	}
	//show file
	while((entry = readdir(dir)) != NULL) {
		get_file_path(path, entry->d_name, file_path);
		if (strncmp("index.html", entry->d_name, 9) == 0)
			continue;
		if(is_special_dir(entry->d_name))
			continue;
		if(lstat(file_path, &statbuf) != 0) {
			continue;
		}
		type = statbuf.st_mode & 0xE000;
		if (type == S_IFREG) {
			if (i == 0) {
				sprintf(buf,
						"<br><a style=\"color: #009900\" href='%s'>%.2d.%s</a><div style='float:right'>%s</div>",
						entry->d_name,
					      i++, entry->d_name,
					      ctime(&statbuf.st_mtime)
					      );
			} else {
				sprintf(buf,
						"<a style=\"color: #009900\" href='%s'>%.2d.%s</a><div style='float:right'>%s</div>",
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
	fwrite("</div>\n", 7, 1, Fdest);
	fwrite("</body>\n", 8, 1, Fdest);
	closedir(dir);
	fclose(Fdest);
err:
	return rc;
}
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
void copy_file(char *des, char *src)
{
	char buffer[1024];
	FILE *in,*out;
	int len;

	if((in=fopen(src, "r"))==NULL){
		printf("源文件打开失败！:%s\n", src);
		exit(1);
	}
	if((out=fopen(des, "w+"))==NULL){
		printf("目标文件创建失败！:%s\n", des);
		exit(1);
	}
	while((len=fread(buffer,1,1024,in))>0){
		fwrite(buffer,1,len,out);
	}
	fclose(out);
	fclose(in);
}
