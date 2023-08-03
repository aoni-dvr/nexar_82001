import os
import sys
import struct

def ust_header_modify(FileName, Version, KeyID, SignedLength):
    if os.path.exists(FileName):
        file_size = os.path.getsize(FileName)
        if (file_size < SignedLength):
            print("Error: " + FileName + " has " + str(file_size) + "bytes, but SignedLength is bigger(" +  str(SignedLength) + ")")
            SignedLength = file_size
        bin_file = open(FileName, 'r+b')
        if bin_file != None:
            # update version (2 bytes)
            bin_file.write(struct.pack('H', Version))
            # update key_id (2 bytes)
            bin_file.write(struct.pack('H', KeyID))
            # update signed length (4 bytes)
            if SignedLength == 0:
                # use file size
                bin_file.write(struct.pack('I', file_size))
            else:
                bin_file.write(struct.pack('I', SignedLength))
            # update total length (4 bytes), it would be file_size + 64 bytes
            bin_file.write(struct.pack('I', file_size + 64))

            bin_file.close()
            print(FileName + " modified for UST header.")
        else:
            print("Can't open " + FileName);
    else:
        print(FileName + " doesn't exist")

def usage_print():
    print("ust_header_modify [filepath] [version] [key id] [signed length]")

if (len(sys.argv) < 5):
    usage_print()
else:
    file_path = sys.argv[1]
    version = int(sys.argv[2])
    key_id = int(sys.argv[3])
    signed_length = int(sys.argv[4])
    ust_header_modify(file_path, version, key_id, signed_length)