/*
 **************************************************************************
 *                                                                        *
 *          General Purpose Hash Function Algorithms Library              *
 *                                                                        *
 * Author: Arash Partow - 2002                                            *
 * URL: http://www.partow.net                                             *
 * URL: http://www.partow.net/programming/hashfunctions/index.html        *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use of the General Purpose Hash Function Algorithms Library is    *
 * permitted under the guidelines and in accordance with the most current *
 * version of the Common Public License.                                  *
 * http://www.opensource.org/licenses/cpl.php                             *
 *                                                                        *
 **************************************************************************
*/

/* modified by jeremy hinds <jhinds@doubleclick.net> 2006-12-20
   to use void* input instead of char*, renamed functions to avoid
   namespace collisions.
 */


#ifndef INCLUDE_GENERALHASHFUNCTION_C_H
#define INCLUDE_GENERALHASHFUNCTION_C_H


#include <stdio.h>


typedef size_t(*hash_function_t) (void *, size_t len);


size_t ht2_RSHash(void *key, size_t len);
size_t ht2_JSHash(void *key, size_t len);
size_t ht2_PJWHash(void *key, size_t len);
size_t ht2_ELFHash(void *key, size_t len);
size_t ht2_BKDRHash(void *key, size_t len);
size_t ht2_SDBMHash(void *key, size_t len);
size_t ht2_DJBHash(void *key, size_t len);
size_t ht2_DEKHash(void *key, size_t len);
size_t ht2_APHash(void *key, size_t len);


#endif
