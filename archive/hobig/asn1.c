#include "asn1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define LIGHT_ARENA_IMPLEMENT
#include "light_arena.h"
#include "../light_array.h"
#include "hobig.h"

typedef unsigned char u8;
typedef unsigned int u32;

#define LITTLE_ENDIAN_32(X) (((X) << 24) | (((X) << 8) & 0xff0000) | (((X) >> 8) & 0xff00) | ((X) >> 24))

char base64_value(char c) {
    if(c >= 'A' && c <= 'Z') return c - 'A';
    if(c >= 'a' && c <= 'z') return c - 'a' + 26;
    if(c >= '0' && c <= '9') return c - '0' + 52;
    if(c == '+') return 62;
    if(c == '/') return 63;

    if(c == '=') {
        return 0;
    }
    return -1;
}

void print_bytes(char* b, int length) {
    for(int i = 0; i < length; ++i) {
        printf("%d ", (unsigned char)b[i]);
    }
}

int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f');
}

Base64_Data 
base64_decode(const char* in, int length) {
    Base64_Data result = {0};
    char* mem = calloc(4, length);

    int error = 0;

    int j = 0;
    for(int i = 0; i < length; i += 4, j += 3) {
        char v0 = base64_value(in[i + 0]);
        char v1 = base64_value(in[i + 1]);
        char v2 = base64_value(in[i + 2]);
        char v3 = base64_value(in[i + 3]);
        if(v0 == -1 || v1 == -1 || v2 == -1 || v3 == -1) {
            result.error = 1;
            break;
        }

        mem[j + 0] = (v0 << 2) | (v1 >> 4);
        mem[j + 1] = (v1 << 4) | (v2 >> 2);
        mem[j + 2] = (v2 << 6) | (v3);

        if(in[i + 2] == '=') j--;
        if(in[i + 3] == '=') j--;
    }

    if(error) {
        free(mem);
    } else {
        result.data = mem;
        result.length = j;
    }

    return result;
}

// Reference:
// https://docs.microsoft.com/en-us/windows/desktop/seccertenroll/about-der-encoding-of-asn-1-types
typedef enum {
    DER_BOOLEAN          = 0x1,
    DER_INTEGER          = 0x2,
    DER_BIT_STRING       = 0x3,
    DER_NULL             = 0x5,
    DER_OCT_STRING       = 0x4,
    DER_OBJECT_ID        = 0x6,
    DER_UTF8_STRING      = 0x0c,
    DER_PRINTABLE_STRING = 0x13,
    DER_IA5_STRING       = 0x16,
    DER_UTC_TIME         = 0x17,
    DER_BMP_STRING       = 0x1e,
    DER_SEQUENCE         = 0x30,
    DER_SET              = 0x31,
    DER_CONSTRUCTED_SEQ  = 0xa0,
} DER_Type;

typedef struct {
    struct DER_Node_t** data; // light_array
} DER_Sequence;

typedef struct {
    int length;
    int unused;
    const char* raw_data;
    struct DER_Node_t* data;
} DER_Bit_String;

typedef struct {
    int length;
    struct DER_Node_t* data;
} DER_Oct_String;

typedef struct {
    int         length;
    const char* data;
} DER_UTF8_String;

typedef struct {
    int length;
    const char* data;
} DER_Print_String;

typedef struct {
    int  length;
    int* data;
} DER_Object_ID;

typedef struct {
    int length;
    struct DER_Node_t* data;
} DER_Set;

typedef struct {
    int         length;
    const char* data;
} DER_UTC_Time;

typedef struct {
    HoBigInt i;
} DER_Integer;

typedef struct DER_Node_t {
    DER_Type kind;
    int      length;
    union {
        DER_Bit_String   bit_string;
        DER_Oct_String   oct_string;
        DER_UTF8_String  utf8_string;
        DER_Print_String printable_string;
        DER_Sequence     sequence;
        DER_Set          set;
        DER_Object_ID    object_id;
        DER_Integer      integer;
        DER_UTC_Time     utc_time;
    };
} DER_Node;

