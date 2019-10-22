#ifndef HTTPD_FS_H
#define HTTPD_FS_H

struct httpd_file{
	struct ssfs_info * sfile;
	int filelen;
	int gz;
	u8 filename[64];
};

struct httpd_file * httpd_file_open(char * path_name,bool b_file,bool b_add);
int httpd_file_close(struct httpd_file * sfile);
int httpd_file_read(struct httpd_file * sfile,char * buf, size_t size);
int httpd_file_len(struct ssfs_info * sfile);


#endif  //HTTPD_FS_H
