//Decodes Base64
//
// https://gist.github.com/barrysteyn/7308212
//

#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <assert.h>
#include <openssl/buffer.h>

//Calculates the length of a decoded string
static size_t calcDecodeLength(const char* b64input) {
    size_t len = strlen(b64input),
            padding = 0;

    if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
        padding = 2;
    else if (b64input[len-1] == '=') //last char is =
        padding = 1;

    return (len*3)/4 - padding;
}

//Decodes a base64 encoded string
int Base64Decode(char* b64message, char** buffer, size_t* length) {
    BIO *bio, *b64;

    int decodeLen = calcDecodeLength(b64message);
    *buffer = (char*)malloc(decodeLen + 1);
    (*buffer)[decodeLen] = '\0';

    bio = BIO_new_mem_buf(b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    *length = BIO_read(bio, *buffer, strlen(b64message));
    assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
    BIO_free_all(bio);

    return (0); //success
}

//Encodes a binary safe base 64 string
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *b64text=(*bufferPtr).data;

    return (0); //success
}
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

/* Base64 encoder/decoder. Originally Apache file ap_base64.c
 */

//#include <string.h>
//
//#include "base64.h"
//
///* aaaack but it's fast and const should make it shared text page. */
//static const unsigned char pr2six[256] =
//        {
//                /* ASCII table */
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
//                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
//                64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
//                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
//                64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
//                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
//        };
//
//int Base64decode_len(const char *bufcoded)
//{
//    int nbytesdecoded;
//    register const unsigned char *bufin;
//    register int nprbytes;
//
//    bufin = (const unsigned char *) bufcoded;
//    while (pr2six[*(bufin++)] <= 63);
//
//    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
//    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
//
//    return nbytesdecoded + 1;
//}
//
//int Base64decode(char *bufplain, const char *bufcoded)
//{
//    int nbytesdecoded;
//    register const unsigned char *bufin;
//    register unsigned char *bufout;
//    register int nprbytes;
//
//    bufin = (const unsigned char *) bufcoded;
//    while (pr2six[*(bufin++)] <= 63);
//    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
//    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
//
//    bufout = (unsigned char *) bufplain;
//    bufin = (const unsigned char *) bufcoded;
//
//    while (nprbytes > 4) {
//        *(bufout++) =
//                (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
//        *(bufout++) =
//                (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
//        *(bufout++) =
//                (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
//        bufin += 4;
//        nprbytes -= 4;
//    }
//
//    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
//    if (nprbytes > 1) {
//        *(bufout++) =
//                (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
//    }
//    if (nprbytes > 2) {
//        *(bufout++) =
//                (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
//    }
//    if (nprbytes > 3) {
//        *(bufout++) =
//                (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
//    }
//
//    *(bufout++) = '\0';
//    nbytesdecoded -= (4 - nprbytes) & 3;
//    return nbytesdecoded;
//}
//
//static const char basis_64[] =
//        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//
//int Base64encode_len(int len)
//{
//    return ((len + 2) / 3 * 4) + 1;
//}
//
//int Base64encode(char *encoded, const char *string, int len)
//{
//    int i;
//    char *p;
//
//    p = encoded;
//    for (i = 0; i < len - 2; i += 3) {
//        *p++ = basis_64[(string[i] >> 2) & 0x3F];
//        *p++ = basis_64[((string[i] & 0x3) << 4) |
//                        ((int) (string[i + 1] & 0xF0) >> 4)];
//        *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
//                        ((int) (string[i + 2] & 0xC0) >> 6)];
//        *p++ = basis_64[string[i + 2] & 0x3F];
//    }
//    if (i < len) {
//        *p++ = basis_64[(string[i] >> 2) & 0x3F];
//        if (i == (len - 1)) {
//            *p++ = basis_64[((string[i] & 0x3) << 4)];
//            *p++ = '=';
//        }
//        else {
//            *p++ = basis_64[((string[i] & 0x3) << 4) |
//                            ((int) (string[i + 1] & 0xF0) >> 4)];
//            *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
//        }
//        *p++ = '=';
//    }
//
//    *p++ = '\0';
//    return p - encoded;
//}

