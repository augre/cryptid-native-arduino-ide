#ifndef __CRYPTID_RAND_BYTES_H
#define __CRYPTID_RAND_BYTES_H

#include "Status.h"
#include <Arduino.h>



#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fills the passed buffer from a cryptographically secure source.
 * 
 * If {@code (__CRYPTID_EXTERN_RANDOM} is defined, then this function will
 * call the {@code int __cryptid_cryptoRandom(void *buf, const int num)} function
 * internally.
 * @param buf the buffer to fill
 * @param num the size of the buffer
 * @return CRYPTID_SUCCESS if everything went right
 */
CryptidStatus cryptid_randomBytes(unsigned char *buf, const int num);

CryptidStatus __cryptid_cryptoRandom(unsigned char *buf, const int num);


#ifdef __cplusplus
}
#endif

#endif
