#include "gmp.h"

#include "EllipticCurve.h"
#include "PrimalityTest.h"

void ellipticCurve_init(EllipticCurve *ellipticCurveOutput, const mpz_t a, const mpz_t b, const mpz_t fieldOrder)
{
    mpz_inits(ellipticCurveOutput->a,ellipticCurveOutput->b, ellipticCurveOutput->fieldOrder, NULL);

    mpz_set(ellipticCurveOutput->a, a);
    mpz_set(ellipticCurveOutput->b, b);
    mpz_set(ellipticCurveOutput->fieldOrder, fieldOrder);
}

void ellipticCurve_initLong(EllipticCurve *ellipticCurveOutput, const long a, const long b, const long fieldOrder)
{
    mpz_inits(ellipticCurveOutput->a, ellipticCurveOutput->b, ellipticCurveOutput->fieldOrder, NULL);

    mpz_set_si(ellipticCurveOutput->a, a);
    mpz_set_si(ellipticCurveOutput->b, b);
    mpz_set_si(ellipticCurveOutput->fieldOrder, fieldOrder);
}

void ellipticCurve_destroy(EllipticCurve ellipticCurve)
{
    mpz_clears(ellipticCurve.a, ellipticCurve.b, ellipticCurve.fieldOrder, NULL);
}

CryptidValidationResult ellipticCurve_isTypeOne(const EllipticCurve ellipticCurve)
{
    if(!mpz_cmp_ui(ellipticCurve.a, 0)
        && !mpz_cmp_ui(ellipticCurve.b, 1)
        && primalityTest_isProbablePrime(ellipticCurve.fieldOrder))
    {
        return CRYPTID_VALIDATION_SUCCESS;
    }

    return CRYPTID_VALIDATION_FAILURE;
}