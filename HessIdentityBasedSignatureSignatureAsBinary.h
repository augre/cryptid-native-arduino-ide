#ifndef __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_SIGNATURE_AS_BINARY_H
#define __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_SIGNATURE_AS_BINARY_H

#include "AffinePointAsBinary.h"
#include "HessIdentityBasedSignatureSignature.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HessIdentityBasedSignatureSignatureAsBinary
{
    AffinePointAsBinary u;

    void *v;

    size_t vLength;
} HessIdentityBasedSignatureSignatureAsBinary;

void hessIdentityBasedSignatureSignatureAsBinary_init(HessIdentityBasedSignatureSignatureAsBinary *signatureAsBinaryOutput, const AffinePointAsBinary u, const void *const v, const size_t vLength);

void hessIdentityBasedSignatureSignatureAsBinary_destroy(HessIdentityBasedSignatureSignatureAsBinary signatureAsBinary);

void hessIdentityBasedSignatureSignatureAsBinary_toHessIdentityBasedSignatureSignature(HessIdentityBasedSignatureSignature *signatureOutput, const HessIdentityBasedSignatureSignatureAsBinary signatureAsBinary);

void hessIdentityBasedSignatureSignatureAsBinary_fromHessIdentityBasedSignatureSignature(HessIdentityBasedSignatureSignatureAsBinary *signatureAsBinaryOutput, const HessIdentityBasedSignatureSignature signature);

#ifdef __cplusplus
}
#endif
#endif
