#include <string.h>
#include <stdlib.h>

//TODO remove if cmake is ready
#define __CRYPTID_BONEH_FRANKLIN_IDENTITY_BASED_ENCRYPTION
#include "BonehFranklinIdentityBasedEncryption.h"
#include "TatePairing.h"
#include "Random.h"
#include "RandBytes.h"
#include "Utils.h"
#include "PrimalityTest.h"
#include <Arduino.h>

// References
//  * [RFC-5091] Xavier Boyen, Luther Martin. 2007. RFC 5091. Identity-Based Cryptography Standard (IBCS) #1: Supersingular Curve Implementations of the BF and BB1 Cryptosystems


static const unsigned int SOLINAS_GENERATION_ATTEMPT_LIMIT = 100;
static const unsigned int POINT_GENERATION_ATTEMPT_LIMIT = 100;

static const unsigned int Q_LENGTH_MAPPING[] = { 160, 224, 256, 384, 512 };
static const unsigned int P_LENGTH_MAPPING[] = { 512, 1024, 1536, 3840, 7680 };

CryptidStatus cryptid_ibe_bonehFranklin_setup(BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary *masterSecretAsBinary, BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary* publicParametersAsBinary, const SecurityLevel securityLevel)
{
    // Implementation of Algorithm 5.1.2 (BFsetup1) in [RFC-5091].
    // Note, that instead of taking the bitlengts of p and q as arguments, this function takes
    // a security level which is in turn translated to bitlengths using {@code P_LENGTH_MAPPING} and
    // {@code Q_LENGTH_MAPPING}.

    /*if (!publicParametersAsBinary)
    {
        return CRYPTID_PUBLIC_PARAMETERS_NULL_ERROR;
    }*/

    // Construct the elliptic curve and its subgroup of interest
    // Select a random \f$n_q\f$-bit Solinas prime \f$q\f$.
    mpz_t q;
    mpz_init(q);

    CryptidStatus status = random_solinasPrime(q, Q_LENGTH_MAPPING[(int) securityLevel], SOLINAS_GENERATION_ATTEMPT_LIMIT);

    if (status)
    {
        mpz_clear(q);
        return status;
    }

    // Select a random integer \f$r\f$, such that \f$p = 12 \cdot r \cdot q - 1\f$ is an \f$n_p\f$-bit prime.
    unsigned int lengthOfR = P_LENGTH_MAPPING[(int) securityLevel] - Q_LENGTH_MAPPING[(int) securityLevel] - 3;
    mpz_t r, p;
    mpz_inits(r, p, NULL);
    do
    {
        random_mpzOfLength(r, lengthOfR);
        mpz_mul_ui(p, r, 12);
        mpz_mul(p, p, q);
        mpz_sub_ui(p, p, 1);
    }
    while (!primalityTest_isProbablePrime(p));

    mpz_t zero, one;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(one, 1);

    EllipticCurve ec;
    ellipticCurve_init(&ec, zero, one, p);

    mpz_clears(zero, one, NULL);

    // Select a point \f$P\f$ of order \f$q\f$ in \f$E(F_p)\f$.
    AffinePoint pointP;
    do
    {
        AffinePoint pointPprime;
        status = random_affinePoint(&pointPprime, ec, POINT_GENERATION_ATTEMPT_LIMIT);

        if (status)
        {
            mpz_clears(p, q, r, NULL);
            ellipticCurve_destroy(ec);
            return status;
        }

        mpz_t rMul;
        mpz_init_set(rMul, r);
        mpz_mul_ui(rMul, rMul, 12);

        status = affine_wNAFMultiply(&pointP, pointPprime, rMul, ec);

        if (status)
        {
            mpz_clears(p, q, r, rMul, NULL);
            ellipticCurve_destroy(ec);
            affine_destroy(pointPprime);
            return status;
        }

        mpz_clear(rMul);
        affine_destroy(pointPprime);
    }
    while (affine_isInfinity(pointP));

    // Determine the master secret.
    mpz_t qMinusTwo, s;
    mpz_init(s);
    mpz_init_set(qMinusTwo, q);
    mpz_sub_ui(qMinusTwo, qMinusTwo, 2);

    random_mpzInRange(s, qMinusTwo);
    mpz_add_ui(s, s, 2);
    mpz_clear(qMinusTwo);

    // Determine the public parameters.
    AffinePoint pointPpublic;

    status = affine_wNAFMultiply(&pointPpublic, pointP, s, ec);

    if (status)
    {
        mpz_clears(p, q, s, NULL);
        affine_destroy(pointP);
        ellipticCurve_destroy(ec);
        return status;
    }

    HashFunction hashFunction;
    hashFunction_initForSecurityLevel(&hashFunction, securityLevel);

    BonehFranklinIdentityBasedEncryptionPublicParameters publicParameters;
    bonehFranklinIdentityBasedEncryptionPublicParameters_init(&publicParameters, ec, q, pointP, pointPpublic, hashFunction);

    bonehFranklinIdentityBasedEncryptionPublicParametersAsBinary_fromBonehFranklinIdentityBasedEncryptionPublicParameters(publicParametersAsBinary, publicParameters);

    masterSecretAsBinary->masterSecret = mpz_export(NULL, &masterSecretAsBinary->masterSecretLength, 1, 1, 0, 0, s);

    mpz_clears(p, q, s, r, NULL);
    bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
    affine_destroy(pointP);
    affine_destroy(pointPpublic);
    ellipticCurve_destroy(ec);

    return CRYPTID_SUCCESS;
}

