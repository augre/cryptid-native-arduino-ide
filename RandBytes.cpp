#include "RandBytes.h"
#include <stdlib.h>


#if defined(_WIN32)

CryptidStatus cryptid_randomBytes(unsigned char *buf, int num)
{
    // TODO Implement Windows secure random generation.
    //      Issue with good-first-issue tag?
    return CRYPTID_RANDOM_GENERATION_ERROR;
}

#elif defined(__CRYPTID_EXTERN_RANDOM)

extern int __cryptid_cryptoRandom(void *buf, const int num);

CryptidStatus cryptid_randomBytes(unsigned char *buf, const int num)
{
    if (!__cryptid_cryptoRandom(buf, num))
    {
        return CRYPTID_SUCCESS;
    }

    return CRYPTID_RANDOM_GENERATION_ERROR;
}

#else

#include <stdio.h>

CryptidStatus cryptid_randomBytes(unsigned char *buf, const int num)
{
    int i;
    randomSeed(analogRead(0));

    for (i = 0; i < num; i++)
    {
      buf[i] = random (0, 255);
      randomSeed(analogRead(0));
    }
    return CRYPTID_SUCCESS;
}

#endif
