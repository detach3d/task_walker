#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */
#include "task_walker.h"

int 
main()
{
	int file_desc, ret_val;
	char msg[4096] = {0};

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
    ret_val = ioctl(file_desc, IOC_WALK_TASKS, (unsigned long)msg);
	
    if (ret_val < 0) {
        printf("ioctl_set_msg failed:%d\n", ret_val);
        exit(-1);
    }

    printf("Message by ioctl:\n%s\n", msg);

	close(file_desc);
	
	return 0;
}