#ifndef TASK_WALKER_H_
#define TASK_WALKER_H_

#include <linux/stddef.h>
#include <linux/ioctl.h>

#define TASK_WALKER_MAGIC		't'
#define IOC_WALK_TASKS			_IO(TASK_WALKER_MAGIC, 1)
#define DEVICE_FILE_NAME        "task_walker"

#endif
