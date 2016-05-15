/***********************************************************
*
* Copyright (C), 2010-2013, WANGFUYU.
* All rights reserved.
*
* File name  : cloud-lzma.c
* Version    : 1.0 
* Description: compress data by lzma 
*
* Author     : Wang FuYu <wangfuyu1989@foxmail.com>
* Create Date: 2015-03-30
* 
* History    : 
*------------------------------------------------------------
*
*01, 30Mar15, Wang FuYu, create file.
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <LzmaLib.h>
#include "cloud-lzma.h"
#include "helloworld.h"

static int lzma_compress(void *strm, void *dest, void *src, int size, int block_size,
		int *error)
{
	unsigned char *d = dest;
	size_t props_size = LZMA_PROPS_SIZE,
	outlen = block_size - LZMA_HEADER_SIZE;
	int res;

	res = LzmaCompress(dest + LZMA_HEADER_SIZE, &outlen, src, size, dest,
		&props_size, 5, block_size, 3, 0, 2, 32, 1);
	
	if(res == SZ_ERROR_OUTPUT_EOF) {
		/*
		 * Output buffer overflow.  Return out of buffer space error
		 */
		return 0;
	}

	if(res != SZ_OK) {
		/*
		 * All other errors return failure, with the compressor
		 * specific error code in *error
		 */
		*error = res;
		return -1;
	}

	/*
	 * Fill in the 8 byte little endian uncompressed size field in the
	 * LZMA header.  8 bytes is excessively large for squashfs but
	 * this is the standard LZMA header and which is expected by the kernel
	 * code
	 */
	d[LZMA_PROPS_SIZE] = size & 255;
	d[LZMA_PROPS_SIZE + 1] = (size >> 8) & 255;
	d[LZMA_PROPS_SIZE + 2] = (size >> 16) & 255;
	d[LZMA_PROPS_SIZE + 3] = (size >> 24) & 255;
	d[LZMA_PROPS_SIZE + 4] = 0;
	d[LZMA_PROPS_SIZE + 5] = 0;
	d[LZMA_PROPS_SIZE + 6] = 0;
	d[LZMA_PROPS_SIZE + 7] = 0;

	/*
	 * Success, return the compressed size.  Outlen returned by the LZMA
	 * compressor does not include the LZMA header space
	 */
	return outlen + LZMA_HEADER_SIZE;
}


static int lzma_uncompress(void *dest, void *src, int size, int block_size,
	int *error)
{
	unsigned char *s = src;
	size_t outlen, inlen = size - LZMA_HEADER_SIZE;
	int res;

	outlen = s[LZMA_PROPS_SIZE] |
		(s[LZMA_PROPS_SIZE + 1] << 8) |
		(s[LZMA_PROPS_SIZE + 2] << 16) |
		(s[LZMA_PROPS_SIZE + 3] << 24);

	res = LzmaUncompress(dest, &outlen, src + LZMA_HEADER_SIZE, &inlen, src,
		LZMA_PROPS_SIZE);
	
	*error = res;
	return res == SZ_OK ? outlen : -1;
}

int getDstLen(void *src, int *len)
{
	unsigned char *s = src;

	*len = s[LZMA_PROPS_SIZE] |
		(s[LZMA_PROPS_SIZE + 1] << 8) |
		(s[LZMA_PROPS_SIZE + 2] << 16) |
		(s[LZMA_PROPS_SIZE + 3] << 24);

	return 0;
}

