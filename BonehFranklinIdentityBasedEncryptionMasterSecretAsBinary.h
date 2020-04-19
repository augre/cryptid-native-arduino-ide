#ifndef __CRYPTID_BONEH_FRANKLIN_IDENTITY_BASED_ENCRYPTION_MASTER_SECRET_AS_BINARY_H
#define __CRYPTID_BONEH_FRANKLIN_IDENTITY_BASED_ENCRYPTION_MASTER_SECRET_AS_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary
{
    void *masterSecret;

    size_t masterSecretLength;
} BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary;
#ifdef __cplusplus
}
#endif

#endif
