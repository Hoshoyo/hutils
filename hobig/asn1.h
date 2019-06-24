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

typedef enum {
    Sig_RSA,
    Sig_MD2WithRSA,
    Sig_MD5WithRSA,
    Sig_SHA1WithRSA,
    Sig_SHA256WithRSA,
    Sig_SHA384WithRSA,
    Sig_SHA512WithRSA,
    Sig_RSAPSS,
    Sig_DSAWithSHA1,
    Sig_DSAWithSHA256,
    Sig_ECDSAWithSHA1,
    Sig_ECDSAWithSHA256,
    Sig_ECDSAWithSHA384,
    Sig_ECDSAWithSHA512,
} Signature_Algorithm;

typedef struct {
    int         length;
    const char* data;
} Cert_Metadata;

typedef struct {
    HoBigInt            serial_number;
    PublicKey           public_key;
    Signature_Algorithm type;
    Signature_Algorithm signature_algorithm;
    Cert_Metadata       common_name;
    Cert_Metadata       country;
    Cert_Metadata       state;
    Cert_Metadata       locality;
    Cert_Metadata       organization;
    Cert_Metadata       email;
} RSA_Certificate;

PublicKey  asn1_parse_public_key_from_file(const char* filename, int* error);
PublicKey  asn1_parse_pem_public_key_from_file(const char* filename, int* error);
PrivateKey asn1_parse_pem_private_key_from_file(const char* filename, int* error);
RSA_Certificate asn1_parse_pem_certificate_from_file(const char* filename, int* error);

RSA_Certificate asn1_parse_pem_certificate(const unsigned char* data, int length, unsigned int* error, int is_base64_encoded);

// Public Key in the format of openssl
PublicKey asn1_parse_public_key(const unsigned char* data, int length, unsigned int* error, int is_base64_encoded);

// Public Key in the PEM format
PublicKey asn1_parse_pem_public(const unsigned char* data, int length, unsigned int* error, int is_base64_encoded);

// Private Key in the PEM format
PrivateKey asn1_parse_pem_private(const unsigned char* data, int length, unsigned int* error, int is_base64_encoded);

// Free functions
void public_key_free(PublicKey p);
void private_key_free(PrivateKey p);

// Print functions
void public_key_print(PublicKey pk);
void private_key_print(PrivateKey pk);