#include "HessIdentityBasedSignatureSignature.h"


void hessIdentityBasedSignatureSignature_init(HessIdentityBasedSignatureSignature *signatureOutput, const AffinePoint u, const mpz_t v)
{
    affine_init(&signatureOutput->u, u.x, u.y);
    
    mpz_init_set(signatureOutput->v, v);
}

void hessIdentityBasedSignatureSignature_destroy(HessIdentityBasedSignatureSignature signature)
{
    affine_destroy(signature.u);
    mpz_clear(signature.v);
}

CryptidValidationResult hessIdentityBasedSignatureSignature_isValid(const HessIdentityBasedSignatureSignature signature, const EllipticCurve ellipticCurve)
{
    if(affine_isValid(signature.u, ellipticCurve)
        && signature.v != NULL)
    {
        return CRYPTID_VALIDATION_SUCCESS;
    }
    
    return CRYPTID_VALIDATION_FAILURE;
}