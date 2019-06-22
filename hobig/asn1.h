#include "hobig.h"

typedef struct {
    char* data;
    int   length;
    int   error;
} Base64_Data;

Base64_Data base64_decode(const char* in, int length);

typedef struct {
    HoBigInt E; // public exponent
    HoBigInt N; // public modulus
} PublicKey;

typedef struct {
    PublicKey public;
    HoBigInt  P;
    HoBigInt  Q;
    HoBigInt  PrivateExponent;
} PrivateKey;

PublicKey asn1_parse_public_key_from_file(const char* filename, int* error);

// Public Key in the format of openssl
PublicKey asn1_parse_public_key(const unsigned char* data, int length, unsigned int* error);

// Public Key in the x509 PEM format
PublicKey asn1_parse_x509_public(const unsigned char* data, int length, unsigned int* error);

// Private Key in the x509 PEM format
PrivateKey asn1_parse_x509_private(const unsigned char* data, int length, unsigned int* error);

// Free functions
void public_key_free(PublicKey p);
void private_key_free(PrivateKey p);

// Print functions
void public_key_print(PublicKey pk);
void private_key_print(PrivateKey pk);