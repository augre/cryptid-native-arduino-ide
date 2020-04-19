/**************************** sha.h ****************************/
/***************** See RFC 6234 for details. *******************/
/*
   Copyright (c) 2011 IETF Trust and the persons identified as
   authors of the code.  All rights reserved.

   Redistribution and use in source and binary forms, with or
   without modification, are permitted provided that the following
   conditions are met:

   - Redistributions of source code must retain the above
     copyright notice, this list of conditions and
     the following disclaimer.
     - Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

   - Neither the name of Internet Society, IETF or IETF Trust, nor
     the names of specific contributors, may be used to endorse or
     promote products derived from this software without specific
     prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef SHA_H
#define SHA_H

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 *  All SHA functions return one of these values.
 */
typedef enum SHAResult{
    shaSuccess = 0,
    shaNull,            /* Null pointer parameter */
    shaInputTooLong,    /* input data too long */
    shaStateError,      /* called Input after FinalBits or Result */
    shaBadParam         /* passed a bad parameter */
} SHAResult;

/*
 *  These constants hold size information for each of the SHA
 *  hashing operations
 */
enum {
    SHA1_Message_Block_Size = 64, SHA224_Message_Block_Size = 64,
    SHA256_Message_Block_Size = 64, SHA384_Message_Block_Size = 128,
    SHA512_Message_Block_Size = 128,
    USHA_Max_Message_Block_Size = SHA512_Message_Block_Size,SHA1HashSize = 20, SHA224HashSize = 28, SHA256HashSize = 32,
    SHA384HashSize = 48, SHA512HashSize = 64,
    USHAMaxHashSize = SHA512HashSize,

    SHA1HashSizeBits = 160, SHA224HashSizeBits = 224,
    SHA256HashSizeBits = 256, SHA384HashSizeBits = 384,
    SHA512HashSizeBits = 512, USHAMaxHashSizeBits = SHA512HashSizeBits
};

/*
 *  These constants are used in the USHA (Unified SHA) functions.
 */
typedef enum SHAversion {
    SHA1, SHA224, SHA256, SHA384, SHA512
} SHAversion;

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation.
 */
typedef struct SHA1Context {
    uint32_t Intermediate_Hash[SHA1HashSize/4]; /* Message Digest */

    uint32_t Length_High;               /* Message length in bits */
    uint32_t Length_Low;                /* Message length in bits */

    int_least16_t Message_Block_Index;  /* Message_Block array index */
                                        /* 512-bit message blocks */
    uint8_t Message_Block[SHA1_Message_Block_Size];

    int Computed;                   /* Is the hash computed? */
    int Corrupted;                  /* Cumulative corruption code */
} SHA1Context;

/*
 *  This structure will hold context information for the SHA-256
 *  hashing operation.
 */
typedef struct SHA256Context {
    uint32_t Intermediate_Hash[SHA256HashSize/4]; /* Message Digest */

    uint32_t Length_High;               /* Message length in bits */
    uint32_t Length_Low;                /* Message length in bits */

    int_least16_t Message_Block_Index;  /* Message_Block array index */
                                        /* 512-bit message blocks */
    uint8_t Message_Block[SHA256_Message_Block_Size];
    int Computed;                   /* Is the hash computed? */
    int Corrupted;                  /* Cumulative corruption code */
} SHA256Context;

/*
 *  This structure will hold context information for the SHA-512
 *  hashing operation.
 */
typedef struct SHA512Context {
#ifdef USE_32BIT_ONLY
    uint32_t Intermediate_Hash[SHA512HashSize/4]; /* Message Digest  */
    uint32_t Length[4];                 /* Message length in bits */
#else /* !USE_32BIT_ONLY */
    uint64_t Intermediate_Hash[SHA512HashSize/8]; /* Message Digest */
    uint64_t Length_High, Length_Low;   /* Message length in bits */
#endif /* USE_32BIT_ONLY */

    int_least16_t Message_Block_Index;  /* Message_Block array index */
                                        /* 1024-bit message blocks */
    uint8_t Message_Block[SHA512_Message_Block_Size];

    int Computed;                   /* Is the hash computed?*/
    int Corrupted;                  /* Cumulative corruption code */
} SHA512Context;

