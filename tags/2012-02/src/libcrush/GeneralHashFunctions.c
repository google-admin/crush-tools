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

#include <crush/ht2_GeneralHashFunctions.h>

size_t ht2_RSHash(void *key, size_t len) {
  size_t b = 378551;
  size_t a = 63689;
  size_t hash = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = hash * a + (*str);
    a = a * b;
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of RS Hash Function */


size_t ht2_JSHash(void *key, size_t len) {
  size_t hash = 1315423911;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash ^= ((hash << 5) + (*str) + (hash >> 2));
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of JS Hash Function */


size_t ht2_PJWHash(void *key, size_t len) {
  size_t BitsInUnsignedInt = (size_t) (sizeof(size_t) * 8);
  size_t ThreeQuarters = (size_t) ((BitsInUnsignedInt * 3) / 4);
  size_t OneEighth = (size_t) (BitsInUnsignedInt / 8);
  size_t HighBits = (size_t) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
  size_t hash = 0;
  size_t test = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = (hash << OneEighth) + (*str);

    if ((test = hash & HighBits) != 0) {
      hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
    }
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of  P. J. Weinberger Hash Function */


size_t ht2_ELFHash(void *key, size_t len) {
  size_t hash = 0;
  size_t x = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = (hash << 4) + (*str);
    if ((x = hash & 0xF0000000L) != 0) {
      hash ^= (x >> 24);
      hash &= ~x;
    }
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of ELF Hash Function */


size_t ht2_BKDRHash(void *key, size_t len) {
  size_t seed = 131;            /* 31 131 1313 13131 131313 etc.. */
  size_t hash = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = (hash * seed) + (*str);
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of BKDR Hash Function */


size_t ht2_SDBMHash(void *key, size_t len) {
  size_t hash = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = (*str) + (hash << 6) + (hash << 16) - hash;
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of SDBM Hash Function */


size_t ht2_DJBHash(void *key, size_t len) {
  size_t hash = 5381;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = ((hash << 5) + hash) + (*str);
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of DJB Hash Function */


size_t ht2_DEKHash(void *key, size_t len) {
  size_t hash = len;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
  }
  return (hash & 0x7FFFFFFF);
}

/* End Of DEK Hash Function */


size_t ht2_APHash(void *key, size_t len) {
  size_t hash = 0;
  size_t i = 0;
  unsigned char *str = (unsigned char *) key;

  for (i = 0; i < len; str++, i++) {
    hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) ^ (hash >> 3)) :
      (~((hash << 11) ^ (*str) ^ (hash >> 5)));
  }

  return (hash & 0x7FFFFFFF);
}

/* End Of AP Hash Function */