CryptidStatus cryptid_ibe_bonehFranklin_extract(AffinePointAsBinary *result, const char *const identity, const size_t identityLength, const BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary masterSecretAsBinary, const BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary publicParametersAsBinary)
{
    // Implementation of Algorithm 5.3.1 (BFextractPriv) in [RFC-5091].

    /*if (!result)
    {
        return CRYPTID_RESULT_POINTER_NULL_ERROR;
    }*/

    if (identityLength == 0)
    {
        return CRYPTID_IDENTITY_LENGTH_ERROR;
    }

    BonehFranklinIdentityBasedEncryptionPublicParameters publicParameters;
    bonehFranklinIdentityBasedEncryptionPublicParametersAsBinary_toBonehFranklinIdentityBasedEncryptionPublicParameters(&publicParameters, publicParametersAsBinary);

    if(!bonehFranklinIdentityBasedEncryptionPublicParameters_isValid(publicParameters))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        return CRYPTID_ILLEGAL_PUBLIC_PARAMETERS_ERROR;
    }

    AffinePoint qId;

    // Let \f$Q_{id} = \mathrm{HashToPoint}(E, p, q, id, \mathrm{hashfcn})\f$.
    CryptidStatus status =
        hashToPoint(&qId, identity, identityLength, publicParameters.q, publicParameters.ellipticCurve, publicParameters.hashFunction);

    if (status) 
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        return status;
    }

    mpz_t masterSecret;
    mpz_init(masterSecret);
    mpz_import(masterSecret, masterSecretAsBinary.masterSecretLength, 1, 1, 0, 0, masterSecretAsBinary.masterSecret);

    AffinePoint affineResult;

    // Let \f$S_{id} = [s]Q_{id}\f$.
    status = affine_wNAFMultiply(&affineResult, qId, masterSecret, publicParameters.ellipticCurve);

    affineAsBinary_fromAffine(result, affineResult);

    bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
    affine_destroy(qId);
    affine_destroy(affineResult);
    mpz_clear(masterSecret);

    return status;
}

