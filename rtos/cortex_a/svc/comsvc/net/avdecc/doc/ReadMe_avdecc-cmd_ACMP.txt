OpenAvnu (as Talker/Listener)
==============================

- Version
	- SHA: 8fa80d1de59fbef14c8d0e80d8d9a62679a6182a (2018-10-08)
		- a04fef499f843b8a7f596bc15441a847b9a18b7a avdecc-lib (remotes/origin/avnu-master)
		- 45e5262677f07c7c35f4bbdb12235a679b79e82c lib/igb_avb (heads/master)
		- 1d95a3905413d99fddb5bcbd30be35a16dbf9119 thirdparty/cpputest (v3.4-2273-g1d95a390)
	- Patches:
        diff --git a/lib/avtp_pipeline/acmp/openavb_acmp_message.c b/lib/avtp_pipeline/acmp/openavb_acmp_message.c
        index 8969f9cd..8a68de0a 100644
        --- a/lib/avtp_pipeline/acmp/openavb_acmp_message.c
        +++ b/lib/avtp_pipeline/acmp/openavb_acmp_message.c
        @@ -177,7 +177,11 @@ static void openavbAcmpMessageRxFrameParse(U8* payload, int payload_len, hdr_inf
                        BIT_B2DNTOHB(pDst1->sv, pSrc, 0x80, 7, 0);
                        BIT_B2DNTOHB(pDst1->version, pSrc, 0x70, 4, 0);
                        BIT_B2DNTOHB(pDst2->message_type, pSrc, 0x0f, 0, 1);
        +#if 0
                        BIT_B2DNTOHB(pDst2->status, pSrc, 0xf800, 11, 0);
        +#else
        +               BIT_B2DNTOHS(pDst2->status, pSrc, 0xf800, 11, 0);
        +#endif
                        BIT_B2DNTOHS(pDst1->control_data_length, pSrc, 0x07ff, 0, 2);
                        OCT_B2DMEMCP(pDst2->stream_id, pSrc);

        diff --git a/lib/avtp_pipeline/avtp_pipeline.mk b/lib/avtp_pipeline/avtp_pipeline.mk
        index e08ac9b9..f6204635 100644
        --- a/lib/avtp_pipeline/avtp_pipeline.mk
        +++ b/lib/avtp_pipeline/avtp_pipeline.mk
        @@ -1,6 +1,7 @@
        -AVB_FEATURE_ENDPOINT ?= 1
        +### Building AVTP pipeline without SRP or MAAP support
        +AVB_FEATURE_ENDPOINT ?= 0
         IGB_LAUNCHTIME_ENABLED ?= 0
        -AVB_FEATURE_GSTREAMER ?= 0
        +AVB_FEATURE_GSTREAMER ?= 1
         PLATFORM_TOOLCHAIN ?= generic

         .PHONY: all clean


- Install igb_avb.ko
- gPTP
	- SHA: 98e3adaeb4c5a9534d24aee166d3690ea6a5c7f4 (2018-09-04)
		- Would need to enlarge 'neighborPropDelayThresh' of gptp_cfg.ini
	- Command
		# build/gptp eth1
- Server
	- Get echo_talker.ini echo_listener_auto.ini from lib/avtp_pipeline/build/bin/
	- Command
		# cd lib/avtp_pipeline/build_avdecc/bin/ && ./openavb_avdecc  -I pcap:eth1 avdecc.ini echo_talker.ini echo_listener_auto.ini
- Host
	- Make sure avdecc_save.ini is empty or not exist, otherwise it will do auto-fast-connect
	- Command
		# cd lib/avtp_pipeline/build/bin/ && ./openavb_host -I pcap:eth1 echo_talker.ini echo_listener_auto.ini

- Notes:
	- OpenAvnu use MAC as Entity ID (EID), e.g. 00:1b:21:de:d6:09 will be 0x001b21-fffe-ded609
		- cf. openavbReadAvdeccConfig()() and openavbAemDescriptorEntitySet_entity_id()
	- The Talker/Listener/Controller on the same machine use the same EID.
	Unique ID (UID) of Talker and Listener should be the same of StreamInfo (index) which start from 0

DUT (as Talker/Listener)
==========================

- Command
	> enet init 00:11:22:33:44:55
	> enet avbinit
	(or) > enet avbmaster
	> enet avdecc 1 1

- Note:
	- The UID rule is
		- Controller = 0x8000 + Instance_index
		- Talker     = 0x4000 + Instance_index
		- Listener   = 0x2000 + Instance_index
		- Responder  = 0x1000 + Instance_index
		- cf. avdecc_GetUniqueId()

	- The Talker/Listener/Controller on the same machine use the different EID,
	  e.g. 00:11:22:33:44:55 will be 0x001122-UID-334455 where,
	  cf. avdecc_GenEID()

avdecc-cmd (as Controller)
============================

avdecc usage:
        avdecc [verbosity] [timeout_in_ms] [network_port] [protocol] ...

        avdecc [verbosity] [timeout_in_ms] [network_port] adp [message_type] (entity_id)

        avdecc [verbosity] [timeout_in_ms] [network_port] acmp [message_type] [sequence_id] [talker_entity_id]
                [talker_unique_id] [listener_entity_id] [listener_unique_id]

        avdecc [verbosity] [timeout_in_ms] [network_port] acmp [message_type] [sequence_id] [talker_entity_id]
                [talker_unique_id] [listener_entity_id] [listener_unique_id] [connection_count]

        avdecc [verbosity] [timeout_in_ms] [network_port] aecp AEM_COMMAND [command] [destination_mac] [target_entity_id] [sequence_id]
                [payload...]

- Suppose
	- OpenAvnu (O):
		- Entity ID (EID): 00:1b:21:ff:fe:de:d6:09
		- Talker Unique ID (UID): 0
		- Listener UID: 0
	- DUT (D)
		- Entity ID (EID): 00:11:22:ff:ff:33:44:55
		- Talker Unique ID (UID): 0x4000
		- Listener UID: 0x2000

Example: DUT.Talker to OpenAvnu.Listener
------------------------------------------

##### CONNECT_RX_COMMAND, should be success
# avdecc 2 1500 eth1 acmp CONNECT_RX_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_RX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 06 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
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
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_TX_RESPONSE
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:91-E0-F0-00-0E-80
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_RX_RESPONSE
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:91-E0-F0-00-0E-80
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ SAVED_STATE ]
                          stream_vlan_id:0x0000


