if SVC_APPS_EX

menu "EX Project"
menuconfig APP_PROJECT_NAME
    string "APP_PROJECT_NAME"
	default "EX"

	config DSP_WORK_SIZE
    hex "dsp work buffer size"
    default 0x0
endmenu

endif
