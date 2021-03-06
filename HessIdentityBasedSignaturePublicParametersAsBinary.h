#ifndef __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_PUBLICPARAMETERS_AS_BINARY_H
#define __CRYPTID_HESS_IDENTITY_BASED_SIGNATURE_PUBLICPARAMETERS_AS_BINARY_H

#include "AffinePointAsBinary.h"
#include "EllipticCurveAsBinary.h"
#include "HashFunction.h"
#include "HessIdentityBasedSignaturePublicParameters.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct storing the Hess-IBS Public Parameters as binaries.
 */
typedef struct HessIdentityBasedSignaturePublicParametersAsBinary
{
    /**
     * The Type-1 elliptic curve we're operating over represented by string values.
     */
    EllipticCurveAsBinary ellipticCurve;

    /**
     * Subgroup order represented as a binary.
     */
    void *q;

    /**
     * The length of the Subgroup order's binary representation.
     */
    size_t qLength;

    /**
     * The string representation of a point in \f$E(F_p)\f$.
     */
    AffinePointAsBinary pointP;

    /**
     * The string representation of a point in \f$E(F_p)\f$.
     */
    AffinePointAsBinary pointPpublic;

    /**
     * The used hash function.
     */
    HashFunction hashFunction;
} HessIdentityBasedSignaturePublicParametersAsBinary;

void hessIdentityBasedSignaturePublicParametersAsBinary_init(HessIdentityBasedSignaturePublicParametersAsBinary *publicParametersAsBinaryOutput, const EllipticCurveAsBinary ellipticCurve, const void *const q, const size_t qLength, const AffinePointAsBinary pointP, const AffinePointAsBinary pointPpublic, const HashFunction hashFunction);

void hessIdentityBasedSignaturePublicParametersAsBinary_destroy(HessIdentityBasedSignaturePublicParametersAsBinary publicParametersAsBinary);

void hessIdentityBasedSignaturePublicParametersAsBinary_toHessIdentityBasedSignaturePublicParameters(HessIdentityBasedSignaturePublicParameters *publicParametersOutput, const HessIdentityBasedSignaturePublicParametersAsBinary publicParametersAsBinary);

void hessIdentityBasedSignaturePublicParametersAsBinary_fromHessIdentityBasedSignaturePublicParameters(HessIdentityBasedSignaturePublicParametersAsBinary *publicParametersAsBinaryOutput, const HessIdentityBasedSignaturePublicParameters publicParameters);

#ifdef __cplusplus
}
#endif
#endif