/************************************************************
 Function   : lzmaCompData
 Author     : Wang FuYu <wangfuyu1989@foxmail.com>
 Description: compress data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int lzmaCompData(char *srcFileName, char *dstFileName)
{
	int ret = -1;
	int error = 0;
	int srcLen = 0;
	int dstLen = 0;
	FILE *srcFd = NULL;
	FILE *dstFd = NULL;
	unsigned char *srcBuf = NULL;
	unsigned char *dstBuf = NULL;

	if (NULL == srcFileName || NULL == dstFileName)
	{
		MSG_PRINT("srcFileName or dstFileName is NULL!\n");
		return -1;
	}

	ret = getFileContentLen(srcFileName, &srcLen);
	if (0 != ret)
	{
		MSG_PRINT("get file<%s> length failed.\n", srcFileName);
		return -1;
	}

	dstLen = srcLen;
	srcBuf = (unsigned char*)malloc(sizeof(unsigned char) * srcLen);
	dstBuf = (unsigned char*)malloc(sizeof(unsigned char) * dstLen);
	if (NULL == srcBuf || NULL == dstBuf)  
	{  
		MSG_PRINT("No enough memory!\n");
		if (srcBuf) 
		{
			FREE_P(srcBuf);
		}
		
		if (dstBuf)
		{
			FREE_P(dstBuf);
		}
		
		return -1;  
	}	

	ret = writeFilesToBuf(srcFileName, srcBuf, srcLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte %s to buf failed.\n", srcFileName);
		return -1;
	}
	
	MSG_PRINT("source file, len is %d, content is\n%s\n", srcLen, srcBuf);
	dstLen = lzma_compress(NULL, dstBuf, srcBuf, srcLen, SLP_LZMA_DICT_SIZE, &error);
	if (0 >= dstLen)
	{
		MSG_PRINT("lzma compress data failed, error is %d.\n", error);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	MSG_PRINT("After lzma compress, len is %d, data is:\n%s\n", dstLen, dstBuf);


	ret = writeBufToFile(dstFileName, dstBuf, dstLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte buf to %s failed.\n", dstFileName);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	
	FREE_P(srcBuf);
	FREE_P(dstBuf);
	return 0;
}

/************************************************************
 Function   : lzmaUncompData
 Author     : Wang FuYu <wangfuyu1989@foxmail.com>
 Description: uncompress data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int lzmaUncompData(char *srcFileName, char *dstFileName)
{
	int ret = -1;
	int error = 0;
	int srcLen = 0;
	int dstLen = 0;
	FILE *srcFd = NULL;
	FILE *dstFd = NULL;
	unsigned char *srcBuf = NULL;
	unsigned char *dstBuf = NULL;

	if (NULL == srcFileName || NULL == dstFileName)
	{
		MSG_PRINT("srcFileName or dstFileName is NULL!\n");
		return -1;
	}

	ret = getFileContentLen(srcFileName, &srcLen);
	if (0 != ret)
	{
		MSG_PRINT("get file<%s> length failed.\n", srcFileName);
		return -1;
	}

	srcBuf = (unsigned char*)malloc(sizeof(unsigned char) * srcLen);
	if (NULL == srcBuf)  
	{  
		MSG_PRINT("No enough memory for src buf!\n");
		return -1;
	}	

	ret = writeFilesToBuf(srcFileName, srcBuf, srcLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte %s to buf failed.\n", srcFileName);
		FREE_P(srcBuf);
		return -1;
	}

	ret = getDstLen(srcBuf, &dstLen);
	if (0 != ret)
	{
		MSG_PRINT("get destination length failed.\n");
		FREE_P(srcBuf);
		return -1;
	}

	dstBuf = (unsigned char*)malloc(sizeof(unsigned char) * dstLen);
	if (NULL == dstBuf)  
	{  
		MSG_PRINT("No enough memory for dst buf!\n");
		FREE_P(srcBuf);
		return -1;
	}
	
	MSG_PRINT("lzma uncompress file, srcLen is %d, dstLen is %d.\n", srcLen, dstLen);
	dstLen = lzma_uncompress(dstBuf, srcBuf, srcLen, SLP_LZMA_DICT_SIZE, &error);
	if (0 >= dstLen)
	{
		MSG_PRINT("lzma uncompress data failed, error is %d.\n", error);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	MSG_PRINT("After lzma uncompress, len is %d, data is:\n%s\n", dstLen, dstBuf);

	ret = writeBufToFile(dstFileName, dstBuf, dstLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte buf to %s failed.\n", dstFileName);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}

	FREE_P(srcBuf);
	FREE_P(dstBuf);
	return 0;
} 

/************************************************************
 Function   : lzmaDataProg
 Author     : Wang FuYu <wangfuyu1989@foxmail.com>
 Description: process data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int lzmaDataProg(char *src, char *dst, unsigned short type)
{
	int ret = -1;
	
	MSG_PRINT("Enter lzma data program...\n");
	if (CLOUD_LZMA_COMPRESS == type)
	{
		ret = lzmaCompData(src, dst);
	}
	else if (CLOUD_LZMA_UNCOMPRESS == type)
	{
		ret = lzmaUncompData(src, dst);
	}

	if (0 != ret)
	{
		MSG_PRINT("cloud (un)compress error.\n");
		return -1;
	}
	
	return 0;
}

