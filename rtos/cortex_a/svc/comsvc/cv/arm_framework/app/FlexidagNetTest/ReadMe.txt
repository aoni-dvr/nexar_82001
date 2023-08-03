Run :
	cd c:
	cv flexidag schdr start .
	cv flexidag run net_test c:\flexidag_run.txt

Argument :
	NUM_RUNS 10
		Total run which you want to run on this flexidag
		
	MEMIO_TYPE 0 (0:picinfo,1:multi-raw)
		Input indirect stucture support. 0 is memio picinfo and 1 is multi-raw.   
		
	OUTPUT_FROM_ARM 0
		Output cvtask from ARM side or ORC side, this is selected for cache handling.
		
	OUTPUT_TYPE 0 (0:raw)
		Output format support. 0 is for output raw data.

	OUTPUT_FILE 0
		Output data to file

	FLEXIDAG_PATH c:\flexidag_openseg/flexibin/flexibin0.bin
		Flexidag bin path

	RAW_PITCH 1376
		Pitch of input raw data, which needed by memio picinfo
		
	RAW_WIDTH 1360
		Width of input raw data, which needed by memio picinfo

	RAW_HEIGHT 768
		Height of input raw data, which needed by memio picinfo

	RAW0_Y_PATH c:\flexidag_openseg/golden/test.yuv
		The first part of input raw data file. Max support 4 file

	RAW0_UV_PATH NULL
		The second part of input raw data file if needed, and NULL if no need. Max support 4 file

	RESULT0_PATH c:\flexidag_openseg/golden/segout.bin
		The result file to compare with output. If there are more than one output, concatenate to one file.

