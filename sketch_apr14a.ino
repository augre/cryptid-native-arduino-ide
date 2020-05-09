


#define __CRYPTID_BONEH_FRANKLIN_IDENTITY_BASED_ENCRYPTION
#define __CRYPTID_EXTERN_RANDOM

#include "BonehFranklinIdentityBasedEncryption.h"
#include "BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary.h"
#include "BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary.h"

#include <math.h>
#include "mini-gmp.h"
#include "Validation.h"
#include "Utils.h"
#include "Complex.h"
#include "AffinePoint.h"
#include "EllipticCurve.h"


char *plaintext;

int i=0;
unsigned char rbyte;

void setup() {
  // put your setup code here, to run once:



    Serial.begin(9600); // open the serial port at 9600 bps:

    const char *identity = "darth.plagueis@sith.com";
    const char *message = "adfadjfa";


    Serial.println("\n\n\n\nStarting\n");
    BonehFranklinIdentityBasedEncryptionPublicParametersAsBinary publicParameters;
    BonehFranklinIdentityBasedEncryptionMasterSecretAsBinary masterSecret;

    CryptidStatus status_ = cryptid_ibe_bonehFranklin_setup(&masterSecret, &publicParameters, LOWEST);
    if (status_)
    {
        Serial.println("\n\n\n\nSetup failed\n");
    }
    else Serial.println("\n\n\n\nSetup success\n");

    
    AffinePointAsBinary privateKey;
    status_ = cryptid_ibe_bonehFranklin_extract(&privateKey, identity, strlen(identity), masterSecret, publicParameters);
    if (CRYPTID_SUCCESS != status_)
    {
        Serial.println("Extract failed\n");
        plaintext = "fail";
    }
    else Serial.println("\n\n\nExtract success\n");

    BonehFranklinIdentityBasedEncryptionCiphertextAsBinary ciphertext;
    status_ = cryptid_ibe_bonehFranklin_encrypt(&ciphertext, message, strlen(message), identity, strlen(identity), publicParameters);
    if (CRYPTID_SUCCESS != status_)
    {
        Serial.println("Encrypt failed\n");
        plaintext = "fail";
    }
    else Serial.println("\n\n\nEncrypt success\n");

    status_ = cryptid_ibe_bonehFranklin_decrypt(&plaintext, ciphertext, privateKey, publicParameters);
    if (CRYPTID_SUCCESS != status_)
    {
        Serial.println("Decrypt failed\n");
        plaintext = "fail";
    }    
    else Serial.println("\n\n\nDecrypt success\n");

    free(plaintext);
    bonehFranklinIdentityBasedEncryptionCiphertextAsBinary_destroy(ciphertext);
    affineAsBinary_destroy(privateKey);
    free(masterSecret.masterSecret);
    bonehFranklinIdentityBasedEncryptionPublicParametersAsBinary_destroy(publicParameters);

}


void loop() {
  // put your main code here, to run repeatedly:
//    char *buf;
// 
//    char *buffer_p = (char*)malloc(sizeof(char)*10);
//
//
//
//    buf = (char*) malloc(2);
//    rbyte = random(0, 255);
//    buf[0] = rbyte;
//    rbyte = random(0, 255);
//    buf[1] = rbyte;
//    
//    while(i<50){
//      Serial.println(buf); 
//      Serial.println((int)buf, BIN); 
//
//      i+=1;
//    }

}
