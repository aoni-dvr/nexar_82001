#!/bin/sh

umount /data && ubidetach -m 14 && ubiformat /dev/mtd14 -y
