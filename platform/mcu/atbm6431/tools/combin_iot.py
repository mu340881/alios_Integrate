#!/usr/bin/python

import string
import sys
import os
import re
import binascii
import struct
import zlib

chksum = 0

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

def combin_iot():
	global chksum
	if len(sys.argv) != 10:
		print 'Usage: combin_iot.py  fw1.bin fw2.bin -o fw1_update.bin fw2_update.bin -p iot_flag iot_version iot_updataCnt'
		sys.exit(0)

	fw1				= sys.argv[1];
	fw2				= sys.argv[2];
	fw1_update		= sys.argv[4];
	fw2_update		= sys.argv[5];
	iot_flag			= sys.argv[7];
	iot_version		= sys.argv[8];
	iot_updataCnt		= sys.argv[9];
	iot_checksum =0

	if os.path.exists(fw1_update):
		cmd='rm ' + fw1_update
		os.system(cmd)
	if os.path.exists(fw2_update):
		cmd='rm ' + fw2_update
		os.system(cmd)

	#print 'fw1=%s' % fw1
	#print 'fw2=%s' % fw2
	#print 'fw1_update=%s' % fw1_update
	#print 'fw2_update=%s' % fw2_update
	#print 'iot_flag=%s' % iot_flag
	#print 'iot_version=%s' % iot_version
	#print 'iot_updataCnt=%s' % iot_updataCnt

	pSrcFd = open(fw1, 'rb')
	if pSrcFd is None:
		print '%s cannot be open\n' % fw1
		sys.exit(0)

	pSrcFd.seek(0,os.SEEK_END)
	iot_CODE1_len = pSrcFd.tell()

	#print 'iot_CODE1_len=%d' % iot_CODE1_len
	pSrcFd.close()
	need_extern= iot_CODE1_len & 0x1
	if need_extern:
		data_str = ['00']*(1)
		data_bin = binascii.a2b_hex(''.join(data_str))
		write_file(fw1, data_bin)
		iot_CODE1_len+=1

	pSrcFd = open(fw1,'rb')
	if pSrcFd is None:
		print '%s cannot be open\n' % fw1
		sys.exit(0)

	#print 'need_extern=%d' % need_extern
	
	#pSrcFd.seek(0,os.SEEK_SET)
	#print 'iot_CODE1_len=%d' % iot_CODE1_len
	
	data_bin = pSrcFd.read(int(iot_CODE1_len))
	write_file(fw1_update,data_bin)
	pSrcFd.close()

	for loop in range(0, len(data_bin), 2):
		#print 'xxxx chksum=0x%x data0=%x,data1=%x, %x' % (chksum,ord(data_bin[loop]),ord(data_bin[loop + 1]),ord(data_bin[loop + 1]) * 256)
		chksum += ord(data_bin[loop]) + ord(data_bin[loop + 1]) * 256
		#print 'chksum=%x' % chksum

	#print 'code1 chksum=0x%x' % chksum
	
	pSrcFd = open(fw2,'rb')
	if pSrcFd is None:
		print '%s cannot be open\n' % fw2
		sys.exit(0)

	pSrcFd.seek(0,os.SEEK_END)
	iot_CODE2_len = pSrcFd.tell()

	#print 'iot_CODE2_len=%d' % iot_CODE2_len
	pSrcFd.close()
	need_extern= iot_CODE2_len & 0x1
	if need_extern:
		data_str = ['00']*(1)
		data_bin = binascii.a2b_hex(''.join(data_str))
		pSrcFd.write(data_bin)
		write_file(fw2, data_bin)
		iot_CODE2_len+=1

	pSrcFd = open(fw2,'rb')
	if pSrcFd is None:
		print '%s cannot be open\n' % fw2
		sys.exit(0)
	#print 'code2 need_extern=%d' % need_extern

	#pSrcFd.seek(0,os.SEEK_SET)
	#print 'iot_CODE2_len=%d' % iot_CODE2_len
	data_bin = pSrcFd.read(int(iot_CODE2_len))
	write_file(fw2_update,data_bin)
	for loop in range(0, len(data_bin), 2):
		chksum += ord(data_bin[loop]) + ord(data_bin[loop + 1]) * 256
	pSrcFd.close()
	
	#print 'code2 chksum=0x%x' % chksum
	
	data_bin = struct.pack('<IIIIII',int(iot_flag), int(iot_version), int(iot_checksum), int(iot_updataCnt), int(iot_CODE1_len), int(iot_CODE2_len))
	for loop in range(0, len(data_bin), 2):
		chksum += ord(data_bin[loop]) + ord(data_bin[loop + 1]) * 256
		
	#print 'msghdr chksum=0x%x' % chksum
	
	iot_CODE2_externLen = 256 - (iot_CODE2_len % 256)
	if iot_CODE2_externLen == 256:
		iot_CODE2_externLen = 0
	if iot_CODE2_externLen:
		data_str = ['00']*(iot_CODE2_externLen)
		data_bin = binascii.a2b_hex(''.join(data_str))
		write_file(fw2_update,data_bin)
	chksum = chksum & 0xffff
	data_bin = struct.pack('<IIIIII',int(iot_flag), int(iot_version), int(chksum), int(iot_updataCnt), int(iot_CODE1_len), int(iot_CODE2_len))
	write_file(fw2_update,data_bin)

if __name__=='__main__':
	combin_iot()

