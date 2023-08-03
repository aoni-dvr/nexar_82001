#!/bin/bash
#
# Find invalid log in  'git log --since=${SINCE}'
#
#   Default: find commit log during ${SINCE} that have no ${PTN}
#   Output:
#       ${LOGNAME}: invalid log
#       ${AUTHNAME}: author and statics.


# Debug
#set -x


# TODO: Branch

# parameter of --since for git log
SINCE=2.week

# Pattern for grep
PTN_PRJ="^\s\s*\[SDK"
PTN_SKIP="^\s\s*Merge\|^\s\s*Revert"
# Together
PTN="${PTN_PRJ}\|${PTN_SKIP}"

# Name of Invalid commit and log
LOGNAME=log_invalid.txt
AUTHNAME=log_author.txt


LOG=log.${SINCE}.txt

git log --since=${SINCE} > ${LOG}

cat /dev/null > ${LOGNAME}
CI=`grep -n "^commit [0-9a-f]*\$" ${LOG} | awk -F ':' '{print $1}' | sort -r -n`
END='$'
for i in ${CI}; do
    (sed -n "${i},${END}p" ${LOG}) >  log_tmp.txt
    ptn=`grep ${PTN} log_tmp.txt`
    if [ "x${ptn}" = "x" ]; then
        cat log_tmp.txt >> ${LOGNAME}
    fi
    END=$((i - 1))
done

rm -f ${LOG} log_tmp.txt

#
#   Statistic
#
cat /dev/null > ${AUTHNAME}
echo "`grep "^Author" ${LOGNAME} | cut -c 9-`" > log_tmp.txt
(cat log_tmp.txt | sort -u) > log_tmp2.txt
exec < log_tmp2.txt
while read line; do
    if [ "x$line" = "x" ]; then
        break
    fi
    n=`grep "Author: $line" ${LOGNAME} | wc -l`
    echo "${line}: ${n}" >> ${AUTHNAME}
done
TotalN=`cat ${AUTHNAME} | awk -F ':' 'BEGIB {v=0;} {v+=$NF} END {print v}'`
TotalA=`cat ${AUTHNAME} | wc -l`
echo "" >> ${AUTHNAME}
echo "Total authors: ${TotalA}" | tee -a ${AUTHNAME}
echo "Total commits: ${TotalN}" | tee -a ${AUTHNAME}

rm -f log_tmp*.txt

