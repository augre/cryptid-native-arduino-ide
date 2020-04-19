#include "gmp.h"
#include "Validation.h"

/**
 * Check whether \f$p\f$ is a probable prime.
 * @param p the number to check
 * @result CRYPTID_VALIDATION_SUCCESS if \f$p\f$ is a probable prime
 */

#ifdef __cplusplus
extern "C" {
#endif

CryptidValidationResult primalityTest_isProbablePrime(const mpz_t p);

#ifdef __cplusplus
}
#endif
