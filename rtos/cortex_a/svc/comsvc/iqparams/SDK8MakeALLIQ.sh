#!/bin/bash
# ambtwubu8:/dump15/wkche/sdk8_dev_20200526/rtos/cortex_a$ svc/comsvc/iqparams/SDK8MakeALLIQ.sh cv2fs



chiplist=($(find ./svc/comsvc/iqparams/ -maxdepth 1 -type d))

for j in ${chiplist[@]}
do
    chipname=${j:22} 
    #remove first two char
    # if [[ "${chipname}" != "" ]] && [[ "${chipname}" != "cv2fs" ]] && [[ "${chipname}" == "h32" ]]; then
    #  if [ "${chipname}" == "cv2fs" ]; then    
    if [[ "${chipname}" != "" ]] && [[ "${chipname}" == $1 ]]; then
        echo $chipname
        echo "make IQ for: $@"
        make svc/comsvc/iqparams/${chipname}/ -j 
        list=($(find ./svc/comsvc/iqparams/${chipname}/ -type d))
        for i in ${list[@]}
        do
            if [[ "${chipname}" == "cv2" ]] || [[ "${chipname}" == "h32" ]] || [[ "${chipname}" == "cv5" ]]; then
            name=${i:26}
            fi
            if [[ "${chipname}" == "cv22" ]] || [[ "${chipname}" == "cv25" ]] || [[ "${chipname}" == "cv28" ]]; then
            name=${i:27}
            fi
            if [[ "${chipname}" == "cv2fs" ]]; then
            name=${i:28}
            fi
            #remove first two char            
            if [ "${name}" != "" ] && [ "${name}" != "build" ]; then
            	echo $name
                cd svc/comsvc/iqparams/${chipname}/${name} 
                ./Iq_bat.bat 
                cd -
            fi    
        done
    fi
done