///*
// *  RFC 1521 base64 encoding/decoding
// *
// *  Copyright (C) 2006-2014, ARM Limited, All Rights Reserved
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
//
///*#if !defined(POLARSSL_CONFIG_FILE)
//#include "polarssl/config.h"
//#else
//#include POLARSSL_CONFIG_FILE
//#endif
//
//#if defined(POLARSSL_BASE64_C)
//
//#include "polarssl/base64.h"
//
//#if defined(_MSC_VER) && !defined(EFIX64) && !defined(EFI32)
//#include <basetsd.h>
//typedef UINT32 uint32_t;
//#else
//#include <inttypes.h>
//#endif
//
//#if defined(POLARSSL_SELF_TEST)
//#include <string.h>
//#if defined(POLARSSL_PLATFORM_C)
//#include "polarssl/platform.h"
//#else
//#include <stdio.h>
//#define polarssl_printf printf
//#endif /* POLARSSL_PLATFORM_C */
////#endif /* POLARSSL_SELF_TEST */
//
//
//#include "base64.h"
//#include <unistd.h>
//#include <stdio.h>
//
//static const unsigned char base64_enc_map[64] =
//        {
//                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
//                'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
//                'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
//                'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
//                'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
//                'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
//                '8', '9', '+', '/'
//        };
//
//static const unsigned char base64_dec_map[128] =
//        {
//                127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
//                127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
//                127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
//                127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
//                127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
//                54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
//                127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
//                5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
//                15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
//                25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
//                29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
//                39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
//                49,  50,  51, 127, 127, 127, 127, 127
//        };
//
///*
// * Encode a buffer into base64 format
// */
//int base64_encode( unsigned char *dst, size_t *dlen,
//                   const unsigned char *src, size_t slen )
//{
//    size_t i, n;
//    int C1, C2, C3;
//    unsigned char *p;
//
//    if( slen == 0 )
//    {
//        *dlen = 0;
//        return( 0 );
//    }
//    n = ( slen << 3 ) / 6;
//
//    switch( ( slen << 3 ) - ( n * 6 ) )
//    {
//        case  2: n += 3; break;
//        case  4: n += 2; break;
//        default: break;
//    }
//
//    if( *dlen < n + 1 )
//    {
//        *dlen = n + 1;
//        return( POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL );
//    }
//
//    n = ( slen / 3 ) * 3;
//
//    for( i = 0, p = dst; i < n; i += 3 )
//    {
//        C1 = *src++;
//        C2 = *src++;
//        C3 = *src++;
//
//        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
//        *p++ = base64_enc_map[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
//        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
//        *p++ = base64_enc_map[C3 & 0x3F];
//    }
//
//    if( i < slen )
//    {
//        C1 = *src++;
//        C2 = ( ( i + 1 ) < slen ) ? *src++ : 0;
//
//        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
//        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
//
//        if( ( i + 1 ) < slen )
//            *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
//        else *p++ = '=';
//
//        *p++ = '=';
//    }
//
//    *dlen = p - dst;
//    *p = 0;
//
//    return( 0 );
//}
//
///*
// * Decode a base64-formatted buffer
// */
//int base64_decode( unsigned char *dst, size_t *dlen,
//                   const unsigned char *src, size_t slen )
//{
//    printf("\t\tbase64_decode()... src: %s\n", src);
//    size_t i, n;
//    uint32_t j, x;
//    unsigned char *p;
//
//    /* First pass: check for validity and get output length */
//    printf("\t\tfirst pass\n");
//    for( i = n = j = 0; i < slen; i++ )
//    {
//        printf("\t\ti: %d\n", i);
//        /* Skip spaces before checking for EOL */
//        x = 0;
//        while( i < slen && src[i] == ' ' )
//        {
//            ++i;
//            ++x;
//        }
//
//        /* Spaces at end of buffer are OK */
//        if( i == slen )
//            break;
//
//        if( ( slen - i ) >= 2 &&
//            src[i] == '\r' && src[i + 1] == '\n' )
//            continue;
//
//        if( src[i] == '\n' )
//            continue;
//
//        /* Space inside a line is an error */
//        if( x != 0 ) {
//            printf("\t\tx != 0\n");
//            return (POLARSSL_ERR_BASE64_INVALID_CHARACTER);
//        }
//
//        if( src[i] == '=' && ++j > 2 ) {
//            printf("\t\tsrc[i] == '=' && ++j > 2\n");
//            return (POLARSSL_ERR_BASE64_INVALID_CHARACTER);
//        }
//
//        if( src[i] > 127 || base64_dec_map[src[i]] > 127 ) {
//            printf("\t\t%d - %d\n", src[i], base64_dec_map[src[i]]);
//            printf("\t\tsrc[i] > 127 || base64_dec_map[src[i]] == 127\n");
//            return (POLARSSL_ERR_BASE64_INVALID_CHARACTER);
//        }
//
//        if( base64_dec_map[src[i]] < 64 && j != 0 ) {
//            printf("\t\tbase64_dec_map[src[i]] < 64 && j != 0\n");
//            return (POLARSSL_ERR_BASE64_INVALID_CHARACTER);
//        }
//
//        n++;
//    }
//
//    printf("\t\t.... n: %d\n", n);
//    if( n == 0 )
//        return( 0 );
//
//    n = ( ( n * 6 ) + 7 ) >> 3;
//    n -= j;
//
//    printf("\t\tbefore dst == NULL || *dlen < n\n");
//    if( dst == NULL || *dlen < n )
//    {
//        printf("\t\tOK. dentro\n");
//        *dlen = n;
//        return( POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL );
//    }
//
//    for( j = 3, n = x = 0, p = dst; i > 0; i--, src++ )
//    {
//        if( *src == '\r' || *src == '\n' || *src == ' ' )
//            continue;
//
//        j -= ( base64_dec_map[*src] == 64 );
//        x  = ( x << 6 ) | ( base64_dec_map[*src] & 0x3F );
//
//        if( ++n == 4 )
//        {
//            n = 0;
//            if( j > 0 ) *p++ = (unsigned char)( x >> 16 );
//            if( j > 1 ) *p++ = (unsigned char)( x >>  8 );
//            if( j > 2 ) *p++ = (unsigned char)( x       );
//        }
//    }
//
//    printf("\t\tdst: %s\n", dst);
//    *dlen = p - dst;
//
//    return( 0 );
//}
//
//#if defined(POLARSSL_SELF_TEST)
//
//static const unsigned char base64_test_dec[64] =
//{
//    0x24, 0x48, 0x6E, 0x56, 0x87, 0x62, 0x5A, 0xBD,
//    0xBF, 0x17, 0xD9, 0xA2, 0xC4, 0x17, 0x1A, 0x01,
//    0x94, 0xED, 0x8F, 0x1E, 0x11, 0xB3, 0xD7, 0x09,
//    0x0C, 0xB6, 0xE9, 0x10, 0x6F, 0x22, 0xEE, 0x13,
//    0xCA, 0xB3, 0x07, 0x05, 0x76, 0xC9, 0xFA, 0x31,
//    0x6C, 0x08, 0x34, 0xFF, 0x8D, 0xC2, 0x6C, 0x38,
//    0x00, 0x43, 0xE9, 0x54, 0x97, 0xAF, 0x50, 0x4B,
//    0xD1, 0x41, 0xBA, 0x95, 0x31, 0x5A, 0x0B, 0x97
//};
//
//static const unsigned char base64_test_enc[] =
//    "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
//    "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw==";
//
///*
// * Checkup routine
// */
//int base64_self_test( int verbose )
//{
//    size_t len;
//    const unsigned char *src;
//    unsigned char buffer[128];
//
//    if( verbose != 0 )
//        polarssl_printf( "  Base64 encoding test: " );
//
//    len = sizeof( buffer );
//    src = base64_test_dec;
//
//    if( base64_encode( buffer, &len, src, 64 ) != 0 ||
//         memcmp( base64_test_enc, buffer, 88 ) != 0 )
//    {
//        if( verbose != 0 )
//            polarssl_printf( "failed\n" );
//
//        return( 1 );
//    }
//
//    if( verbose != 0 )
//        polarssl_printf( "passed\n  Base64 decoding test: " );
//
//    len = sizeof( buffer );
//    src = base64_test_enc;
//
//    if( base64_decode( buffer, &len, src, 88 ) != 0 ||
//         memcmp( base64_test_dec, buffer, 64 ) != 0 )
//    {
//        if( verbose != 0 )
//            polarssl_printf( "failed\n" );
//
//        return( 1 );
//    }
//
//    if( verbose != 0 )
//        polarssl_printf( "passed\n\n" );
//
//    return( 0 );
//}
//
//#endif /* POLARSSL_SELF_TEST */
//
////#endif /* POLARSSL_BASE64_C */