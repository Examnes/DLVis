#######################
# Powershell XOR 2 Files
# xor.py
# Jul 2016
# Website: http://www.Megabeets.net
# Use: ./xor.py file1 file2 outputFile
# Example: ./xor.py C:\a.txt C:\b.txt C:\result.txt
#######################

import sys

# Read two files as byte arrays
file1_b = bytearray(open(sys.argv[1], 'rb').read())
key = int(sys.argv[2])
size = len(file1_b)
# Set the length to be the smaller one
xord_byte_array = bytearray(size)

# XOR between the files
for i in range(size):
	xord_byte_array[i] = file1_b[i] ^ key

# Write the XORd bytes to the output file	
open(sys.argv[3], 'wb').write(xord_byte_array)