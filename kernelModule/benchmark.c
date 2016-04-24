#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];

int main() {
	int fd = open("/dev/helloworld", O_RDWR, S_IRUSR|S_IWUSR);
	char *str = "this is a benchmark";

	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}
	
	printf("%d\n", strlen(str));

	if (write(fd, str, strlen(str)) < 0) {
		perror("Failed to write message to device.");
		return errno;
	}
	printf("wrote message: %s\n", str);

	if (read(fd, receive, BUFFER_LENGTH) < 0) {
		perror("Failed to read message from device.");
		return errno;
	}
	printf("received message: %s\n", receive);

	close(fd);
	return 0;
}
