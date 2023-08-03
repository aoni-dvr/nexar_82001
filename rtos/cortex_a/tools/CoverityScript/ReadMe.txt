[Support Server]: ambtwubu3, ambtwubu17 and ambtwubu18

[Generage Coverity Configuration]
  $ cd CoverityScript/
  $ bash CoverityConfigCreate.sh
  
  [Note]: This script only support arm-none-eabi-gcc, aarch64-none-elf-gcc, vis-gcc and qcc, user can create others if want.
    # for gcc 32bit
    $ cov-configure --config rtos_config.xml --template --comptype gcc --compiler arm-none-eabi-gcc
    # for gcc 64bit
    $ cov-configure --config rtos_config.xml --template --comptype gcc --compiler aarch64-none-elf-gcc
    # for cv
    $ cov-configure --config rtos_config.xml --template --comptype gcc --compiler vis-gcc
    # for qnx
    $ cov-configure --config rtos_config.xml --template --comptype qnxcc --compiler qcc
 
[Analysis Flow Example]:
  $ cp -pfr /share/CoverityScript/ rtos/cortex_a/
  $ cd rtos/cortex_a/
  $ export PATH=/opt/cov-analysis-linux64-2020.12/bin:$PATH
  $ rm -rf output* dir_*
  $ source /opt/amba/env/env_set.sh cv2
  $ make -j 8 cv2/icam/misra_c_scan_defconfig
  $ cov-build --dir dir_coverity --config CoverityScript/rtos_config.xml --emit-complementary-info make -j 8 soc/dsp/imgkernel/imgkernel/cv2x/build/threadx/
  $ cov-build --dir dir_coverity --config CoverityScript/rtos_config.xml --emit-complementary-info make -j 8 soc/dsp/imgkernel/imgkernelcore/cv2x/build/threadx/
  $ cov-build --dir dir_coverity --config CoverityScript/rtos_config.xml --emit-complementary-info make -j 8 soc/dsp/imgkernel/imgcal/build/threadx/
  $ cov-build --dir dir_coverity --config CoverityScript/rtos_config.xml --emit-complementary-info make -j 8 svc/comsvc/imgproc/imgalgo/cv2x/
  $ cov-build --dir dir_coverity --config CoverityScript/rtos_config.xml --emit-complementary-info make -j 8 amba_svc
  
  [For MISRA-C] (user can modify "misrac2012-amba_20210824.config" to add/delete deviation)
  $ cov-analyze --config CoverityScript/rtos_config.xml --dir dir_coverity \
    --coding-standard-config CoverityScript/misrac2012-amba_20210824.config --jobs 8

  [For CERT-C] (user can modify "cert-c-amba_20211122.config" to add/delete deviation)
  $ cov-analyze --config CoverityScript/rtos_config.xml --dir dir_coverity \
    --coding-standard-config CoverityScript/cert-c-amba_20210819.config --jobs 8

  [Generage html report] 
  $ cov-format-errors --dir dir_coverity --filesort --exclude-files ".*/output.64/.*|.*/output/.*" \
    --html-output Result_MISRA