int der_get_length(u8* data, unsigned int* error, int* advance) {
    int length = 0;
    data++;
    (*advance)++;
    if((*data & 0x80) == 0) {
        // Only this bit specifies length
        length = *data;
        data++;
        (*advance)++;
    } else {
        // More than one byte specify length
        u8 extra_bytes = *data & 0x7f;
        data++;
        (*advance)++;
        if(extra_bytes > 4) {
            // TODO(psv): Specify error
            if(error) *error |= 1;
            return 0;
        } else {
            for(int i = 0; i < extra_bytes; ++i) {
                length |= (*data) << ((extra_bytes - i - 1) * 8);
                data++;
                (*advance)++;
            }
        }
    }
    return length;
}

DER_Node*
parse_der(Light_Arena* arena, u8* data, int total_length, unsigned int* error) {
    u8* at = data;
    int advance = 0;

    DER_Node* node = arena_alloc(arena, sizeof(DER_Node));

    if(*at == DER_NULL) {
        at++;
        if(*at++ != 0) {
            // length is not zero, therefore invalid null value
            if(error) {
                *error |= 1;
                return 0;
            }
        }
        node->kind = DER_NULL;
        node->length = 2;
        return node;
    }

    // Get encoded length
    int length = der_get_length(at, error, &advance);
    if(error && *error) return 0;
    u8 a = *at;
    at += advance;

    switch(a) {
        case DER_SEQUENCE: {
            node->kind = DER_SEQUENCE;
            node->length = length + advance;

            node->sequence.data = array_new(DER_Node);

            array_push(node->sequence.data, parse_der(arena, at, length, error));
            if(error && *error) return 0;

            length -= node->sequence.data[0]->length;
            at += node->sequence.data[0]->length;

            while(length) {
                DER_Node* n = parse_der(arena, at, length, error);
                array_push(node->sequence.data, n);
                length -= n->length;
                at += n->length;
            }
        } break;
        case DER_BIT_STRING: {
            int unused = *at++;
            node->kind = DER_BIT_STRING;
            node->length = length + advance;
            node->bit_string.length = length;
            node->bit_string.unused = unused;
            node->bit_string.raw_data = at;
            node->bit_string.data = parse_der(arena, at, length, error);
            at += length;
        } break;
        case DER_UTF8_STRING: {
            node->kind = DER_UTF8_STRING;
            node->length = length + advance;
            node->utf8_string.length = length;
            node->utf8_string.data = at;
            at += length;
        } break;
        case DER_OCT_STRING: {
            node->kind = DER_OCT_STRING;
            node->length = length + advance;
            node->oct_string.length = length;
            node->oct_string.data = parse_der(arena, at, length, error);
            at += length;
        } break;
        case DER_OBJECT_ID: {
            int* nodes = arena_alloc(arena, (length + 1) * sizeof(int));
            int v0 = *at;
            
            u8* end_ = at + length;
            at++;
            nodes[0] = v0 / 40;
            nodes[1] = v0 - (nodes[0] * 40);

            int k = 2;
            for(; at < end_; ++k) {
                if((*at & 0x80) == 0) {
                    nodes[k] = *at++ & 0x7f;
                } else {
                    u8* s = at;
                    while(*s & 0x80) s++;
                    int count = s - at + 1;
                    if(count > 4) {
                        // too many bytes encoded
                        if(error) *error |= 1;
                        return 0;
                    }
                    int res = 0;
                    for(int t = count - 1; t >= 0; --t) {
                        res |= ((*at) & 0x7f) << ((t * 8) - t);
                        at++;
                    }
                    nodes[k] = res;
                }
            }
            node->kind = DER_OBJECT_ID;
            node->length = length + advance;
            node->object_id.length = k;
            node->object_id.data = nodes;
        } break;
        case DER_INTEGER: {
            int extra_length = 0;
            if(*at & 0x80) {
                // negative not supported
                at++;
            } else if(*at == 0) {
                at++;
                length--;
                extra_length++;
            }
            HoBigInt b = hobig_int_new_from_memory(at, length);
            node->kind = DER_INTEGER;
            node->length = length + advance + extra_length;
            node->integer.i = b;
        } break;
        case DER_IA5_STRING:
        case DER_PRINTABLE_STRING: {
            node->kind = DER_PRINTABLE_STRING;
            node->length = length + advance;
            node->printable_string.length = length;
            node->printable_string.data = at;
        } break;
        case DER_SET: {
            node->kind = DER_SET;
            node->length = length + advance;
            node->set.length = length;
            node->set.data = parse_der(arena, at, length, error);
        } break;
        case DER_UTC_TIME: {
            node->kind = DER_UTC_TIME;
            node->length = length + advance;
            node->utc_time.length = length;
            node->utc_time.data = at;
        } break;
        case DER_CONSTRUCTED_SEQ: {
            node->kind = DER_CONSTRUCTED_SEQ;
            node->length = advance + length;
        } break;
        default: {
            node->kind = DER_NULL;
            node->length = advance + length;
        } break;
    }
    return node;
}

