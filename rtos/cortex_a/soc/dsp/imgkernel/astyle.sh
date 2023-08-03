#!/bin/bash

for i in $(find ./ -name '*.c' -or -name '*.h')
do
	echo $i
	astyle --style=linux --indent=spaces=4 $i
	dos2unix $i
	chmod 644 $i
	rm -rf $i.orig
done
