#ifndef _BASE64_H_
#define _BASE64_H_


int Base64Encode(const unsigned char* buffer, size_t length, char** b64text);
int Base64Decode(char* b64message, /*unsigned*/ char** buffer, size_t* length);
#endif //_BASE64_H_

/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */
/* ====================================================================
 * Copyright (c) 1995-1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */


//
//#ifndef _BASE64_H_
//#define _BASE64_H_
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//int Base64encode_len(int len);
//int Base64encode(char * coded_dst, const char *plain_src,int len_plain_src);
//
//int Base64decode_len(const char * coded_src);
//int Base64decode(char * plain_dst, const char *coded_src);
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif //_BASE64_H_


//////////////
///**
// * \file base64.h
// *
// * \brief RFC 1521 base64 encoding/decoding
// *
// *  Copyright (C) 2006-2013, ARM Limited, All Rights Reserved
// *
// *  This file is part of mbed TLS (https://tls.mbed.org)
// *
// *  This program is free software; you can redistribute it and/or modify
// *  it under the terms of the GNU General Public License as published by
// *  the Free Software Foundation; either version 2 of the License, or
// *  (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// *  GNU General Public License for more details.
// *
// *  You should have received a copy of the GNU General Public License along
// *  with this program; if not, write to the Free Software Foundation, Inc.,
// *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// */
//#ifndef POLARSSL_BASE64_H
//#define POLARSSL_BASE64_H
//
//#include <stddef.h>
//#include <unistd.h>
//#include <stdint.h>
//
//#define POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL               -0x002A  /**< Output buffer too small. */
//#define POLARSSL_ERR_BASE64_INVALID_CHARACTER              -0x002C  /**< Invalid character in input. */
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
///**
// * \brief          Encode a buffer into base64 format
// *
// * \param dst      destination buffer
// * \param dlen     size of the buffer
// * \param src      source buffer
// * \param slen     amount of data to be encoded
// *
// * \return         0 if successful, or POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL.
// *                 *dlen is always updated to reflect the amount
// *                 of data that has (or would have) been written.
// *
// * \note           Call this function with *dlen = 0 to obtain the
// *                 required buffer size in *dlen
// */
//int base64_encode( unsigned char *dst, size_t *dlen,
//                   const unsigned char *src, size_t slen );
//
///**
// * \brief          Decode a base64-formatted buffer
// *
// * \param dst      destination buffer (can be NULL for checking size)
// * \param dlen     size of the buffer
// * \param src      source buffer
// * \param slen     amount of data to be decoded
// *
// * \return         0 if successful, POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL, or
// *                 POLARSSL_ERR_BASE64_INVALID_CHARACTER if the input data is
// *                 not correct. *dlen is always updated to reflect the amount
// *                 of data that has (or would have) been written.
// *
// * \note           Call this function with *dst = NULL or *dlen = 0 to obtain
// *                 the required buffer size in *dlen
// */
//int base64_decode( unsigned char *dst, size_t *dlen,
//                   const unsigned char *src, size_t slen );
//
///**
// * \brief          Checkup routine
// *
// * \return         0 if successful, or 1 if the test failed
// */
//int base64_self_test( int verbose );
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* base64.h */