void
public_key_print(PublicKey pk) {
    printf("{ ");
    hobig_int_print(pk.N);
    printf(", ");
    hobig_int_print(pk.E);
    printf(" }\n");
}

void
private_key_print(PrivateKey pk) {
    printf("{\n\tPublic Key: ");
    public_key_print(pk.public);
    printf("\n\tPrivateExponent: ");
    hobig_int_print(pk.PrivateExponent);
    printf(",\n\tP: ");
    hobig_int_print(pk.P);
    printf(",\n\tQ: ");
    hobig_int_print(pk.Q);
    printf("\n}");
}

void
public_key_free(PublicKey p) {
    hobig_free(p.E);
    hobig_free(p.N);
}

void
private_key_free(PrivateKey p) {
    hobig_free(p.public.E);
    hobig_free(p.public.N);
    hobig_free(p.PrivateExponent);
    hobig_free(p.P);
    hobig_free(p.Q);
}

// Parses a PEM file of a private key.
// Reference:
// https://crypto.stackexchange.com/questions/21102/what-is-the-ssl-private-key-file-format
PrivateKey
asn1_parse_pem_private(const u8* data, int length, u32* error, int is_base64_encoded) {
    Base64_Data r = {0};
    if(is_base64_encoded) {
        r = base64_decode(data, length);
        if(r.error != 0) {
            fprintf(stderr, "Could not parse key base64 data\n");
            free(r.data);
            if(error) *error |= 1;
            return (PrivateKey){0};
        }
    } else {
        r.data = (char*)data;
        r.length = length;
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);
    PrivateKey key = {0};

    #define FATAL_PEM_PRIVATE(T) if(error) *error |= 1; \
        fprintf(stderr, T); \
        if(is_base64_encoded) free(r.data); \
        arena_free(arena); \
        private_key_free(key); \
        return (PrivateKey){0};

    if(node->kind != DER_SEQUENCE) {
        FATAL_PEM_PRIVATE("Could not parse private key, expected DER sequence\n");
    }

    DER_Node** seq = node->sequence.data;
    if(!seq || array_length(seq) < 6) {
        FATAL_PEM_PRIVATE("Could not parse private key, not enough integers in the file\n");
    }

    for(int i = 0; i < 6; ++i) {
        if(seq[i]->kind != DER_INTEGER) {
            FATAL_PEM_PRIVATE("Could not parse private key, not enough integers in the file\n");
        }
    }

    if(seq[0]->integer.i.value != 0) {
        FATAL_PEM_PRIVATE("PEM file does not contain RSA with 2 primes\n");
    }

    // Public modulus
    key.public.N = seq[1]->integer.i;
    key.public.E = seq[2]->integer.i;
    key.PrivateExponent = seq[3]->integer.i;
    key.P = seq[4]->integer.i;
    key.Q = seq[5]->integer.i;
    
    for(int i = 6; i < array_length(seq); ++i) {
        if(seq[i]->kind == DER_INTEGER) {
            hobig_free(seq[i]->integer.i);
        }
    }

    free(r.data);
    arena_free(arena);
    return key;
}

static Signature_Algorithm signature_algorithm_from_oid(DER_Object_ID oid);

