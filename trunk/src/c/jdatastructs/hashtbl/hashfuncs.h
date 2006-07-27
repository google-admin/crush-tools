/** @file hashfuncs.h
  * @brief contains various hash functions for use in the hashtable 
  * implementation.
  *
  * Note that jeremy hinds did not write any of these functions.  they are
  * used in accordance with the copyrights (if any) of their respective
  * sources.
  */


#ifndef HASHFUNCS_H
#define HASHFUNCS_H

/** this algorithm (k=33) was first reported by dan bernstein many years ago
  * in comp.lang.c. another version of this algorithm (now favored by bernstein)
  * uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33 (why
  * it works better than many other constants, prime or not) has never been
  * adequately explained.
  *
  * from http://www.cs.yorku.ca/~oz/hash.html with no license restrictions
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int djb2(unsigned char* str) ;

/** this algorithm was created for sdbm (a public-domain reimplementation of
  * ndbm) database library. it was found to do well in scrambling bits, causing
  * better distribution of the keys and fewer splits. it also happens to be a
  * good general hashing function with good distribution. the actual function is
  * hash(i) = hash(i - 1) * 65599 + str[i]; what is included below is the faster
  * version used in gawk. [there is even a faster, duff-device version] the
  * magic constant 65599 was picked out of thin air while experimenting with
  * different constants, and turns out to be a prime. this is one of the
  * algorithms used in berkeley db (see sleepycat) and elsewhere.
  *
  * from http://www.cs.yorku.ca/~oz/hash.html with no license restrictions
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int sdbm(unsigned char* str) ;

/** A simple hash function from Robert Sedgwicks Algorithms in C book. I've
  * added some simple optimizations to the algorithm in order to speed up its
  * hashing process.
  *
  * Author: Arash Partow - 2002
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int RSHash(unsigned char* str) ;

/** A bitwise hash function written by Justin Sobel.
  *
  * Author: Arash Partow - 2002
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int JSHash(unsigned char* str) ;

/** This hash algorithm is based on work by Peter J. Weinberger of AT&T Bell
  * Labs.
  *
  * Author: Arash Partow - 2002
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int PJWHash(unsigned char* str) ;

/** Similar to the PJW Hash function, but tweaked for 32-bit processors. Its
  * the hash function widely used on most UNIX systems.
  *
  * Author: Arash Partow - 2002
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int ELFHash(unsigned char* str) ;

/** This hash function comes from Brian Kernighan and Dennis Ritchie's book
  * "The C Programming Language". It is a simple hash function using a strange
  * set of possible seeds which all constitute a pattern of 31....31...31 etc,
  * it seems to be very similar to the DJB hash function.

  * NOTE - JH:	this appears to be 2nd-edition version, not to be mistaken for
		the first ed. version described as follows.

http://www.cs.yorku.ca/~oz/hash.html

lose lose
This hash function appeared in K&R (1st ed) but at least the reader was warned:
"This is not the best possible algorithm, but it has the merit of extreme
simplicity." This is an understatement; It is a terrible hashing algorithm,
and it could have been much better without sacrificing its "extreme simplicity."
[see the second edition!] Many C programmers use this function without actually
testing it, or checking something like Knuth's Sorting and Searching, so it
stuck. It is now found mixed with otherwise respectable code, eg. cnews. sigh.
[see also: tpop] 

	unsigned long hash(unsigned char *str) {
		unsigned int hash = 0;
		int c;

		while (c = *str++)
			 hash += c;

		return hash;
	}

  *
  * Author: Arash Partow - 2002
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int BKDRHash(unsigned char* str) ;

/** This is the algorithm of choice which is used in the open source SDBM
  * project. The hash function seems to have a good over-all distribution for
  * many different data sets. It seems to work well in situations where there
  * is a high variance in the MSBs of the elements in a data set.
  *
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * Author: Arash Partow - 2002
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int SDBMHash(unsigned char* str) ;

/** An algorithm produced by me [sic] Arash Partow. I took ideas from all of the
  * above hash functions making a hybrid rotative and additive hash function
  * algorithm based around four primes 3,5,7 and 11. There isn't any real
  * mathematical analysis explaining why one should use this hash function
  * instead of the others described above other than the fact that I tired
  * to resemble the design as close as possible to a simple LFSR. An empirical
  * result which demonstrated the distributive abilities of the hash algorithm
  * was obtained using a hash-table with 100003 buckets, hashing The Project
  * Gutenberg Etext of Webster's Unabridged Dictionary, the longest encountered
  * chain length was 7, the average chain length was 2, the number of empty
  * buckets was 4579.
  *
  * from http://www.partow.net/programming/hashfunctions/
  * under common public license : http://www.opensource.org/licenses/cpl.php
  *
  * Author: Arash Partow - 2002
  * @param str string to be hashed
  * @return hashed value of the string
  */
unsigned int APHash(unsigned char* str) ;

#endif /* HASHFUNCS_H */
