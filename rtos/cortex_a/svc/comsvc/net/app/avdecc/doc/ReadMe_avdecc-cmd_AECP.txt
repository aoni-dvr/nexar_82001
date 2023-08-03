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

Example: Reboot command only
------------------------------------

# ./avdecc 2 1500 eth1 aecp AEM_COMMAND 0x2a 00:11:22:33:44:55 00:11:22:FF:FF:33:44:55 168 000b0000
Sent:
                                      DA:00-11-22-33-44-55
                                      SA:00-1B-21-DE-D6-09
                               EtherType:0x22F0
                          Payload Length:0x001C
                            message_type:AEM_COMMAND
                                  status:SUCCESS
                     control_data_length:0x0010
                        target_entity_id:00:11:22:FF:FF:33:44:55
                    controller_entity_id:00:1B:21:FF:FE:DE:D6:09
                             sequence_id:0x00A8
                         u (unsolicited):false
                            command_type:REBOOT
                                 content:00 0B 00 00



Packet payload data: fb 00 00 10 00 11 22 40 00 33 44 55 00 1b 21 ff fe de d6 09 00 a8 00 2a 00 0b 00 00


And DUT will get
a:\> [00449827][CA53_0] AEM, REBOOT, descriptor_type: 0xB, descriptor_index: 0x0
