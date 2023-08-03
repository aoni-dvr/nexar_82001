1. Prepare:
   prepare orcvp.bin and schdrlog.bin
   --- Linux ---
   A. Use "dmesg" to dump ambacv kernel driver log and find out the schdr log buffer physical adderess(VP: SCHED).

	==> ambacv driver log
	[    9.038801]  VP:   [0x50400000 ++ 0x00078964]
	[    9.083893] start visorc
	[    9.132872] =============== Log Buffer Info ===============
	[    9.138446]    VP:CVTASK: @ [0x53c97f00--0x53cdfeb7] (294840)
	[    9.144185]    VP: SCHED: @ [0x53c4ff00--0x53c97eb7] (294840)
	==> log buffer physical adderess: 0x53c4ff00, size: 294840(DEC)

   B. Use JTAG to dump the schdr print buffer to schdrlog.bin
   C. Use JTAG to dump orcvp.bin or use original orcvp.bin

   --- RTOS ---
   A. Use "cv log_console 0x3" to enable cv debug print and find the schdr log buffer physical adderess(VP: SCHED).

	==> RTOS log
	Loading c:\orc_scheduler\visorc\orcvp.bin, Fsize=478111, addr=0x50400000
	[00723901][CA53_0] =============== Log Buffer Info ===============
	[00723901][CA53_0]   VP:CVTASK: @
	[00723901][CA53_0] [0x57C97F00--0x57CDFEB7] (294840)
	[00723901][CA53_0]   VP: SCHED: @
	[00723901][CA53_0] [0x57C4FF00--0x57C97EB7] (294840)
	[00723901][CA53_0]   VP:  PERF: @
	==> log buffer physical adderess: 0x57C4FF00, size: 294840(DEC)

   B. Use JTAG to dump the schdr print buffer to schdrlog.bin
   C. Use JTAG to dump orcvp.bin or use original orcvp.bin

2. Build:
   $ make

3. Run:
   Use host_cvschdr_log to dump the schdr log.
   $ ./host_cvschdr_log -c orcvp.bin -b schdrlog.bin -o log.txt
