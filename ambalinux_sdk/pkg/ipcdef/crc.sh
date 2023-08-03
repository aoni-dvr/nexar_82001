#This shell script is used to check crc value of rpc headers and svae them to a header file.
#$1: The generated crc.h.
#$2: The macro definition for the generated crc.h.
#$3: The directory we'll looking for rpc headers.
#$4: The parameter name for the array to store crc values.
#$5: The parameter name for the array to store prog id.
#$6: The parameter name for the number of rpc header.

if [ -e $1 ]; then
	rm -f $1
fi
FILE_LIST=`ls $3 | grep "\.h"`
echo $FILE_LIST
TEMP="#ifndef "$2
echo $TEMP > tmp
TEMP="#define "$2
echo $TEMP >> tmp
NEED_COMMA=0
COUNT=0
CRC_COLLECTION="unsigned int "$4"[]={"
for i in $FILE_LIST
do
	if [ "$i" = "aipc_crc.h" -o "$i" = "crc.h" -o "$i" = "priv_crc.h" -o "$i" = "aipc_priv_crc.h" -o "$i" = "AmbaIPC_Rpc_Def.h" ]; then
		continue
	fi
	if [ $NEED_COMMA -eq 1 ]; then
		CRC_COLLECTION=$CRC_COLLECTION", "
	fi

	CRC=`./crc_check $3$i`
	CRC_COLLECTION=$CRC_COLLECTION$CRC"U"
	COUNT=`expr $COUNT + 1`
	NEED_COMMA=1
done
CRC_COLLECTION=$CRC_COLLECTION"};"
PROG_ID="int "$5"[]={"
NEED_COMMA=0
for i in $FILE_LIST
do
	if [ "$i" = "aipc_crc.h" -o "$i" = "crc.h" -o "$i" = "priv_crc.h" -o "$i" = "aipc_priv_crc.h" -o "$i" = "AmbaIPC_Rpc_Def.h" ]; then
		continue
	fi
	if [ $NEED_COMMA -eq 1 ]; then
		PROG_ID=$PROG_ID", "
	fi
	ID=`cat $3$i | grep "PROG_ID" | grep "#define" |awk '{print $3}'| awk -F '
' '{print $1}'`
	PROG_ID=$PROG_ID$ID
	NEED_COMMA=1
done
PROG_ID=$PROG_ID"};"
echo $CRC_COLLECTION >> tmp
echo $PROG_ID >> tmp
RPCNUM="int "$6" = "$COUNT";"
echo $RPCNUM >> tmp
echo "#endif" >> tmp

#remove the windows character in the end of line
sed -e 's///g' tmp > $1
rm -f tmp
