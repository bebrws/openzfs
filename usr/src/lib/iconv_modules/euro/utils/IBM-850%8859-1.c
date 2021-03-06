/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 1991-2005 Unicode, Inc. All rights reserved. Distributed
 * under the Terms of Use in http://www.unicode.org/copyright.html.
 *
 * This file has been modified by Sun Microsystems, Inc.
 *
 */
/*
 * Copyright (c) 1994, 1995 by Sun Microsystems, Inc.
 * Copyright (c) 1994, Nihon Sun Microsystems K.K.
 * All Rights Reserved.
 */

/*
  Converts EBCDIC IBM-850 to ASCII ISO 8859-1.
  By Greg Nakhimovsky, Sun Microsystems.
  April, 1996.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define MAGIC_NUMBER	(0x216513)
#define ERR_RETURN      (-1)            /* result code on error */

#define GET(c)          ((c) = *ip, ip++, ileft--)
#define PUT(c)          (*op = (c), op++, oleft--)
#define UNGET()         (ip--, ileft++)


/*
 * Open; called from iconv_open()
 */
void *
_icv_open()
{
	return ((void*)MAGIC_NUMBER);
}


/*
 * Close; called from iconv_close
 */
void
_icv_close(int* cd)
{
	if (!cd || cd != (int*)MAGIC_NUMBER)
		errno = EBADF;
}


/*
 * Actual conversion; called from iconv()
 */
size_t
_icv_iconv(int* cd, char **inbuf, size_t *inbytesleft,
				char **outbuf, size_t *outbytesleft)
{
	unsigned char	*ip, ic, *op;
	size_t			ileft, oleft;
	size_t			retval = 0;

	static unsigned char map[256] = {
0x00 , 0x01 , 0x02 , 0x03 , 0x04 , 0x05 , 0x06 , 0x07 , 0x08 , 0x09 ,
0x0A , 0x0B , 0x0C , 0x0D , 0x0E , 0x0F , 0x10 , 0x11 , 0x12 , 0x13 ,
0x14 , 0x15 , 0x16 , 0x17 , 0x18 , 0x19 , 0x1C , 0x1B , 0x7F , 0x1D ,
0x1E , 0x1F , 0x20 , 0x21 , 0x22 , 0x23 , 0x24 , 0x25 , 0x26 , 0x27 ,
0x28 , 0x29 , 0x2A , 0x2B , 0x2C , 0x2D , 0x2E , 0x2F , 0x30 , 0x31 ,
0x32 , 0x33 , 0x34 , 0x35 , 0x36 , 0x37 , 0x38 , 0x39 , 0x3A , 0x3B ,
0x3C , 0x3D , 0x3E , 0x3F , 0x40 , 0x41 , 0x42 , 0x43 , 0x44 , 0x45 ,
0x46 , 0x47 , 0x48 , 0x49 , 0x4A , 0x4B , 0x4C , 0x4D , 0x4E , 0x4F ,
0x50 , 0x51 , 0x52 , 0x53 , 0x54 , 0x55 , 0x56 , 0x57 , 0x58 , 0x59 ,
0x5A , 0x5B , 0x5C , 0x5D , 0x5E , 0x5F , 0x60 , 0x61 , 0x62 , 0x63 ,
0x64 , 0x65 , 0x66 , 0x67 , 0x68 , 0x69 , 0x6A , 0x6B , 0x6C , 0x6D ,
0x6E , 0x6F , 0x70 , 0x71 , 0x72 , 0x73 , 0x74 , 0x75 , 0x76 , 0x77 ,
0x78 , 0x79 , 0x7A , 0x7B , 0x7C , 0x7D , 0x7E , 0x1A , 0xC7 , 0xFC ,
0xE9 , 0xE2 , 0xE4 , 0xE0 , 0xE5 , 0xE7 , 0xEA , 0xEB , 0xE8 , 0xEF ,
0xEE , 0xEC , 0xC4 , 0xC5 , 0xC9 , 0xE6 , 0xC6 , 0xF4 , 0xF6 , 0xF2 ,
0xFB , 0xF9 , 0xFF , 0xD6 , 0xDC , 0xF8 , 0xA3 , 0xD8 , 0xD7 , 0x1A ,
0xE1 , 0xED , 0xF3 , 0xFA , 0xF1 , 0xD1 , 0xAA , 0xBA , 0xBF , 0xAE ,
0xAC , 0xBD , 0xBC , 0xA1 , 0xAB , 0xBB , 0x1A , 0x1A , 0x1A , 0x1A ,
0x1A , 0xC1 , 0xC2 , 0xC0 , 0xA9 , 0x1A , 0x1A , 0x1A , 0x1A , 0xA2 ,
0xA5 , 0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0xE3 , 0xC3 ,
0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0x1A , 0xA4 , 0xF0 , 0xD0 ,
0xCA , 0xCB , 0xC8 , 0x1A , 0xCD , 0xCE , 0xCF , 0x1A , 0x1A , 0x1A ,
0x1A , 0xA6 , 0xCC , 0x1A , 0xD3 , 0xDF , 0xD4 , 0xD2 , 0xF5 , 0xD5 ,
0xB5 , 0xFE , 0xDE , 0xDA , 0xDB , 0xD9 , 0xFD , 0xDD , 0xAF , 0xB4 ,
0xAD , 0xB1 , 0x1A , 0xBE , 0xB6 , 0xA7 , 0xF7 , 0xB8 , 0xB0 , 0xA8 ,
0xB7 , 0xB9 , 0xB3 , 0xB2 , 0x1A , 0xA0 };

	if (!cd || cd != (int*)MAGIC_NUMBER)
	{
		errno = EBADF;
		return((size_t)ERR_RETURN);
	}

	if ((inbuf == 0) || (*inbuf == 0))
		return((size_t)0);

	ip = (unsigned char*)*inbuf;
	op = (unsigned char *)*outbuf;
	ileft = *inbytesleft;
	oleft = *outbytesleft;

	/*
	 * Main loop; basically 1 loop per 1 input byte
	 */

	while (ileft > 0) {
		GET(ic);
		if (oleft < 1) {
			UNGET();
			errno = E2BIG;
			retval = ERR_RETURN;
			goto ret;
		}
		PUT(map[ic]);
		retval++;
	}

ret:
	*inbuf = (char *)ip;
	*inbytesleft = ileft;
	*outbuf = (char *)op;
	*outbytesleft = oleft;

	return (retval);
}
