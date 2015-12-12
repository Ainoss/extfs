#!/bin/bash

IMG_NAME=./fs.img
TEST_FOLDER=$1

set -e

dd if=/dev/zero of=$IMG_NAME bs=1048576 count=128 > /dev/null
/sbin/mke2fs -b 4096 -I 128 -O none -r 1 -t ext2 ./fs.img

if [ -d "$TEST_FOLDER" ] 
then
    echo "Copy $TEST_FOLDER to image root..."
else
    echo "Skip copying test dir to image..."
    exit 0
fi
mkdir -p ./fs_mnt
if ! mount $IMG_NAME ./fs_mnt
then
    echo "Superuser rights are required, aborting"
    rm -r ./fs_mnt
    exit 0
fi
cp -r $TEST_FOLDER ./fs_mnt/
umount ./fs_mnt
rm -r ./fs_mnt

echo "Image $IMG_NAME with ext2 fs created successfully!"

