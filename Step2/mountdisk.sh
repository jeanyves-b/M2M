DISK=minidist.img
MOUNT_DIR=/mnt/minidist

sudo mkdir -p $MOUNT_DIR
sudo losetup -o131072 /dev/loop1 $DISK
#sudo mke2fs /dev/loop1

sudo mount /dev/loop1 $MOUNT_DIR
ls $MOUNT_DIR
