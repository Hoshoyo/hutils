#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../light_array.h"
#include "hobig.c"

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

typedef struct {
    char* data;
    int   length;
    int   error;
} Base64_Data;

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
    DER_BMP_STRING       = 0x1e,
    DER_SEQUENCE         = 0x30,
    DER_SET              = 0x31,
} DER_Type;

typedef struct {
    int length;
    struct DER_Node_t* data;
} DER_Sequence;

typedef struct {
    int length;
    int unused;
    struct DER_Node_t* data;
} DER_Bit_String;

typedef struct {
    int  length;
    int* data;
} DER_Object_ID;

typedef struct {
    HoBigInt i;
} DER_Integer;

typedef struct DER_Node_t {
    DER_Type       kind;
    int            length;
    union {
        DER_Bit_String bit_string;
        DER_Sequence   sequence;
        DER_Object_ID  object_id;
        DER_Integer    integer;
    };
    struct DER_Node_t* next;
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
parse_der(u8* data, int length, unsigned int* error) {
    u8* at = data;
    int advance = 0;
    switch(*at) {
        case DER_SEQUENCE: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

            DER_Node* node = calloc(1, sizeof(DER_Node));
            node->kind = DER_SEQUENCE;
            node->length = length + advance;
            node->sequence.length = length;
            node->sequence.data = parse_der(at, length, error);
            if(error && *error) return 0;

            length -= node->sequence.data->length;
            at += node->sequence.data->length;

            DER_Node* n = node;
            while(length) {
                n->next = parse_der(at, length, error);
                n = n->next;
                length -= n->length;
                at += n->length;
            }
            return node;
        } break;
        case DER_BIT_STRING: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

            int unused = *at++;
            DER_Node* node = calloc(1, sizeof(DER_Node));
            node->kind = DER_BIT_STRING;
            node->length = length + advance;
            node->bit_string.length = length;
            node->bit_string.unused = unused;
            node->bit_string.data = parse_der(at, length, error);
            at += length;
            return node;
        } break;
        case DER_OBJECT_ID: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

            int* nodes = calloc(1, (length + 1) * sizeof(int));
            int v0 = *at;
            at++;
            nodes[0] = v0 / 40;
            nodes[1] = v0 - (nodes[0] * 40);

            for(int k = 2; k < length + 1; ++k) {
                if((*at & 0x80) == 0) {
                    nodes[k] = *at++ & 0x7f;
                } else {
                    // ignore it for now
                    at++;
                }
            }
            DER_Node* node = calloc(1, sizeof(DER_Node));
            node->kind = DER_OBJECT_ID;
            node->length = length + advance;
            node->object_id.length = length + 1;
            node->object_id.data = nodes;
            return node;
        } break;
        case DER_INTEGER: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;
            int extra_length = 0;
            if(*at & 0x80) {
                // negative not supported
                at++;
            } else if(*at == 0) {
                at++;
                length--;
                extra_length++;
            }
            HoBigInt b = hobig_int_from_memory(at, length, 0);
            DER_Node* node = calloc(1, sizeof(DER_Node));
            node->kind = DER_INTEGER;
            node->length = length + advance + extra_length;
            node->integer.i = b;

            return node;
        } break;
        case DER_NULL:{
            at++;
            if(*at++ != 0) {
                // length is not zero, therefore invalid null value
                if(error) {
                    *error |= 1;
                    return 0;
                }
            }
            DER_Node* node = calloc(1, sizeof(DER_Node));
            node->kind = DER_NULL;
            node->length = 2;
            return node;
        } break;
        break;
    }
    return 0;
}

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

void
public_key_print(PublicKey pk) {
    printf("{ ");
    hobig_int_print(pk.N);
    printf(", ");
    hobig_int_print(pk.E);
    printf(" }");
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

PrivateKey
asn1_parse_x509_private(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        if(error) *error |= 1;
    }

    DER_Node* node = parse_der(r.data, r.length, error);
    PrivateKey key = {0};

    if(node->kind != DER_SEQUENCE) {
        if(error) *error |= 1;
        fprintf(stderr, "Could not parse private key, expected DER sequence\n");
        return (PrivateKey){0};
    }

    DER_Node* rsa_indication = node->sequence.data;
    if(rsa_indication->kind != DER_INTEGER) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid RSA version, expected integer entry\n");
        return (PrivateKey){0};
    }
    if(rsa_indication->integer.i.value[0] != 0) {
        // must be 0 indicating 2 primes
        if(error) *error |= 1;
        fprintf(stderr, "Invalid RSA version, expected 0 value to indicate RSA key of 2 primes\n");
        return (PrivateKey){0};
    }
    DER_Node* next = node->next;
    if(next->kind == DER_INTEGER) {
        key.public.N = next->integer.i;
    } else {
        // error expected integer public modulus
        if(error) *error |= 1;
        fprintf(stderr, "Could not find public modulus in the file\n");
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.public.E = next->integer.i;
    } else {
        // error expected integer public exponent
        if(error) *error |= 1;
        fprintf(stderr, "Could not find public exponent in the file\n");
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.PrivateExponent = next->integer.i;
    } else {
        // error expected private exponent
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private exponent in the file\n");
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.P = next->integer.i;
    } else {
        // error expected private P
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private prime P in the file\n");
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.Q = next->integer.i;
    } else {
        // error expected private Q
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private prime Q in the file\n");
        return (PrivateKey){0};
    }

    return key;
}

