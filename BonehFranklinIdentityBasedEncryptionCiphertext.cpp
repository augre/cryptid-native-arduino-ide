#include <stdlib.h>
#include <string.h>

#include "BonehFranklinIdentityBasedEncryptionCiphertext.h"


void bonehFranklinIdentityBasedEncryptionCiphertext_init(BonehFranklinIdentityBasedEncryptionCiphertext *ciphertextOutput, const AffinePoint cipherU, const unsigned char *const cipherV, const int cipherVLength,
                                     const unsigned char *const cipherW, const int cipherWLength)
{
    affine_init(&ciphertextOutput->cipherU, cipherU.x, cipherU.y);
    
    ciphertextOutput->cipherV = (unsigned char*)malloc(cipherVLength * sizeof(unsigned char) + 1);
    memcpy(ciphertextOutput->cipherV, cipherV, cipherVLength + 1);
    
    ciphertextOutput->cipherVLength = cipherVLength;
    
    ciphertextOutput->cipherW = (unsigned char*)malloc(cipherWLength * sizeof(unsigned char) + 1);
    memcpy(ciphertextOutput->cipherW, cipherW, cipherWLength + 1);
    
    ciphertextOutput->cipherWLength = cipherWLength;
}

void bonehFranklinIdentityBasedEncryptionCiphertext_destroy(BonehFranklinIdentityBasedEncryptionCiphertext ciphertext)
{
    affine_destroy(ciphertext.cipherU);
    free(ciphertext.cipherV);
    free(ciphertext.cipherW);
}

CryptidValidationResult bonehFranklinIdentityBasedEncryptionCiphertext_isValid(const BonehFranklinIdentityBasedEncryptionCiphertext bonehFranklinIdentityBasedEncryptionCiphertext, const EllipticCurve ellipticCurve)
{
    if(affine_isValid(bonehFranklinIdentityBasedEncryptionCiphertext.cipherU, ellipticCurve)
        && bonehFranklinIdentityBasedEncryptionCiphertext.cipherV
        && bonehFranklinIdentityBasedEncryptionCiphertext.cipherVLength != 0
        && bonehFranklinIdentityBasedEncryptionCiphertext.cipherW
        && bonehFranklinIdentityBasedEncryptionCiphertext.cipherWLength != 0)
    {
        return CRYPTID_VALIDATION_SUCCESS;
    }

    return CRYPTID_VALIDATION_FAILURE;
}