// Parses PEM file of a public key
// Reference: https://medium.com/@bn121rajesh/understanding-rsa-public-key-70d900b1033c
PublicKey
asn1_parse_pem_public(const u8* data, int length, u32* error, int is_base64_encoded) {
    Base64_Data r = {0};
    
    if(is_base64_encoded) {
        r = base64_decode(data, length);
        if(r.error != 0) {
            fprintf(stderr, "Could not parse key base64 data\n");
            free(r.data);
            if(error) *error |= 1;
            return (PublicKey){0};
        }
    } else {
        r.data = (char*)data;
        r.length = length;
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);
    PublicKey pk = {0};

    #define FATAL_PEM_PUBLIC(T) if(error) *error |= 1; \
        fprintf(stderr, T); \
        if(is_base64_encoded) free(r.data); \
        arena_free(arena); \
        return (PublicKey){0};

    if(node->kind != DER_SEQUENCE) {
        FATAL_PEM_PUBLIC("Invalid format, expected DER sequence\n");
    }

    DER_Node** seq = node->sequence.data;

    if(array_length(seq) < 2 || seq[0]->kind != DER_SEQUENCE || array_length(seq[0]->sequence.data) < 2) {
        FATAL_PEM_PUBLIC("Invalid format, Sequence must contain OID and Key information\n");
    }

    DER_Node* oid = seq[0]->sequence.data[0];
    if(oid->kind != DER_OBJECT_ID || seq[0]->sequence.data[1]->kind != DER_NULL) {
        FATAL_PEM_PUBLIC("Invalid format, expected OID followed by NULL\n");
    }
    
    Signature_Algorithm alg = signature_algorithm_from_oid(oid->object_id);
    
    if(alg != Sig_RSA) {
        FATAL_PEM_PUBLIC("Invalid format, unsupported encryption format\n");
    }

    if(seq[1]->kind != DER_BIT_STRING || seq[1]->bit_string.data->kind != DER_SEQUENCE) {
        FATAL_PEM_PUBLIC("Invalid format, expected sequence in bit string\n");
    }
    DER_Node** values = seq[1]->bit_string.data->sequence.data;
    if(array_length(values) < 2 || values[0]->kind != DER_INTEGER || values[1]->kind != DER_INTEGER) {
        FATAL_PEM_PUBLIC("Invalid format, could not find modulus and exponent in bit string\n");
    }
    pk.N = values[0]->integer.i;
    pk.E = values[1]->integer.i;

    free(r.data);
    arena_free(arena);
    return pk;
}

PublicKey
asn1_parse_public_key(const u8* data, int length, u32* error, int is_base64_encoded) {
    Base64_Data r = {0};

    if(is_base64_encoded) {
        r = base64_decode(data, length);
        if(r.error != 0) {
            fprintf(stderr, "Could not parse key base64 data\n");
            if(error) *error |= 1;
        }
    } else {
        r.data = (char*)data;
        r.length = length;
    }

    char* at = r.data;

    // algorithm length
    int prefix_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;
    if(prefix_length != sizeof("ssh-rsa") - 1){
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected 'ssh-rsa'\n");
        free(r.data);
        return (PublicKey){0};
    }
    at += prefix_length;

    int exponent_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;

    if(exponent_length > 4096 * 4) {
        if(error) *error |= 1;
        fprintf(stderr, "Public exponent length is too big '%d' bytes\n", exponent_length);
        free(r.data);
        return (PublicKey){0};
    }

    HoBigInt exponent = hobig_int_new_from_memory(at, exponent_length);
    at += exponent_length;

    int modulus_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;

    if(modulus_length > 4096 * 4) {
        if(error) *error |= 1;
        fprintf(stderr, "Public modulus length is too big '%d' bytes\n", modulus_length);
        free(r.data);
        return (PublicKey){0};
    }

    HoBigInt modulus = hobig_int_new_from_memory(at, modulus_length);

    PublicKey result = {0};
    result.E = exponent;
    result.N = modulus;

    free(r.data);

    return result;
}

#define FATAL_CERTIFICATE_PEM(X) if(error) *error |= 1; \
    fprintf(stderr, X); \
    if(is_base64_encoded) free(r.data); \
    arena_free(arena); \
    return (RSA_Certificate){0}