PublicKey
asn1_parse_x509_public(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        if(error) *error |= 1;
    }

    DER_Node* node = parse_der(r.data, r.length, error);
    PublicKey pk = {0};

    if(node->kind != DER_SEQUENCE) {
        // error
        if(error) *error |= 1;
        return pk;
    }
    // ignore
    // node->sequence.data
    if(!node->next || node->next->kind != DER_BIT_STRING) {
        // error
        if(error) *error |= 1;
        return pk;
    }

    if(node->next->bit_string.data->kind != DER_SEQUENCE) {
        // error
        if(error) *error |= 1;
        return pk;
    }

    DER_Node* modulus = node->next->bit_string.data->sequence.data;
    if(modulus->kind != DER_INTEGER) {
        // error
        if(error) *error |= 1;
        return pk;
    }

    DER_Node* exponent = node->next->bit_string.data->next;
    if(exponent->kind != DER_INTEGER) {
        // error
        if(error) *error |= 1;
        return pk;
    }

    // TODO(psv): free des tree
    pk.E = exponent->integer.i;
    pk.N = modulus->integer.i;

    public_key_print(pk);
    return pk;
}

PublicKey
asn1_parse_public_key(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        if(error) *error |= 1;
    }

    char* at = r.data;

    int prefix_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;
    at += prefix_length;

    int exponent_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;

    HoBigInt exponent = hobig_int_from_memory(at, exponent_length, 0);
    at += exponent_length;

    int modulus_length = LITTLE_ENDIAN_32(*(int*)at);
    at += 4;

    HoBigInt modulus = hobig_int_from_memory(at, modulus_length, 0);

    PublicKey result = {0};
    result.E = exponent;
    result.N = modulus;

    return result;
}

