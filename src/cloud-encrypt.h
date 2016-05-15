/***********************************************************
*
* Copyright (C), 2010-2014, TP-LINK TECHNOLOGIES CO., LTD.
* All rights reserved.
*
* File name  : cloud-encrypt.h 
* Version	 : 1.0 
* Description: header file of encrypt-handler
*
* Author	 : Wang FuYu <wangfuyu@tp-link.net>
* Create Date: 2015-04-01
* 
* History	 : 
*------------------------------------------------------------
*
*01, 01Apr15, Wang FuYu, create file.
************************************************************/
#ifndef _CLOUD_ENCRYPT_H_
#define _CLOUD_ENCRYPT_H_

#include <des_min_inc.h>

#define ENCRYPT_OK	0	
#define ENCRYPT_ERR	-1

#define ENCRYPT_KEY_LEN 8
#define ENCYRPT_KEY_DEVID	"ABCDEFGH"

typedef enum
{
	CLOUD_DES_ENCRYPT,
	CLOUD_DES_DECRYPT,
	CLOUD_DES_INVALID,
} SERVICE_TYPE;

/* encrypt file */
unsigned char *encryptFile(unsigned char *srcEncryptBuf, int srcEncryptBufLen, int *desMd5Len);

/* decrypt file */
unsigned char *decryptFile(unsigned char *srcEncryptBuf, int srcEncryptBufLen, int *desMd5Len);

unsigned char *desEncryptBuf(unsigned char *srcBuf, int srcLen, int *destLen, int type);

#endif