CryptidStatus cryptid_ibe_bonehFranklin_encrypt(BonehFranklinIdentityBasedEncryptionCiphertextAsBinary *result, const char *const message, const size_t messageLength, const char *const identity, const size_t identityLength, const BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary publicParametersAsBinary)
{
    // Implementation of Algorithm 5.4.1 (BFencrypt) in [RFC-5091].

    if(!message)
    {
        return CRYPTID_MESSAGE_NULL_ERROR;
    }

    if(messageLength == 0)
    {
        return CRYPTID_MESSAGE_LENGTH_ERROR;
    }

    if(!identity)
    {
        return CRYPTID_IDENTITY_NULL_ERROR;
    }

    if(identityLength == 0)
    {
        return CRYPTID_IDENTITY_LENGTH_ERROR;
    }

    BonehFranklinIdentityBasedEncryptionPublicParameters publicParameters;
    bonehFranklinIdentityBasedEncryptionPublicParametersAsBinary_toBonehFranklinIdentityBasedEncryptionPublicParameters(&publicParameters, publicParametersAsBinary);

    if(!bonehFranklinIdentityBasedEncryptionPublicParameters_isValid(publicParameters))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        return CRYPTID_ILLEGAL_PUBLIC_PARAMETERS_ERROR;
    }

    mpz_t l;
    mpz_init(l);

    // Let {@code hashlen} be the length of the output of the cryptographic hash
    // function hashfcn from the public parameters.
    int hashLen;
    hashFunction_getHashSize(&hashLen, publicParameters.hashFunction);

    // \f$Q_{id} = \mathrm{HashToPoint}(E, p, q, id, \mathrm{hashfcn})\f$
    // which results in a point of order \f$q\f$ in \f$E(F_p)\f$.
    AffinePoint pointQId;
    CryptidStatus status = hashToPoint(&pointQId, identity, identityLength, publicParameters.q, publicParameters.ellipticCurve, publicParameters.hashFunction);
    if(status)
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        mpz_clear(l);
        return status;
    }

    // Select a random {@code hashlen}-bit vector {@code rho}, represented as (\f$\frac{\mathrm{hashlen}}{8}\f$)-octet
    // string in big-endian convention.
    unsigned char* rho = (unsigned char*)calloc(hashLen + 1, sizeof(unsigned char));
    cryptid_randomBytes(rho, hashLen);
    rho[hashLen] = '\0';

    // Let \f$t = \mathrm{hashfcn}(m)\f$, a {@code hashlen}-octet string resulting from applying
    // the {@code hashfcn} algorithm to the input \f$m\f$.
    unsigned char* t = (unsigned char*)calloc(hashLen, sizeof(unsigned char));
    hashFunction_hash(t, (unsigned char*) message, messageLength, publicParameters.hashFunction);

    // Let \f$l = \mathrm{HashToRange}(rho || t, q, \mathrm{hashfcn})\f$, an integer in the range
    // \f$0\f$ to \f$q - 1\f$ resulting from applying {@code HashToRange}
    // to the \f$(2 * \mathrm{hashlen})\f$-octet concatenation of {@code rho} and \f$t\f$.
    unsigned char* concat = (unsigned char*)calloc(2*hashLen + 1, sizeof(unsigned char));
    for(int i = 0; i < hashLen; i++)
    {
        concat[i] = rho[i];
    }
    for(int i = 0; i < hashLen; i++)
    {
        concat[hashLen + i] = t[i];
    }
    concat[2*hashLen] = '\0';

    hashToRange(l, concat, 2*hashLen, publicParameters.q, publicParameters.hashFunction);

    // Let \f$U = [l]P\f$, which is a point of order \f$q\f$ in \f$E(F_p)\f$.
    AffinePoint cipherPointU;
    status = affine_wNAFMultiply(&cipherPointU, publicParameters.pointP, l, publicParameters.ellipticCurve);
    if(status)
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        mpz_clear(l);
        affine_destroy(pointQId);
        free(rho);
        free(t);
        free(concat);
        return status;
    }
    
    // Let \f$\mathrm{theta} = \mathrm{Pairing}(E, p, q, P_{pub}, Q_{id})\f$, which is an element of
    // the extension field \f$F_p^2\f$ obtained using the modified Tate pairing.
    Complex theta;
    status = tate_performPairing(&theta, publicParameters.pointPpublic, pointQId, 2, publicParameters.q, publicParameters.ellipticCurve);
    if(status)
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        mpz_clear(l);
        affine_destroy(pointQId);
        free(rho);
        free(t);
        free(concat);
        affine_destroy(cipherPointU);
        return status;
    }

    //Let \f$\mathrm{theta}^{\prime} = \mathrm{theta}^l\f$, which is theta raised to the power of \f$l\f$ in \f$F_p^2\f$.
    Complex thetaPrime;
    complex_modPow(&thetaPrime, theta, l, publicParameters.ellipticCurve.fieldOrder);

    // Let \f$z = \mathrm{Canonical}(p, k, 0, \mathrm{theta}^{\prime})\f$, a canonical string
    // representation of {@code theta'}.
    int zLength;
    unsigned char* z;
    canonical(&z, &zLength, thetaPrime, publicParameters.ellipticCurve.fieldOrder, 1);

    // Let \f$w = \mathrm{hashfcn}(z)\f$ using the {@code hashfcn} hashing algorithm, the
    // result of which is a {@code hashlen}-octet string.
    unsigned char* w = (unsigned char*)calloc(hashLen, sizeof(unsigned char));
    hashFunction_hash(w, z, zLength, publicParameters.hashFunction);

    // Let \f$V = w \oplus rho\f$, which is the {@code hashlen}-octet long bit-wise XOR
    // of \f$w\f$ and {@code rho}.
    unsigned char* cipherV = (unsigned char*)calloc(hashLen + 1, sizeof(unsigned char));
    for(int i = 0; i < hashLen; i++) 
    {
        cipherV[i] = w[i] ^ rho[i];
    }
    cipherV[hashLen] = '\0';

    // Let W = \mathrm{HashBytes}(|m|, rho, \mathrm{hashfcn}) \oplus m\f$, which is the bit-wise
    // XOR of \f$m\f$ with the first \f$|m|\f$ octets of the pseudo-random bytes
    // produced by {@code HashBytes} with seed {@code rho}.
    unsigned char* cipherW = (unsigned char*)calloc(messageLength + 1, sizeof(unsigned char));
    unsigned char* hashedBytes;
    hashBytes(&hashedBytes, messageLength, rho, hashLen, publicParameters.hashFunction);
    for(size_t i = 0; i < messageLength; i++) 
    {
        cipherW[i] = hashedBytes[i] ^ message[i];
    }
    cipherW[messageLength] = '\0';

    // The ciphertext is the triple \f$(U, V, W)\f$.
    BonehFranklinIdentityBasedEncryptionCiphertext ciphertext;
    bonehFranklinIdentityBasedEncryptionCiphertext_init(&ciphertext, cipherPointU, cipherV, hashLen, cipherW, messageLength);

    bonehFranklinIdentityBasedEncryptionCiphertextAsBinary_fromBonehFranklinIdentityBasedEncryptionCiphertext(result, ciphertext);

    bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
    bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
    mpz_clear(l);
    affine_destroy(pointQId);
    affine_destroy(cipherPointU);
    complex_destroyMany(2, theta, thetaPrime);
    free(rho);
    free(concat);
    free(z);
    free(t);
    free(w);
    free(cipherV);
    free(cipherW);
    free(hashedBytes);

    return CRYPTID_SUCCESS;
}

