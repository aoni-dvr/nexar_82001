#!/bin/bash


E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" = "${0}" ]; then
    echo "Usage: source ${E_FILE}"
    exit
fi


# 'source' common patterns and variables
PATH_ENV=$(dirname ${E_FILE})
if [ "x${ENV_COMMON}" = "x" ]; then
    source ${PATH_ENV}/env_common.sh
    fn_amba_env_set
fi


function fn_amba_tw2_bashrc_set()
{
	if  [ $# == 0 ]; then
		PROJECT=""
		AMBA_ROOT="/opt/amba"
	elif  [ $# == 1 ]; then
		PROJECT=$1
		AMBA_ROOT="/opt/amba"
	else
		PROJECT=$1
		AMBA_ROOT=$2
	fi

	case $PROJECT in
	cv1)
		TV2_CONFIG=$AMBA_ROOT/$PROJECT/tv2/$PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		CVTOOL_VER="CVTOOLS sw-$PROJECT.1.8.3.562"

		LOCAL_CONFIG_FILE=~/.tv2.cv1u
		;;

	cv2)
		TV2_CONFIG=$AMBA_ROOT/$PROJECT/tv2/$PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		CVTOOL_VER="CVTOOLS sw-$PROJECT.2.0.1.714"

		LOCAL_CONFIG_FILE=~/.tv2.cv2u
		;;

	cv22|cv25|cv2s)
		__PROJECT=cv22
		TV2_CONFIG=$AMBA_ROOT/$__PROJECT/tv2/$__PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$__PROJECT/local/bin
			    $AMBA_ROOT/$__PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		CVTOOL_VER="CVTOOLS sw-$__PROJECT.2.0.1.714"

		LOCAL_CONFIG_FILE=~/.tv2.cv22u
		;;
	cv2fs|cv22fs)
		__PROJECT=cv2fs
		TV2_CONFIG=$AMBA_ROOT/$__PROJECT/tv2/$__PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$__PROJECT/local/bin
			    $AMBA_ROOT/$__PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		CVTOOL_VER="CVTOOLS sw-$__PROJECT.2.0.1.714"

		LOCAL_CONFIG_FILE=~/.tv2.cv2fsu
		;;
	cv2a)
		TV2_CONFIG=$AMBA_ROOT/$PROJECT/tv2/$PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		# CVTOOL_VER="CVTOOLS sw-daily"
		# Use particular verion in cvtools sw-daily release
		CVTOOL_VER="libvdg           cv2a.qa.2020-05-11.76
VpRef            cv2a.qa.2020-05-11.76
Zamboni          cv2a.qa.2020-05-11.76
Vas              cv2a.qa.2020-05-11.76
AmbaCnn          cv2a.qa.2020-05-11.76
AmbaCnnUtils     cv2a.qa.2020-05-11.76
gentask          cv2a.qa.2020-05-11.76
CnnUtils         cv2a.qa.2020-05-11.76
AmbaCnnUtils     cv2a.qa.2020-05-11.76
gentask          cv2a.qa.2020-05-11.76
CnnUtils         cv2a.qa.2020-05-11.76
CnnUtils         cv2a.qa.2020-05-11.76
Amalgam          cv2a.qa.2020-05-11.76
AmbaCnn          cv2a.qa.2020-05-11.76
AmbaCnnUtils     cv2a.qa.2020-05-11.76
gentask          cv2a.qa.2020-05-11.76
CnnUtils         cv2a.qa.2020-05-11.76
libamcl          cv2.002
sfc              1.01
ctc              1.00
pdc              2.00
TableArchive     1.03
LibHostapi       1.01"

		LOCAL_CONFIG_FILE=~/.tv2.cv2au
		;;
	cv28)
		TV2_CONFIG=$AMBA_ROOT/$PROJECT/tv2/$PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		# CVTOOL_VER="CVTOOLS sw-daily"
		# Use particular verion in cvtools sw-daily release
		CVTOOL_VER="libvdg           cv28.qa.2020-09-02.773
VpRef            cv28.qa.2020-09-02.773
Amalgam          cv28.qa.2020-09-02.773
Zamboni          cv28.qa.2020-09-02.773
Vas              cv28.qa.2020-09-02.773
AmbaCnn          cv2.qa.2020-09-02.773
AmbaCnnUtils     cv2.qa.2020-09-02.773
gentask          cv2.qa.2020-09-07.779
CnnUtils         cv2.qa.2020-09-02.773"

		LOCAL_CONFIG_FILE=~/.tv2.cv28u
		;;
	cv5)
		TV2_CONFIG=$AMBA_ROOT/$PROJECT/tv2/$PROJECT/tv2config
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"
		CVTOOL_VER="AmbaCnn          cv2.qa.2021-05-18.1040.ubu1804
