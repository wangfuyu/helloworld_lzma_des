/***********************************************************
*
* Copyright (C), 2010-2013, WANGFUYU.
* All rights reserved.
*
* File name  : cloud-lzma.h
* Version    : 1.0 
* Description: header file for cloud-lzma
*
* Author     : Wang FuYu <wangfuyu1989@foxmail.com>
* Create Date: 2015-03-30
* 
* History    : 
*------------------------------------------------------------
*
*01, 30Mar15, Wang FuYu, create file.
************************************************************/

#ifndef _CLOUD_LZMA_H_
#define _CLOUD_LZMA_H_

#define LZMA_PROPS_SIZE 5
#define SLP_LZMA_LEVEL 3
#define SLP_LZMA_
#define LZMA_HEADER_SIZE	(LZMA_PROPS_SIZE + 8)
#define SLP_LZMA_DICT_SIZE (1 << 20)
#define SLP_LZMA_BLOCK_SIZE_DEBUG (1 << 20)

typedef enum
{
	CLOUD_LZMA_COMPRESS,
	CLOUD_LZMA_UNCOMPRESS,
	CLOUD_LZMA_VALID,
} SERVICE_TYPE;

/* process data with lzma alogorithm */
int lzmaDataProg(char *src, char *dst, unsigned short type);

#endif
