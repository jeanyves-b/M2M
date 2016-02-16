#http://www.lanana.org/docs/device-list/devices-2.6+.txt


DIR=/mnt/minidist/dev

rm -r $DIR
mkdir $DIR

#disque dur IDE
#mknod $DIR/hda b 3 0
#1ere partition disque dur IDE
#mknod $DIR/hda1 b 3 1

#1er uart port série
#mknod $DIR/ttyS0 c 4 64

#1er uart port série
#mknod $DIR/tty0 c 4 0

mknod $DIR/tty0 c 4 0
mknod $DIR/tty1 c 4 1
mknod $DIR/sr0 b 11 0
mknod $DIR/sda b 8 0
mknod $DIR/sda1 b 8 1
mknod $DIR/sda2 b 8 2
mknod $DIR/hda b 3 0
mknod $DIR/hda1 b 3 1
mknod $DIR/hda2 b 3 2
mknod -m 0600 $DIR/console c 5 1
mknod $DIR/null c 1 3
mknod $DIR/zero c 1 5

ls -l $DIR