CryptidStatus cryptid_ibe_bonehFranklin_decrypt(char **result, const BonehFranklinIdentityBasedEncryptionCiphertextAsBinary ciphertextAsBinary, const AffinePointAsBinary privateKeyAsBinary, const BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary publicParametersAsBinary)
{
    // Implementation of Algorithm 5.5.1 (BFdecrypt) in [RFC-5091].

    BonehFranklinIdentityBasedEncryptionPublicParameters publicParameters;
    bonehFranklinIdentityBasedEncryptionPublicParametersAsBinary_toBonehFranklinIdentityBasedEncryptionPublicParameters(&publicParameters, publicParametersAsBinary);

    if(!bonehFranklinIdentityBasedEncryptionPublicParameters_isValid(publicParameters))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        return CRYPTID_ILLEGAL_PUBLIC_PARAMETERS_ERROR;
    }

    AffinePoint privateKey;
    affineAsBinary_toAffine(&privateKey, privateKeyAsBinary);

    if(!affine_isValid(privateKey, publicParameters.ellipticCurve))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        affine_destroy(privateKey);
        return CRYPTID_ILLEGAL_PRIVATE_KEY_ERROR;
    }

    BonehFranklinIdentityBasedEncryptionCiphertext ciphertext;
    bonehFranklinIdentityBasedEncryptionCiphertextAsBinary_toBonehFranklinIdentityBasedEncryptionCiphertext(&ciphertext, ciphertextAsBinary);

    if(!bonehFranklinIdentityBasedEncryptionCiphertext_isValid(ciphertext, publicParameters.ellipticCurve))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        affine_destroy(privateKey);
        bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
        return CRYPTID_ILLEGAL_CIPHERTEXT_ERROR;
    }

    mpz_t l;
    mpz_init(l);

    // Let {@code hashlen} be the length of the output of the hash function
    // {@code hashfcn} measured in octets.
    int hashLen;
    hashFunction_getHashSize(&hashLen, publicParameters.hashFunction);

    // Let \f$theta = \mathrm{Pairing}(E, p ,q, U, S_{id})\f$ by applying the modified
    // Tate pairing.
    Complex theta;
    CryptidStatus status = tate_performPairing(&theta, ciphertext.cipherU, privateKey, 2, publicParameters.q, publicParameters.ellipticCurve);
    if(status)
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        affine_destroy(privateKey);
        bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
        mpz_clear(l);
        return status;
    }
    
    // Let \f$z = \mathrm{Canonical}(p, k, 0, theta)\f$ a canonical string representation of {@code theta}.
    int zLength;
    unsigned char* z;
    canonical(&z, &zLength, theta, publicParameters.ellipticCurve.fieldOrder, 1);

    // Let \f$w = \mathrm{hashfcn}(z)$ using the {@code hashfcn} hashing algorithm, the result
    // of which is a {@code hashlen}-octet string.
    unsigned char* w = (unsigned char*)calloc(hashLen, sizeof(unsigned char));
    hashFunction_hash(w, z, zLength, publicParameters.hashFunction);

    // Let \f$rho = w \oplus V\f$, the bit-wise XOR of \f$w\f$ and \f$V\f$.
    unsigned char* rho = (unsigned char*)calloc(hashLen + 1, sizeof(unsigned char));
    for(int i = 0; i < hashLen; i++) 
    {
        rho[i] = w[i] ^ ciphertext.cipherV[i];
    }
    rho[hashLen] = '\0';

    // Let \f$m = \mathrm{HashBytes}(|W|, rho, \mathrm{hashfcn}) \oplus W\f$, which is the bit-wise
    // XOR of \f$m\f$ with the first \f$|W|\f$ octets of the pseudo-random bytes
    // produced by HashBytes with seed {@code rho}.
    char* m = (char*)calloc(ciphertext.cipherWLength + 1, sizeof(char));
    unsigned char* hashedBytes;
    hashBytes(&hashedBytes, ciphertext.cipherWLength, rho, hashLen, publicParameters.hashFunction);
    for(size_t i = 0; i < ciphertext.cipherWLength; i++) 
    {
        m[i] = hashedBytes[i] ^ ciphertext.cipherW[i];
    }
    m[ciphertext.cipherWLength] = '\0';

    // Let \f$t = \mathrm{hashfcn}(m)\f$ using the \f$hashfcn\f$ algorithm.
    unsigned char* t = (unsigned char*)calloc(hashLen, sizeof(unsigned char));
    hashFunction_hash(t, (unsigned char*) m, ciphertext.cipherWLength, publicParameters.hashFunction);

    // Let \f$l = \mathrm{HashToRange}(rho || t, q, \mathrm{hashfcn}) using HashToRange
    // on the \f$(2 * \mathrm{hashlen})\f$-octet concatenation of {@code rho} and
    // \f$t\f$.
    unsigned char* concat = (unsigned char*)calloc(2*hashLen + 1, sizeof(unsigned char));
    for(int i = 0; i < hashLen; i++)
    {
        concat[i] = rho[i];
    }
    for(int i = 0; i < hashLen; i++)
    {
        concat[hashLen + i] = t[i];
    }
    concat[2*hashLen] = '\0';
    
    hashToRange(l, concat, 2*hashLen, publicParameters.q, publicParameters.hashFunction);

    complex_destroy(theta);
    free(z);
    free(t);
    free(w);
    free(rho);
    free(hashedBytes);
    free(concat);
    
    // Verify that \f$U = [l]P\f$.
    AffinePoint testPoint;
    status = affine_wNAFMultiply(&testPoint, publicParameters.pointP, l, publicParameters.ellipticCurve);
    if(status)
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        affine_destroy(privateKey);
        bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
        mpz_clear(l);
        free(m);
        return status;
    }

    // If this is the case, then the decrypted plaintext \f$m\f$ is returned.
    if(affine_isEquals(ciphertext.cipherU, testPoint))
    {
        bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
        affine_destroy(privateKey);
        bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
        affine_destroy(testPoint);
        mpz_clear(l);
        *result = m;
        return CRYPTID_SUCCESS;
    }

    // Otherwise, the ciphertext is rejected and no plaintext is returned.
    bonehFranklinIdentityBasedEncryptionPublicParameters_destroy(publicParameters);
    affine_destroy(privateKey);
    bonehFranklinIdentityBasedEncryptionCiphertext_destroy(ciphertext);
    affine_destroy(testPoint);
    mpz_clear(l);
    free(m);
    return CRYPTID_DECRYPTION_FAILED_ERROR;
}
