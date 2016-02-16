#http://www.lanana.org/docs$DIRice-list$DIRices-2.6+.txt


DIR=/mnt/minidist/dev

#disque dur IDE
#mknod $DIR/hda b 3 0

#1ere partition disque dur IDE
#mknod $DIR/hda1 b 3 1

#1er uart port série
#mknod $DIR/ttyS0 c 4 64

#1er uart port série
#mknod $DIR/tty0 c 4 0

[ -d $DIR ] || mkdir -m 0755 $DIR
[ -d /root ] || mkdir -m 0700 /root
[ -d /sys ] || mkdir /sys
[ -d /proc ] || mkdir /proc
[ -d /tmp ] || mkdir /tmp
mkdir -p /var/lock
mount -t sysfs -o nodev,noexec,nosuid none /sys 
mount -t proc -o nodev,noexec,nosuid none /proc 

[ -e $DIR/tty0 ] || mknod $DIR/tty0 c 4 0
[ -e $DIR/tty1 ] || mknod $DIR/tty1 c 4 1
[ -e $DIR/sr0 ] || mknod $DIR/sr0 b 11 0
[ -e $DIR/sda ] || mknod $DIR/sda b 8 0
[ -e $DIR/sda1 ] || mknod $DIR/sda1 b 8 1
[ -e $DIR/sda2 ] || mknod $DIR/sda2 b 8 2
[ -e $DIR/hda ] || mknod $DIR/hda b 3 0
[ -e $DIR/hda1 ] || mknod $DIR/hda1 b 3 1
[ -e $DIR/hda2 ] || mknod $DIR/hda2 b 3 2
[ -e $DIR/cdrom ] || ln  -s $DIR/sr0 $DIR/cdrom
[ -e $DIR/console ] || mknod -m 0600 $DIR/console c 5 1
[ -e $DIR/null ] || mknod $DIR/null c 1 3
[ -e $DIR/zero ] || mknod $DIR/zero c 1 5

[ -d /etc ] || mkdir -m 0700 /etc
[ -e /etc/mtab ] || touch /etc/mtab

ls -l $DIR