static Signature_Algorithm
signature_algorithm_from_oid(DER_Object_ID oid) {
    // All asn1 signatures of known algorithms encoded as OID
    static int oid_signature_RSA[]      = {1, 2, 840, 113549, 1, 1, 1};
    static int oid_signature_MD2WithRSA[]      = {1, 2, 840, 113549, 1, 1, 2};
    static int oid_signature_MD5WithRSA[]      = {1, 2, 840, 113549, 1, 1, 4};
    static int oid_signature_SHA1WithRSA[]     = {1, 2, 840, 113549, 1, 1, 5};
    static int oid_signature_SHA256WithRSA[]   = {1, 2, 840, 113549, 1, 1, 11};
    static int oid_signature_SHA384WithRSA[]   = {1, 2, 840, 113549, 1, 1, 12};
    static int oid_signature_SHA512WithRSA[]   = {1, 2, 840, 113549, 1, 1, 13};
    static int oid_signature_RSAPSS[]          = {1, 2, 840, 113549, 1, 1, 10};
    static int oid_signature_DSAWithSHA1[]     = {1, 2, 840, 10040, 4, 3};
    static int oid_signature_DSAWithSHA256[]   = {2, 16, 840, 1, 101, 3, 4, 3, 2};
    static int oid_signature_ECDSAWithSHA1[]   = {1, 2, 840, 10045, 4, 1};
    static int oid_signature_ECDSAWithSHA256[] = {1, 2, 840, 10045, 4, 3, 2};
    static int oid_signature_ECDSAWithSHA384[] = {1, 2, 840, 10045, 4, 3, 3};
    static int oid_signature_ECDSAWithSHA512[] = {1, 2, 840, 10045, 4, 3, 4};

    int length_bytes = oid.length * sizeof(int);

    #define OID_EQUAL(A, S) if(length_bytes == sizeof(A) && memcmp(oid.data, A, length_bytes) == 0) return S

    OID_EQUAL(oid_signature_RSA, Sig_RSA);
    OID_EQUAL(oid_signature_SHA1WithRSA, Sig_SHA1WithRSA);
    OID_EQUAL(oid_signature_SHA256WithRSA, Sig_SHA256WithRSA);
    OID_EQUAL(oid_signature_SHA384WithRSA, Sig_SHA384WithRSA);
    OID_EQUAL(oid_signature_SHA512WithRSA, Sig_SHA512WithRSA);
    OID_EQUAL(oid_signature_RSAPSS, Sig_RSAPSS);
    OID_EQUAL(oid_signature_DSAWithSHA1, Sig_DSAWithSHA1);
    OID_EQUAL(oid_signature_DSAWithSHA256, Sig_DSAWithSHA256);
    OID_EQUAL(oid_signature_ECDSAWithSHA1, Sig_ECDSAWithSHA1);
    OID_EQUAL(oid_signature_ECDSAWithSHA256, Sig_ECDSAWithSHA256);
    OID_EQUAL(oid_signature_ECDSAWithSHA384, Sig_ECDSAWithSHA384);
    OID_EQUAL(oid_signature_ECDSAWithSHA512, Sig_ECDSAWithSHA512);
    OID_EQUAL(oid_signature_MD2WithRSA, Sig_MD2WithRSA);
    OID_EQUAL(oid_signature_MD5WithRSA, Sig_MD5WithRSA);

    #undef OID_EQUAL
    return -1;
}

