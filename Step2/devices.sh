#http://www.lanana.org/docs/device-list/devices-2.6+.txt


DIR=~/git/M2M/Step2/MiniDist/dev

rm -r $DIR
mkdir $DIR

#disque dur IDE
mknod $DIR/hda b 3 0
mknod $DIR/hda1 b 3 1

#1er uart port série
mknod $DIR/ttyS0 c 4 64

#1er uart port série
mknod $DIR/tty0 c 4 0

ls -l $DIR