/*
 *  This structure will hold context information for the SHA-224
 *  hashing operation.  It uses the SHA-256 structure for computation.
 */
typedef struct SHA256Context SHA224Context;

/*
 *  This structure will hold context information for the SHA-384
 *  hashing operation.  It uses the SHA-512 structure for computation.
 */
typedef struct SHA512Context SHA384Context;

/*
 *  This structure holds context information for all SHA
 *  hashing operations.
 */
typedef struct USHAContext {
    int whichSha;               /* which SHA is being used */
    union {
      SHA1Context sha1Context;
      SHA224Context sha224Context; SHA256Context sha256Context;
      SHA384Context sha384Context; SHA512Context sha512Context;
    } ctx;
} USHAContext;

/* SHA-1 */
int SHA1Reset(SHA1Context *);
int SHA1Input(SHA1Context *, const uint8_t *bytes,
                     unsigned int bytecount);
int SHA1FinalBits(SHA1Context *, uint8_t bits,
                         unsigned int bit_count);
int SHA1Result(SHA1Context *,
                      uint8_t Message_Digest[SHA1HashSize]);

/* SHA-224 */
int SHA224Reset(SHA224Context *);
int SHA224Input(SHA224Context *, const uint8_t *bytes,
                       unsigned int bytecount);
int SHA224FinalBits(SHA224Context *, uint8_t bits,
                           unsigned int bit_count);
int SHA224Result(SHA224Context *,
                        uint8_t Message_Digest[SHA224HashSize]);

/* SHA-256 */
int SHA256Reset(SHA256Context *);
int SHA256Input(SHA256Context *, const uint8_t *bytes,
                       unsigned int bytecount);
int SHA256FinalBits(SHA256Context *, uint8_t bits,
                           unsigned int bit_count);
int SHA256Result(SHA256Context *,
                        uint8_t Message_Digest[SHA256HashSize]);

/* SHA-384 */
int SHA384Reset(SHA384Context *);
int SHA384Input(SHA384Context *, const uint8_t *bytes,
                       unsigned int bytecount);
int SHA384FinalBits(SHA384Context *, uint8_t bits,
                           unsigned int bit_count);
int SHA384Result(SHA384Context *,
                        uint8_t Message_Digest[SHA384HashSize]);

/* SHA-512 */
int SHA512Reset(SHA512Context *);
int SHA512Input(SHA512Context *, const uint8_t *bytes,
                       unsigned int bytecount);
int SHA512FinalBits(SHA512Context *, uint8_t bits,
                           unsigned int bit_count);
int SHA512Result(SHA512Context *,
                        uint8_t Message_Digest[SHA512HashSize]);

SHAResult SHA1_OneCall(const unsigned char* message, size_t messageLength, unsigned char* hashResult);

SHAResult SHA224_OneCall(const unsigned char* message, size_t messageLength, unsigned char* hashResult);

SHAResult SHA256_OneCall(const unsigned char* message, size_t messageLength, unsigned char* hashResult);

SHAResult SHA384_OneCall(const unsigned char* message, size_t messageLength, unsigned char* hashResult);

SHAResult SHA512_OneCall(const unsigned char* message, size_t messageLength, unsigned char* hashResult);

/************************ sha-private.h ************************/
/***************** See RFC 6234 for details. *******************/
/*
 * These definitions are defined in FIPS 180-3, section 4.1.
 * Ch() and Maj() are defined identically in sections 4.1.1,
 * 4.1.2, and 4.1.3.
 *
 * The definitions used in FIPS 180-3 are as follows:
 */
#ifndef USE_MODIFIED_MACROS
#define SHA_Ch(x,y,z)        (((x) & (y)) ^ ((~(x)) & (z)))
#define SHA_Maj(x,y,z)       (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#else /* USE_MODIFIED_MACROS */
/*
 * The following definitions are equivalent and potentially faster.
 */

#define SHA_Ch(x, y, z)      (((x) & ((y) ^ (z))) ^ (z))
#define SHA_Maj(x, y, z)     (((x) & ((y) | (z))) | ((y) & (z)))

#endif /* USE_MODIFIED_MACROS */

#define SHA_Parity(x, y, z)  ((x) ^ (y) ^ (z))

#ifdef __cplusplus
}
#endif

#endif /* SHA_H */