static void 
metadata_from_object_id(RSA_Certificate* certificate, DER_Object_ID oid, Cert_Metadata str) {
    int length_bytes = oid.length * sizeof(int);

    // Reference: https://www.alvestrand.no/objectid/2.5.4.html
    static int attribute_types[] = {2, 5, 4};
    static int pkcs9_signatures[] = {1, 2, 840, 113549, 1, 9};

    #define  oid_aliased_entry_name 1
    #define  oid_common_name 3
    #define  oid_country_name 6
    #define  oid_state_name 8
    #define  oid_locality_name 7
    #define  oid_organization_name 10
    #define  oid_organization_unit_name 11

    #define oid_email_address 1

    switch(oid.length) {
        case 4:
        {
            if(memcmp(attribute_types, oid.data, sizeof(int) * 3) == 0) {
                // attribute types
                switch(oid.data[3]) {
                    case oid_aliased_entry_name:
                    case oid_organization_unit_name:
                    default: break;

                    case oid_common_name:
                        certificate->common_name = str;
                        break;
                    case oid_country_name:
                        certificate->country = str;
                        break;
                    case oid_locality_name:
                        certificate->locality = str;
                        break;
                    case oid_organization_name:
                        certificate->organization = str;
                        break;
                    case oid_state_name:
                        certificate->state = str;
                        break;
                }
            }
        }break;
        case 7:
        {
            // email supported
            // https://www.alvestrand.no/objectid/1.2.840.113549.1.9.html
            if(memcmp(pkcs9_signatures, oid.data, sizeof(int) * 6) == 0 && oid.data[6] == oid_email_address) {
                certificate->email = str;
                return;
            }
        }break;
        default: // unsupported
        break;
    }
}

RSA_Certificate
asn1_parse_pem_certificate(const u8* data, int length, u32* error, int is_base64_encoded) {
    Base64_Data r = {0};

    if(is_base64_encoded) {
        r = base64_decode(data, length);
        if(r.error != 0) {
            fprintf(stderr, "Could not parse key base64 data\n");
            free(r.data);
            if(error) *error |= 1;
        }
    } else {
        r.data = (char*)data;
        r.length = length;
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);

    RSA_Certificate certificate = {0};

    DER_Node* at = node;
    if(node->kind != DER_SEQUENCE) {
        FATAL_CERTIFICATE_PEM("Could not parse PEM certificate, expected DER sequence\n");
    }

    at = at->sequence.data[0];
    if(!at || at->kind != DER_SEQUENCE) {
        FATAL_CERTIFICATE_PEM("Could not parse PEM certificate, expected DER sequence\n");
    }

    for(int i = 0; i < array_length(at->sequence.data); ++i) {
        // each entry
        DER_Node* node = at->sequence.data[i];
        switch(node->kind) {
            case DER_INTEGER: {
                // Serial number
                certificate.serial_number = node->integer.i;
            } break;
            case DER_SEQUENCE: {
                for(int j = 0; j < array_length(node->sequence.data); ++j) {
                    DER_Node* metadata = node->sequence.data[j];
                    if(metadata->kind == DER_BIT_STRING) {
                        // Public Key
                        if(!metadata->bit_string.data || metadata->bit_string.data->kind != DER_SEQUENCE) {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, could not find Public Key integer\n");
                        }
                        DER_Node** seq = metadata->bit_string.data->sequence.data;
                        if(array_length(seq) != 2 || seq[0]->kind != DER_INTEGER || seq[1]->kind != DER_INTEGER) {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, Public Key does not contain exponent and modulus\n");
                        }
                        certificate.public_key.N = seq[0]->integer.i;
                        certificate.public_key.E = seq[1]->integer.i;
                    } else if(metadata->kind == DER_OBJECT_ID) {
                        // type of signature
                        certificate.signature_algorithm = signature_algorithm_from_oid(metadata->object_id);
                    } else if(metadata->kind == DER_SET) {
                        DER_Node* m = metadata->set.data;
                        if(m->kind != DER_SEQUENCE) {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, expecting DER sequence\n");
                        }
                        // Always OID and (PRINTABLE_STRING | UTF8_STRING)
                        DER_Node** sq = m->sequence.data;
                        if(array_length(sq) != 2 || sq[0]->kind != DER_OBJECT_ID) {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, expecting sequence of OID and string\n");
                        }
                        // OID
                        DER_Object_ID oid = sq[0]->object_id;
                        DER_Node* str = sq[1];
                        Cert_Metadata s = {0};
                        if(str->kind == DER_PRINTABLE_STRING || str->kind == DER_IA5_STRING) {
                            s = (Cert_Metadata){str->printable_string.length, str->printable_string.data};
                        } else if(str->kind == DER_UTF8_STRING) {
                            s = (Cert_Metadata){str->utf8_string.length, str->utf8_string.data};
                        } else {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, metadata OID must be followed by a string\n");
                        }
                        metadata_from_object_id(&certificate, oid, s);
                    } else if(metadata->kind == DER_SEQUENCE) {
                        // OID and Key Type
                        DER_Node** key = metadata->sequence.data;
                        if(array_length(key) != 2 || key[0]->kind != DER_OBJECT_ID) {
                            FATAL_CERTIFICATE_PEM("Fail to parse PEM metadata, Key type must be OID + NULL\n");
                        }
                        certificate.type = signature_algorithm_from_oid(key[0]->object_id);
                    }
                }
            } break;
            default: {

            } break;
        }
    }
    return certificate;
}

