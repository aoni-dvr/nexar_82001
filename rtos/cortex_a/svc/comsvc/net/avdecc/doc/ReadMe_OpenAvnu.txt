Repository (2018-10-12)
======================================================================================================

OpenAvnu
---------

$ git clone https://github.com/AVnu/OpenAvnu.git
$ cd OpenAvnu
$ git submodule init
$ git submodule update


gPTP
-----

$ git clone https://github.com/AVnu/gptp.git


Build
======================================================================================================

OpenAvnu
---------

$ make all
$ cd lib/igb_avb
$ make lib
$ make kmod


gPTP
-----

$ travis.sh

Run
======================================================================================================

igb_avb (refer to run_igb.sh)
------------------------------

#Under OpenAvnu
$ rmmod igb
$ modprobe i2c_algo_bit
$ modprobe dca
$ modprobe ptp
$ insmod lib/igb_avb/kmod/igb_avb.ko
$ ethtool -i eth1
driver: igb_avb
version: 5.3.2_AVB
firmware-version: 3.25, 0x800006eb
expansion-rom-version:
bus-info: 0000:04:00.0
supports-statistics: yes
supports-test: yes
supports-eeprom-access: yes
supports-register-dump: yes
supports-priv-flags: no

$ ethtool -T eth1
Time stamping parameters for eth1:
Capabilities:
        hardware-transmit     (SOF_TIMESTAMPING_TX_HARDWARE)
        software-transmit     (SOF_TIMESTAMPING_TX_SOFTWARE)
        hardware-receive      (SOF_TIMESTAMPING_RX_HARDWARE)
        software-receive      (SOF_TIMESTAMPING_RX_SOFTWARE)
        software-system-clock (SOF_TIMESTAMPING_SOFTWARE)
        hardware-raw-clock    (SOF_TIMESTAMPING_RAW_HARDWARE)
PTP Hardware Clock: 1
Hardware Transmit Timestamp Modes:
        off                   (HWTSTAMP_TX_OFF)
        on                    (HWTSTAMP_TX_ON)
Hardware Receive Filter Modes:
        none                  (HWTSTAMP_FILTER_NONE)
        all                   (HWTSTAMP_FILTER_ALL)


daemons (refer to run_daemons.sh)
----------------------------------

$ groupadd ptp > /dev/null 2>&1
$ cd gPTP_Path
(gPTP) $ build/gptp eth1 &

<<<< Or with SRP support, i.e. AVB_FEATURE_ENDPOINT=0 when building >>>>

(gPTP) $ cd OpenAvnu
(OpenAvnu) $ daemons/mrpd/mrpd -mvs -i eth1 &
(OpenAvnu) $ daemons/maap/linux/build/maap_daemon -i eth1 -d /dev/null
(OpenAvnu) $ daemons/shaper/shaper_daemon -d &


AVTP_PipeLine
---------------

cf. lib/avtp_pipeline/README.md
Run as Listener and Camera as Talker, connect with Fast-Connect

Stop
+++++

$ stop_avtp_pipeline.sh


Start Server
++++++++++++++

(lib/avtp_pipeline/build_avdecc/bin) $ ./openavb_avdecc  -I pcap:eth1 avdecc.ini example_listener.ini

For Fast-Connect, need a saved avdecc_save.ini to restore connection configuration.
cf. openavbAvdeccGetSaveStateInfo() in OpenAvnu/lib/avtp_pipeline/platform/Linux/avdecc/openavb_avdecc_read_ini.c:575
    for avdecc_save.ini format
For example:
    example_listener
    0
    4660
    00:11:22:ff:ff:33:44:55
    01:23:45:67:89:ab:cd:ef


Start Client
++++++++++++++

(lib/avtp_pipeline/build/bin) $ ./openavb_host -I pcap:eth1 example_listener.ini
<<<< Or without SRP support, i.e. AVB_FEATURE_ENDPOINT=0 when building >>>>
(lib/avtp_pipeline/build/bin) $ ./openavb_harness -I pcap:eth1 example_listener.ini

Camera
+++++++

a:\> enet init 00:11:22:33:44:55
[00179878][CA53_0]  UserPreInitCb invert GTX clk
[00179878][CA53_0] init PHC = 185
[00179878][CA53_0] Mac 0x4537 Phy@0x0 0x1CC915
[00179878][CA53_0]  ENET AmbaRTSL_EnetLinkup(303) 1000M
[00179878][CA53_0]  ENET AmbaRTSL_EnetLinkup(306) full duplex
[00179878][CA53_0]  ENET AmbaRTSL_EnetLinkup(375) Auto-Negotiation process completed

a:\> enet avbinit
a:\> enet avdecc 1 0
[00200646][CA53_0] Start Talker
a:\>
[00254953][CA53_0] Start streaming.....


You should get message in openavb_avdecc like:
[1539758146:552828156 OPENAVB ACMP] INFO: Listener example_listener attempting fast connect to flags=0x0000, talker_unique_id=0x1234, talker_entity_id=00:11:22:ff:ff:33:44:55, controller_entity_id=01:23:45:67:89:ab:cd:ef

