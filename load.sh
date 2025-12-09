#!/bin/bash

module=$1
mode=666

/sbin/insmod ./${module}.ko ${@:2} || exit 1
major=$(awk "\$2 == \"$module\" {print \$1}" /proc/devices)
rm -f $module
mknod -m $mode $module c $major 0