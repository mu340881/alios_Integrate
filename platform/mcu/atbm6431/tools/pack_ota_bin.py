#!/usr/bin/python

import string
import sys
import os
import re
import binascii
import struct
import zlib

chksum = 0

FILE_READ_SIZE=512

def write_file(file_name,data):
	if file_name is None:
		print 'file_name cannot be none\n'
		sys.exit(0)

	fp = open(file_name,'ab')

	if fp:
		fp.seek(0,os.SEEK_END)
		fp.write(data)
		fp.close()
	else:
		print '%s write fail\n'%(file_name)

def packotabin():
	if len(sys.argv) != 5:
		print 'Usage: pack_ota_bin.py  fw1.bin fw2.bin -o http_ota_fw_v100.bin'
		sys.exit(0)

	fw_updata1=sys.argv[1]
	fw_updata2=sys.argv[2]
	destfile=sys.argv[4]

	#print 'fw_updata1=%s \n'%(fw_updata1)
	#print 'fw_updata2=%s \n'%(fw_updata2)
	
	if os.path.exists(destfile):
		cmd='rm ' + destfile
		os.system(cmd)

	pSrcFd1 = open(fw_updata1, 'rb')
	if pSrcFd1 is None:
		print '%s cannot be open\n' % fw_updata1
		sys.exit(0)

	pSrcFd1.seek(0,os.SEEK_END)
	fw1_length = pSrcFd1.tell()
	pSrcFd1.seek(0,os.SEEK_SET)
	
	pSrcFd2 = open(fw_updata2, 'rb')
	if pSrcFd2 is None:
		print '%s cannot be open\n' % fw_updata2
		sys.exit(0)

	#print 'fw1_length=%d \n'%(fw1_length)
	
	pSrcFd2.seek(0,os.SEEK_END)
	fw2_length = pSrcFd2.tell()
	pSrcFd2.seek(0,os.SEEK_SET)
	
	#print 'fw2_length=%d \n'%(fw2_length)
	
	#file_total_length = fw2_length + fw1_length
	paddingfw1_length = 0
	paddingfw2_length = 0
	
	if fw1_length % FILE_READ_SIZE:
		paddingfw1_length = FILE_READ_SIZE - (fw1_length % FILE_READ_SIZE)
	if fw2_length % FILE_READ_SIZE:
		paddingfw2_length = FILE_READ_SIZE - (fw2_length % FILE_READ_SIZE)

	magic_data0 = 0x5a
	magic_data1 = 0x47
	version = 4338
	fw1_length = fw1_length + paddingfw1_length
	fw2_length = fw2_length
	reserved=0x0
	header = struct.pack('<BBHIIIIII', int(magic_data0), int(magic_data1),int(version), int(reserved), int(reserved), int(reserved), int(reserved), int(fw1_length),int(fw2_length))
	write_file(destfile, header)
	data_str = ['00']*(FILE_READ_SIZE - 28)
	data_bin = binascii.a2b_hex(''.join(data_str))
	write_file(destfile, data_bin)

	data_bin= pSrcFd1.read(int(fw1_length))
	write_file(destfile, data_bin)
	if paddingfw1_length:
		data_str = ['00']*(int(paddingfw1_length))
		data_bin = binascii.a2b_hex(''.join(data_str))
		write_file(destfile, data_bin)
	data_bin= pSrcFd2.read(int(fw2_length))
	write_file(destfile, data_bin)
	pSrcFd1.close()
	pSrcFd2.close()


if __name__=='__main__':
	packotabin()