AmbaCnnUtils     cv2.qa.2021-05-18.1040.ubu1804
gentask          cv2.qa.2021-05-18.1040.ubu1804
CnnUtils         cv2.qa.2021-05-18.1040.ubu1804
CommonCnnUtils   cv2.qa.2021-05-18.1040.ubu1804
frameworklibs    cv2.qa.2021-05-18.1040.ubu1804
libvdg           cv5.qa.2021-05-18.1040.ubu1804
VpRef            cv5.qa.2021-05-18.1040.ubu1804
Amalgam          cv5.qa.2021-05-18.1040.ubu1804
Zamboni          cv5.qa.2021-05-18.1040.ubu1804
Vas              cv5.qa.2021-05-18.1040.ubu1804
AdesRuntime      cv5.qa.2021-05-18.1040.ubu1804
VpDebugUtils     cv5.qa.2021-05-18.1040.ubu1804"

		LOCAL_CONFIG_FILE=~/.tv2.cv5u
		;;
	h32)
		return
		;;
	*)
		echo $"Error!!! Unsupported chip, please source env_set.sh with chip name"
		echo $"Usage: source ${PATH_ENV}/env_set.sh [CHIP]"
		echo $"       CHIP,  If use cv tools, please source with chip name"
		echo $"              e.g. cv2, cv22, cv25, cv28, cv2fs, cv22fs, cv5"
		return
	esac

	CVPATH=
	for i in ${CVTOOL_DIR}; do
		CVPATH=${i}:${CVPATH}
	done

	# Remove old the same one
	PATH=`echo ${PATH} | sed -e "s!${CVPATH}!!g"`

	# Ignore 1st ':'
	PATH=${CVPATH}${PATH}

	if [ -z ${org_ld_path+x} ]; then
		export org_ld_path=$LD_LIBRARY_PATH;
	fi
	export USE64BIT=1
	export AMBA_ROOT
	export PROJECT
	export TV2_CONFIG
	export PATH
	export CVTOOL_VER

	if [ -e $LOCAL_CONFIG_FILE ]; then
		mv -vf $LOCAL_CONFIG_FILE $LOCAL_CONFIG_FILE.bak
	fi

	echo "$CVTOOL_VER" > $LOCAL_CONFIG_FILE
	echo "Sepcify CVTOOLS version: \"$CVTOOL_VER in $LOCAL_CONFIG_FILE\""
	echo PROJECT=\"$PROJECT\"
#	echo USE64BIT=\"$USE64BIT\"
	echo AMBA_ROOT=\"$AMBA_ROOT\"
	echo TV2_CONFIG=\"$TV2_CONFIG\"
	echo CVTOOL_DIR=\"$CVTOOL_DIR\"
}

function _tw2_bashrc_unset()
{
	PROJECT=$1

	case $PROJECT in
	cv1)
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv1u
		;;

	cv2)
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv2u
		;;

	cv22|cv25|cv2s)
		__PROJECT=cv22
		CVTOOL_DIR="$AMBA_ROOT/$__PROJECT/local/bin
			    $AMBA_ROOT/$__PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv22u
		;;
	cv2fs|cv22fs)
		__PROJECT=cv2fs
		CVTOOL_DIR="$AMBA_ROOT/$__PROJECT/local/bin
			    $AMBA_ROOT/$__PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv2fsu
		;;
	cv2a)
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv2au
		;;
	cv28)
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv28u
		;;
	cv5)
		CVTOOL_DIR="$AMBA_ROOT/$PROJECT/local/bin
			    $AMBA_ROOT/$PROJECT/tv2/exe
			    $AMBA_ROOT/cv2/tv2/exe
			    $AMBA_ROOT/cv2_third/tv2/exe
			    $AMBA_ROOT/base/tv2/exe"

		LOCAL_CONFIG_FILE=~/.tv2.cv5u
		;;
	h32)
		return
		;;
	*)
		echo $"Error!!! Unsupported chip name \"$PROJECT\" "
		return
	esac

	CVPATH=
	for i in ${CVTOOL_DIR}; do
		CVPATH=${i}:${CVPATH}
	done

	# Remove CVPATH
	PATH=`echo ${PATH} | sed -e "s!${CVPATH}!!g"`

	export PATH

	if [ -e $LOCAL_CONFIG_FILE.bak ]; then
		mv -vf $LOCAL_CONFIG_FILE.bak $LOCAL_CONFIG_FILE
	fi
}

function fn_amba_tw2_bashrc_unset()
{
	if  [ $# == 0 ]; then
		PROJECT="cv2 cv22 cv25 cv28 cv2fs cv22fs cv5"
	elif  [ $# == 1 ]; then
		PROJECT=$1
	else
		PROJECT=$1
		AMBA_ROOT=$2
	fi

	for i in ${PROJECT}; do
		_tw2_bashrc_unset ${i}
	done

	unset org_ld_path
	unset USE64BIT
	unset AMBA_ROOT
	unset CVTOOL_VER
	unset PROJECT
	unset TV2_CONFIG
}

