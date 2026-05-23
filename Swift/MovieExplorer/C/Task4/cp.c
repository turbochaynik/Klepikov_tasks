#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

int cpFile(char* arg1, char* arg2)
{
	int fd1 = open(arg1, O_RDONLY);
	if (fd1 < 0)
	{
		fprintf(stderr, "Error in %s: %s\n", arg1, strerror(errno));
		return 1;
	}

	struct stat sb;

        if (lstat(arg1, &sb) < 0)
        {
                fprintf(stderr, "Error for lstat in %s\n", arg1);
                return 1;
        }

	int mode = sb.st_mode;

	char* chk1 = realpath(arg1, NULL);
	char* chk2 = realpath(arg2, NULL);

	if (chk1 != NULL && chk2 != NULL && strcmp(chk1, chk2) == 0)
	{
		fprintf(stderr, "Error: %s and %s are the same files\n", arg1, arg2);
		close(fd1);
		
		free(chk1);
		free(chk2);
		return 1;
	}

	free(chk1);
	free(chk2);

	char buf[256];

	if (!(S_ISDIR(mode)))
	{
		int fd2 = open(arg2, O_WRONLY | O_TRUNC | O_CREAT);
		if (fd2 < 0)
		{
			close(fd1);
			fprintf(stderr, "Error in %s: %s\n", arg2, strerror(errno));
			return 1;
		}

		int num;

		// If fd1 != dir

		while ((num = read(fd1, buf, 256)) != 0)
			write(fd2, buf, num);	

		close(fd1);
		close(fd2);	

		chmod(arg2, mode);
	}

	else
	{
		close(fd1);
		
		DIR* d1 = opendir(arg1);

		if (mkdir(arg2, mode) < 0)
			fprintf(stderr, "Error in mkdir: %s\n", strerror(errno));

		if (d1 == NULL)
		{
			fprintf(stderr, "Error in opendir : %s\n", arg1);
			return 1;
		}

		struct dirent* sd;
		char* str1 = calloc(256, sizeof(char));
		char* str2 = calloc(256, sizeof(char));
		char* zero = calloc(256, sizeof(char));

		int len1 = strlen(arg1);
		int len2 = strlen(arg2);

		memcpy(str1, arg1, len1);
		memcpy(str2, arg2, len2);

		while((sd = readdir(d1)))
		{
			if (strncmp(sd->d_name, "..\0", 4096) == 0 || strcmp(sd->d_name, ".\0") == 0)
			       continue;

			int tmp1 = strlen(sd->d_name);
			if (tmp1 + len1 + 1 > 256)
			{
				fprintf(stderr, "Full path is too big!\n");
				return 1;
			}

			int tmp2 = tmp1;
			if (tmp2 + len2 + 1> 256)
			{
				fprintf(stderr, "Full paths are too big!\n");
				return 1;
			}		
			str1[len1] = '/';
			str2[len2] = '/';	

			memcpy(str1 + len1 + 1, (sd->d_name), tmp1);
			memcpy(str2 + len2 + 1, (sd->d_name), tmp2);

			cpFile(str1, str2);

			memcpy(str1 + len1, zero, tmp1);
			memcpy(str2 + len2, zero, tmp2);
		}

		free(str1);
		free(str2);
		free(zero);

		closedir(d1);
	}
return 0;
}



int main(int argc, char* argv[])
{
	if (argc != 3)
	{
	
		fprintf(stderr, "Wrong arguments number!\nUsage: <source file> <destination file>\n");
		return 1;
	}

	struct stat sb;

	char *tmp1, *tmp2;

	tmp1 = realpath(argv[1], NULL);
	tmp2 = realpath(argv[2], NULL);

	if (tmp1 != NULL && tmp2 != NULL && strcmp(tmp1, tmp2) == 0)
	{
		fprintf(stderr, "Error: %s and %s are the same files\n", argv[1], argv[2]);
		free(tmp1);
		free(tmp2);
		return 1;
	}
	if (tmp1 != NULL)
	free(tmp1);
	if (tmp2 != NULL)
	free(tmp2);

	errno = 0;
	int mode = 0;
	lstat(argv[2], &sb);

	if (!errno)
		mode = sb.st_mode; // If file exists and no error

	int len2 = strlen(argv[2]);
	char* name = strrchr(argv[1], '/');
	if (name == NULL)
		name = argv[1];
	else
		++name; // Getting rid of /
	// Getting name of argv[1] file


	char* str = NULL;
	int isdir = 0;
	
	if (S_ISDIR(mode))
	{
		isdir = 1;
		if (len2 + strlen(argv[1]) + 2 > 256)
		{
			fprintf(stderr, "Names of args are too big!\n");
			return 1;
		}
		str = calloc(256, sizeof(char));
		memcpy(str, argv[2], len2);
		str[len2] = '/';
		memcpy(str+len2+1, name, strlen(name));
	}
	else
		str = argv[2];

	cpFile(argv[1], str);

	if (isdir)
		free(str);
}

