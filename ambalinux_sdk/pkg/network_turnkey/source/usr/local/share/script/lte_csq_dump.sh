#!/bin/sh

if [ -e /mnt/extsd/csq_dump.txt ]; then
        while true
        do
                echo "[`date +"%Y-%m-%d %H:%M:%S"`]`devid --csq`" >> /mnt/extsd/csq_dump.txt
                sleep 2
        done
fi