int main(int argc, char** argv) {
    // Cure public key
    //char* key = "AAAAB3NzaC1yc2EAAAADAQABAAACAQDXZn2FZvALUt3kYWoOg0PZsHsBunPUwlEyKeFny4BOl1qRXMi0IihEvTHzJplMIdeX2tOVuLEMq3WcWOkZm7wELbze+KQytHLyx9CKT3c2PYrBjBGtDh4UiI9F1Do1BqsJo78dTWpwgc7HiPyRaAQkVPo/d/eN0X3tdD8IbuYNiJ6HrxVJNDpdpG7iBoPESK7IX0AHJ5NjhWFkBSct015Lxt683yTNKvdHOH5t4CdnyNckwS5fYHT51fioNknj7OeqH4ey7MJ8k8rLgAI3gZ1xzkxZFIiNhoMG0hoTagdituaMFkwHsnwKOqsfouNtUpCyOcj71xgw76OWhjnhxTCTbB3MWe4H5G2eQTFYoHSJ4uvKgQlWg7I76XzDIvqFpaKEANVUtVA8SH74GiXkHZKokgNtZ7tYKWLLIniCyuZWohMeGM1Cam9rSM5VMVT1/sO4b+x+s0aB/RrgKKoWT6H3IR9LjobFTxdnM5UhdnNpvGKwTpKxg/2UfP1HqXcnChfCoGnk9RhweSmASv5DhsvcOjO2pAjAQoK3672DmGPj/Zp0zb+vOT/gDYtP2oopfYnswSt48T6WGBSRMsBmUISDB3XgzAcXSBuqbpn78OP/EMnFUpcAPct9jn0f1TfGy8K8ib10HN/WweHQWC09SI3OcqmiiK3l0xdSQ892M7s/YQ==";
    //char* key = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA6yXyPWWLXoGOhgg9hJEY7U9zaG4UNLzHCq5TcjQq6+6XwTSr8rwoblixzDxzy7kR5teQdh2OjaaHiqd/TrkXxl1/2UHnGZUtOsHbA/Z/bH7Vh4M4fzt9PNLqj9Oi71keY9DtOalDWIadKeQ9oKkM0ItjrAR97lomZG5pHpc4EIjFHcm3J8kXBNJFpOIVKx3qOeQDX/XmORKu+Ai9hMhw/FaB4Kkye4qrOEKHN4PHV+m7UPl70u+Qns+VGyWWqk/8iQnPRL5asNJntVQaCly7K50EMH/lZng4y70a9mz8tBzJyvPcS5s1OBdtb64jyJcujAXUtPm/LTRsOoN+1m624QIDAQAB";

    u32 error = 0;
    char* key = "MIIEowIBAAKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQABAoIBAQCKKq3UoI2Pq749MFjSdFjZHAMrF/AJennFPzy36dSA6qIahltKVEr45IOeG+h5S691fz0/jwRav/PTDEu0qfihtSVbL4NLggwhKG3GWUt4NshfsNouKNI8bPippHdIfW43drkla2ieZUp41o6eh+dGZe3EzkmQc7y3btTQF0XJdlwfjFhrR6ZoAw/Im8DCE11Z6+731uagHvHsFGWZ/HQ/oG7NG9c8evoTb5ILVaK+Vqwrkeu7HX2VL1VMiUZED6Tiiq1+rSR4dCoSqEy62ZWxybVG2rFHSOaU4shJDu4cA476ttwucts5Ia5Z3JGocjCBlG1DKl83LbnhEBaNFvNpAoGBANsQC9xnkrWwFYijBKUs1SXX3Q99QH6KSQW5FRiuwVDc43h878WpSIEBTS7qFRw33c3e8dkg9tsYmeEmWdOpUN5oFatSCaZSkh8w+HqA6aw68q4pkwQ8llUgyzl1DrfNpctFQY/POoKbTtehmO/WumJhv+i3TY3GTOYi4YaEJee/AoGBAMiSxzMA5TXs/mtfv8djUo99fWRNnX+uEENqh755VPIcuYWBbXa8UGLhcjcZFOqarQCNVA69VY9PFNQzYLkBggr28jhF6eIutjeQDDckIpvqywa3vLUtdzfTWPsrii825oNDUR9bGO+xOKhh3/tbWxRB4saDyOxtT0zq73/mrdLzAoGAN+veu0MNZqgutxS2aNwLBYAXhI662hK/FWDsC8MAwn3A688o/lJ6mcQVSfajsPJqAtX48y7BFakwDxPVNn0wkbYMYhGtOPI3LxM3Oz6RaFAcB23BhAFbdxvKBT7mpPEwc7WYSPfjvdebxtwPyJoONnMxpFy2xYxrsQwSel5dts0CgYA+nry8asIlFOnVwh4Q9Sx4ihhU8XqDu2dudNsOl7jyog815FO1p1N9m59aHmWOXV439ufQdkI5LNp26dd/yz27iJ/U+9bqe+T98eYubQS1Ixfh8Allk11OO5jjShOpa/2J68FvBbUCWJU01OHmCv6jk3Jmwgw/7Fy+yfaeOvn4CwKBgG50oT8HaR3kwfMl1i0w6tExVv4HctvHsYwEwEmTeLP4K5BGYP+hlrCZF6463+GAsrK/0lKA0FudqvdeJxDuhRDCprtug0W/KU/h4AueDalCTcuRSNrfkZFqzQz3Itj3T1JEY/u0WxwggatBEyV2jvgTZCIB9XZT+6PoXpCAInBR";
    //char* key = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQAB";
    PrivateKey p = asn1_parse_x509_private(key, strlen(key), &error);
    private_key_print(p);

    // Hoshoyo public key
    //char* key = "AAAAB3NzaC1yc2EAAAADAQABAAACAQDOL2ikG34/XVsFyKJJhOUECUAKNjKcMp/CeDfFOxoIXXFmTAOCYwRjW5DtDKMYxslwrfGSUFAtYRCNR0pEU7z3LgQ8NKjKp9kr5SKA6LhvH3yYpDfDoIbJ7cDu6Tvu9VOb8g2+AZP5JdAisrpKMkKsjYgaWCfrEcf6XSjUH8DSFfAgBYN0RfNHhOeQf3CeOzpL4F5vJpK3lyjwOVLaBk2nMm4xCCW4bisJZGMkcTBgmjYcOpu8rBVtuHWmtM+ckbHTeX8ASalZaSHBMejAKoepy9W/cEYrYoaHqmBb5qIOezlaR0nhutC5QIJWIC56ptI12tMqvMUKpfP4ybwHFGRaCpwMFvGLtBLjCPYMhHiok3rbBovqLa/aydKzZYPm4wmGgELL3qwVFvJDdFRv2Ja707wljeRonbTtKmnuyEwphr1G6zpbBLl9p34LoC433kqJTZl5IcFNq2CSrRMQn9W1jgkKHSsHZ6cX+1yQnJedcSlV0Tf8JM3a3g1dsjA0TssENAwPC8oD9ZQIdKE2Mi92PG/Ogl/+oq0lzRYoikyziakTU2LauN6ZE6DfbPAMUjWw+y/Q5HR1MmpnQ9hg5TdjZDm4cYGXTNITA8mQ9RhFPIgd7R1FUFj3qp065wBjjRUO1v44ND24plPRhPzBV+jNEWs/AdNUM9Im5ySJhg8AEQ==";
    //PublicKey pk = asn1_parse_public_key(key, strlen(key), 0);
    //public_key_print(pk);
    return 0;
}