static void*
load_entire_file(const char* filename, int* out_size) {
    FILE* file = fopen(filename, "rb");

    if(!file) {
        fprintf(stderr, "Could not find file %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    int file_size = (int)ftell(file);
    fseek(file, 0, SEEK_SET);

    if(file_size == 0) {
        fclose(file);
        fprintf(stderr, "File %s is empty\n", filename);
        return 0;
    }

    void* memory = calloc(1, file_size + 1);

    if(fread(memory, file_size, 1, file) != 1) {
        fclose(file);
        free(memory);
        fprintf(stderr, "Could not read entire file %s\n", filename);
        return 0;
    }
    fclose(file);

    if(out_size) *out_size = file_size;
    return memory;
}

PublicKey 
asn1_parse_public_key_from_file(const char* filename, int* error) {
    int file_size = 0;
    void* memory = load_entire_file(filename, &file_size);
    if(!memory) { if(error) *error |= 1; return (PublicKey){0}; };

    // File contents are loaded
    char* at = memory;
    int at_index = 0;
    while(is_whitespace(*at)) { at++; at_index++; }
    if(file_size < sizeof("ssh-rsa") - 1) {
        // file size is not long enough
        free(memory);
        fprintf(stderr, "File format is invalid\n");
        if(error) *error = 1;
        return (PublicKey){0};
    }
    if(strncmp("ssh-rsa", at, sizeof("ssh-rsa") - 1) != 0) {
        free(memory);
        fprintf(stderr, "Invalid file format, expected ssh-rsa prefix\n");
        if(error) *error = 1;
        return (PublicKey){0};
    }
    at += sizeof("ssh-rsa") - 1;
    while(is_whitespace(*at)) { at++; at_index++; }

    char* start_data = at;
    while(!is_whitespace(*at)) { at++; at_index++; }
    PublicKey pubk = asn1_parse_public_key(start_data, at - start_data, error, 1);

    // email may follow but we ignore it

    return pubk;
}

PublicKey
asn1_parse_pem_public_key_from_file(const char* filename, int* error) {
    int file_size = 0;
    void* memory = load_entire_file(filename, &file_size);
    if(!memory) { if(error) *error |= 1; return (PublicKey){0}; };

    // File contents are loaded
    char* at = memory;
    int at_index = 0;

    while(is_whitespace(*at)) { at++; at_index++; }
    if((file_size - at_index < sizeof("-----BEGIN PUBLIC KEY-----") - 1) ||
        strncmp("-----BEGIN PUBLIC KEY-----", at, sizeof("-----BEGIN PUBLIC KEY-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(memory);
        fprintf(stderr, "File format invalid, expected -----BEGIN PUBLIC KEY-----\n");
        return (PublicKey){0};
    }

    at += sizeof("-----BEGIN PUBLIC KEY-----") - 1;
    while(is_whitespace(*at)) { at++; at_index++; }

    char* start_data = at;
    while(*at != '-') { at++; at_index++; }
    int data_bytes = at - start_data;
    char* d = calloc(1, data_bytes + 1);

    // -----END PUBLIC KEY----- follows
    if((file_size - at_index < sizeof("-----END PUBLIC KEY-----") - 1) ||
        strncmp("-----END PUBLIC KEY-----", at, sizeof("-----END PUBLIC KEY-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(d);
        free(memory);
        fprintf(stderr, "File format invalid, expected -----END PUBLIC KEY-----\n");
        return (PublicKey){0};
    }

    at = start_data;
    int trimmed_length = 0;
    for(int i = 0, k = 0; i < data_bytes; ++i, at++) {
        if(!is_whitespace(*at)) {
            d[k++] = *at;
            trimmed_length++;
        }
    }

    PublicKey result = asn1_parse_pem_public(d, trimmed_length, error, 1);

    return result;
}

PrivateKey
asn1_parse_pem_private_key_from_file(const char* filename, int* error) {
    int file_size = 0;
    void* memory = load_entire_file(filename, &file_size);
    if(!memory) { if(error) *error |= 1; return (PrivateKey){0}; };

    // File contents are loaded
    char* at = memory;
    int at_index = 0;

    while(is_whitespace(*at)) { at++; at_index++; }
    if((file_size - at_index < sizeof("-----BEGIN RSA PRIVATE KEY-----") - 1) ||
        strncmp("-----BEGIN RSA PRIVATE KEY-----", at, sizeof("-----BEGIN RSA PRIVATE KEY-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(memory);
        fprintf(stderr, "File format invalid, expected -----BEGIN RSA PRIVATE KEY-----\n");
        return (PrivateKey){0};
    }

    at += sizeof("-----BEGIN RSA PRIVATE KEY-----") - 1;
    while(is_whitespace(*at)) { at++; at_index++; }

    char* start_data = at;
    while(*at != '-') { at++; at_index++; }
    int data_bytes = at - start_data;
    char* d = calloc(1, data_bytes + 1);

    // -----END RSA PRIVATE KEY----- follows
    if((file_size - at_index < sizeof("-----END RSA PRIVATE KEY-----") - 1) ||
        strncmp("-----END RSA PRIVATE KEY-----", at, sizeof("-----END RSA PRIVATE KEY-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(d);
        free(memory);
        fprintf(stderr, "File format invalid, expected -----END PUBLIC KEY-----\n");
        return (PrivateKey){0};
    }

    at = start_data;
    int trimmed_length = 0;
    for(int i = 0, k = 0; i < data_bytes; ++i, at++) {
        if(!is_whitespace(*at)) {
            d[k++] = *at;
            trimmed_length++;
        }
    }

    PrivateKey result = asn1_parse_pem_private(d, trimmed_length, error, 1);

    return result;
}

RSA_Certificate 
asn1_parse_pem_certificate_from_file(const char* filename, int* error) {
    //-----BEGIN CERTIFICATE-----
    int file_size = 0;
    void* memory = load_entire_file(filename, &file_size);
    if(!memory) { if(error) *error |= 1; return (RSA_Certificate){0}; };

    // File contents are loaded
    char* at = memory;
    int at_index = 0;

    while(is_whitespace(*at)) { at++; at_index++; }
    if((file_size - at_index < sizeof("-----BEGIN CERTIFICATE-----") - 1) ||
        strncmp("-----BEGIN CERTIFICATE-----", at, sizeof("-----BEGIN CERTIFICATE-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(memory);
        fprintf(stderr, "File format invalid, expected -----BEGIN CERTIFICATE-----\n");
        return (RSA_Certificate){0};
    }

    at += sizeof("-----BEGIN CERTIFICATE-----") - 1;
    while(is_whitespace(*at)) { at++; at_index++; }

    char* start_data = at;
    while(*at != '-') { at++; at_index++; }
    int data_bytes = at - start_data;
    char* d = calloc(1, data_bytes + 1);

    // -----END CERTIFICATE----- follows
    if((file_size - at_index < sizeof("-----END CERTIFICATE-----") - 1) ||
        strncmp("-----END CERTIFICATE-----", at, sizeof("-----END CERTIFICATE-----") - 1) != 0) 
    {
        // File is not large enough or pattern not found
        free(d);
        free(memory);
        fprintf(stderr, "File format invalid, expected -----END CERTIFICATE-----\n");
        return (RSA_Certificate){0};
    }

    at = start_data;
    int trimmed_length = 0;
    for(int i = 0, k = 0; i < data_bytes; ++i, at++) {
        if(!is_whitespace(*at)) {
            d[k++] = *at;
            trimmed_length++;
        }
    }

    RSA_Certificate result = asn1_parse_pem_certificate(d, trimmed_length, error, 1);

    return result;
}