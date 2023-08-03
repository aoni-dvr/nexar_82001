#!/bin/bash

# Error out on error
set -e

# Verbose
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

SVC_ENABLE_SECURITY_SH_CMD=$1
shift

run_securebin_gen_cmd()
{
	while getopts "k:i:o:a:t:e:p:K:" OPT
	do
		case ${OPT} in
			k)
				KEY_FILE=${OPTARG};;
			K)
				KEY_ID=${OPTARG};;
			i)
				SECURE_SRC=${OPTARG};;
			o)
				SECURE_DST=${OPTARG};;
			a)
				ALIGN_256=${OPTARG};;
			t)
				TYPE_ENCRYPT=${OPTARG};;
			e)
				OPENSSL_PATH=${OPTARG};;
			p)
				ECC_HEADER_GEN=${OPTARG};;
		esac
	done

	if [ "${OPENSSL_PATH}" = "" ]; then
		OPENSSL_PATH=/usr/bin/openssl
	fi

	if [ "x${KEY_ID}" = "x" ]; then
		KEY_ID=0
	elif [  "${KEY_ID}" -gt "15" ]; then
		# NOTE: consist with range setting in Kconfig.
		#       Otherwise, to update for other design.
		KEY_ID=0
	fi

	echo "Gen signatured files ${SECURE_SRC} with ${OPENSSL_PATH}"
	# ---- For generating RSA signatured file including output.64/security/, SYS, ATF ---- #

	if [ -f "${KEY_FILE}" ]; then
		if [ -d "${SECURE_SRC}" ]; then
			# -- create array of the source binary file name
			SECURE_FILE_LIST=(`ls ${SECURE_SRC}`)
			for SECURE_FILE_NAME in ${SECURE_FILE_LIST[@]}
			do
				# -- signature file, only 2048 bits (256 bytes)
				SECURE_SIG_FILE=${SECURE_SRC}${SECURE_FILE_NAME}.sig
				# -- source file, the file which will be appended with signature file
				SECURE_SRC_FILE=${SECURE_SRC}${SECURE_FILE_NAME}

				#round up file size to multiple of 256 bytes
				if [ "${ALIGN_256}" = "align_256" ]; then
					truncate --size %256 ${SECURE_SRC_FILE}
				fi

				if [ "${TYPE_ENCRYPT}" = "ecc" ]; then
					if [ "${ECC_HEADER_GEN}" = "y" ]; then
						python ${srctree}/tools/hostutils/ust_header_modify.py ${SECURE_SRC_FILE} 0 ${KEY_ID} 0
					fi
					# -- ecc sign command with openssl
					${OPENSSL_PATH} pkeyutl -sign -inkey ${KEY_FILE} -out ${SECURE_SIG_FILE} -rawin -in ${SECURE_SRC_FILE}
				else
					# -- rsa sign command with openssl
					${OPENSSL_PATH} dgst -sha256 -sign ${KEY_FILE} -out ${SECURE_SIG_FILE} ${SECURE_SRC_FILE}
				fi

				# -- append signature to source file
				cat ${SECURE_SIG_FILE} >> ${SECURE_SRC_FILE}

				# -- remove signature file if needed
				#rm ${SECURE_SIG_FILE}
			done
		elif [ -f "${SECURE_SRC}" ]; then
			# -- signature file, only 2048 bits (256 bytes)
			SECURE_SRC_SIG_FILE=${SECURE_SRC}.sig

			#round up file size to multiple of 256 bytes
			if [ "${ALIGN_256}" = "align_256" ]; then
				truncate --size %256 ${SECURE_SRC}
			fi

			if [ "${TYPE_ENCRYPT}" = "ecc" ]; then
				if [ "${ECC_HEADER_GEN}" = "y" ]; then
					python ${srctree}/tools/hostutils/ust_header_modify.py ${SECURE_SRC} 0 ${KEY_ID} 0
				fi
				# -- ecc sign command with openssl
				${OPENSSL_PATH} pkeyutl -sign -inkey ${KEY_FILE} -out ${SECURE_SRC_SIG_FILE} -rawin -in ${SECURE_SRC}
			else
				# -- rsa sign command to ATF bin file
				${OPENSSL_PATH} dgst -sha256 -sign ${KEY_FILE} -out ${SECURE_SRC_SIG_FILE} ${SECURE_SRC}
			fi

			if [ "${SECURE_DST}" != "" ]; then
				cp ${SECURE_SRC} ${SECURE_DST}

				# -- append signature to dst file
				cat ${SECURE_SRC_SIG_FILE} >> ${SECURE_DST}
			else
				# -- append signature to source file
				cat ${SECURE_SRC_SIG_FILE} >> ${SECURE_SRC}
			fi

			# -- remove signature if needed
			#rm ${SECURE_ATF_SIG_FILE}
		else
			echo "Cannot find ${SECURE_SRC}"
		fi
	else
		echo "Cannot find ${KEY_FILE}"
	fi
}

run_pubkey_header_gen_cmd()
{
	# ---- For generating public key modulus based on the private key for RSA ---- #
	while getopts "k:o:" OPT
	do
		case ${OPT} in
			k)
				KEY_FILE=${OPTARG};;
			o)
				AUTO_PUB_KEY_FILE=${OPTARG};;
		esac
	done
	echo "Gen .autogen_pubkey"

    if [ -f ${AUTO_PUB_KEY_FILE} ]; then
        # -- remove the original one
        rm -f ${AUTO_PUB_KEY_FILE}
    fi

    if [ -f ${KEY_FILE} ]; then
        # -- generate public key modulus
        MODULUS_STR=`/usr/bin/openssl rsa -noout -in ${KEY_FILE} -modulus | sed 's/Modulus=//'`
        echo "#define RSA_N \"${MODULUS_STR}\"" >> ${AUTO_PUB_KEY_FILE}
        echo "#define RSA_E \"10001\"" >> ${AUTO_PUB_KEY_FILE}
	else
		echo "Cannot find ${KEY_FILE}"
    fi
}

echo "$0"
if [ $# -lt 2 ]; then
	echo "USAGE:"
	echo " securebin_gen -k [private key path] -i [input file/folder] -o [output file/folder] -a [align_256] -t [ecc/rsa] -e [openssl path] -p [y]"
	echo " pubkey_header_gen -k [private key path] -o [output file]"
else
	if [ "${SVC_ENABLE_SECURITY_SH_CMD}" = "securebin_gen" ]; then
		run_securebin_gen_cmd $@
	elif [ "${SVC_ENABLE_SECURITY_SH_CMD}" = "pubkey_header_gen" ]; then
		run_pubkey_header_gen_cmd $@
	fi
fi
