#ifndef __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_SIGNATURE_H
#define __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_SIGNATURE_H

#include "gmp.h"

#include "EllipticCurve.h"
#include "AffinePoint.h"
#include "Validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HessIdentityBasedSignatureSignature
{
    AffinePoint u;
    mpz_t v;
} HessIdentityBasedSignatureSignature;

void hessIdentityBasedSignatureSignature_init(HessIdentityBasedSignatureSignature *signatureOutput, const AffinePoint u, const mpz_t v);

void hessIdentityBasedSignatureSignature_destroy(HessIdentityBasedSignatureSignature signature);

/**
 * Validates that the specified signature is correct.
 * @param signature the signature to check
 * @param ellipticCurve the elliptic curve field, we operating over
 * @return CRYPTID_VALIDATION_SUCCESS if the signature is valid
 */ 
CryptidValidationResult hessIdentityBasedSignatureSignature_isValid(const HessIdentityBasedSignatureSignature signature, const EllipticCurve ellipticCurve);

#ifdef __cplusplus
}
#endif
#endif