##### CONNECT_RX_COMMAND, should be failed
# avdecc 2 1500 eth1 acmp CONNECT_RX_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_RX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 06 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
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
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_TX_RESPONSE
                                  status:TALKER_EXCLUSIVE
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:CONNECT_RX_RESPONSE
                                  status:TALKER_EXCLUSIVE
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000


##### GET_TX_STATE_COMMAND, should be failed for DUT, but success for OpenAvnu
# avdecc 2 1500 eth1 acmp GET_TX_STATE_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_TX_STATE_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 04 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_TX_STATE_RESPONSE
                                  status:STATE_UNAVAILABLE
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000


##### GET_RX_STATE_COMMAND, should be failed for DUT, but success for OpenAvnu
# avdecc 2 1500 eth1 acmp GET_RX_STATE_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_RX_STATE_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 0a 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_RX_STATE_RESPONSE
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:91-E0-F0-00-0E-80
                        connection_count:0x0001
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000


##### GET_TX_CONNECTION_COMMAND, should be failed for DUT
# avdecc 2 1500 eth1 acmp GET_TX_CONNECTION_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_TX_CONNECTION_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 0c 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:GET_TX_CONNECTION_RESPONSE
                                  status:NOT_SUPPORTED
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000


##### DISCONNECT_RX_COMMAND, should be success
# avdecc 2 1500 eth1 acmp DISCONNECT_RX_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_RX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 08 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_TX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_TX_RESPONSE
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_RX_RESPONSE
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000


##### DISCONNECT_RX_COMMAND, should be failed
# avdecc 2 1500 eth1 acmp DISCONNECT_RX_COMMAND 168 00:11:22:FF:FF:33:44:55 0x4000 00:1b:21:ff:fe:de:d6:09 0
Sent ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_RX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Packet payload data: fc 08 00 2c 00 00 00 00 00 00 00 00 00 1b 21 ff fe de d6 09 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 40 00 00 00 00 00 00 00 00 00 00 00 00 a8 00 00 00 00 00 00
Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_TX_COMMAND
                                  status:SUCCESS
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-11-22-33-44-55
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_TX_RESPONSE
                                  status:NOT_CONNECTED
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000

Received ACMPDU:
                                      DA:91-E0-F0-01-00-00
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x0038
                            message_type:DISCONNECT_RX_RESPONSE
                                  status:NOT_CONNECTED
                     control_data_length:0x002C
                               stream_id:00-00-00-00-00-00:00-00
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_entity_id:00:11:22:FF:FF:33:44:55
                      listener_entity_id:00:1B:21:FF:FE:DE:D6:09
                        talker_unique_id:0x4000
                      listener_unique_id:0x0000
                         stream_dest_mac:00-00-00-00-00-00
                        connection_count:0x0000
                             sequence_id:0x00A8
                                   flags:[ ]
                          stream_vlan_id:0x0000
