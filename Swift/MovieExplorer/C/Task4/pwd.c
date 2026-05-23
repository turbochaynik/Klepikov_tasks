#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	char str[256];
	errno = 0;
	getcwd(str, 256);
	if (errno)
	{
		printf("Error! Error code: %d\n", errno);
		return errno;
	}
	printf("%s\n", str);
	return 0;
}


