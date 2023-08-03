
avdecc-cmd: Refer to https://github.com/jdkoftinoff/avdecc-cmd

Test Case
===========

* Camera run as Talker, i.e. stream out
* avdecc-cmd run as Controller/Listener.
* Fast-Connect scenario:
    1. Listener tries to discover entities in the network by ADP message.
    2. Talker get ADP message and do advertisement.
    3. Listener get advertisement message from Talker and send CONNECT_TX_COMMAND.
    4. Talker get CONNECT_TX_COMMAND and start to stream and return CONNECT_TX_RESPONSE.

Logs
=====

Camera side
-------------

a:\> enet init 00:11:22:33:44:55
[00011869][CA53_0]  UserPreInitCb invert GTX clk
[00011869][CA53_0] init PHC = 17
[00011869][CA53_0] Mac 0x4537 Phy@0x0 0x1CC915
[00011869][CA53_0]  ENET AmbaRTSL_EnetLinkup(303) 1000M
[00011869][CA53_0]  ENET AmbaRTSL_EnetLinkup(306) full duplex
[00011869][CA53_0]  ENET AmbaRTSL_EnetLinkup(375) Auto-Negotiation process completed
a:\> enet avbinit
a:\> enet avdecc 1 0
[00012319][CA53_0] Start Talker
a:\>
[00059246][CA53_0] Start streaming.....


adecc-cmd
-----------


# ./avdecc 2 1500 eth1 adp ENTITY_DISCOVER 00:11:22:FF:FF:33:44:55
Sent:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0044
                            message_type:ENTITY_DISCOVER
                    valid_time (seconds):0x0000
                     control_data_length:0x0038
                               entity_id:00:11:22:FF:FF:33:44:55

Packet payload data: fa 02 00 38 00 11 22 ff ff 33 44 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Received ADPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0044
                            message_type:ENTITY_AVAILABLE
                    valid_time (seconds):0x003E
                     control_data_length:0x0038
                               entity_id:00:11:22:FF:FF:33:44:55
                         available_index:0x00000001
                         entity_model_id:33:44:55:66:77:88:99:AA
                     entity_capabilities:[ AEM_SUPPORTED CLASS_A_SUPPORTED CLASS_B_SUPPORTED GPTP_SUPPORTED ]
                   talker_stream_sources:0x0001
                     talker_capabilities:[ IMPLEMENTED AUDIO_SOURCE VIDEO_SOURCE ]
                     gptp_grandmaster_id:01:23:45:67:89:AB:CD:EF
                      gptp_domain_number:0x00A5


[2018-10-17 17:57:35] # ./avdecc 2 1500 eth1 acmp  CONNECT_TX_COMMAND 1 00:11:22:FF:FF:33:44:55 0x1234 00:1b:21:ff:fe:de:d6:09 0x5678
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_TX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x1234
                      listener_unique_id:0x5678
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x0001
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 00 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 ff ff 33 44 55 00 1b 21 ff fe de d6 09 12 34 56 78 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00

