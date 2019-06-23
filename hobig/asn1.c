#include "asn1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define LIGHT_ARENA_IMPLEMENT
#include "light_arena.h"
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
    int length;
    struct DER_Node_t* data;
} DER_Oct_String;

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
        DER_Oct_String oct_string;
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
parse_der(Light_Arena* arena, u8* data, int length, unsigned int* error) {
    u8* at = data;
    int advance = 0;
    switch(*at) {
        case DER_SEQUENCE: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_SEQUENCE;
            node->length = length + advance;
            node->sequence.length = length;
            node->sequence.data = parse_der(arena, at, length, error);
            if(error && *error) return 0;

            length -= node->sequence.data->length;
            at += node->sequence.data->length;

            DER_Node* n = node;
            while(length) {
                n->next = parse_der(arena, at, length, error);
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
            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_BIT_STRING;
            node->length = length + advance;
            node->bit_string.length = length;
            node->bit_string.unused = unused;
            node->bit_string.data = parse_der(arena, at, length, error);
            at += length;
            return node;
        } break;
        case DER_OCT_STRING: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_OCT_STRING;
            node->length = length + advance;
            node->oct_string.length = length;
            node->oct_string.data = parse_der(arena, at, length, error);
            at += length;
            return node;
        } break;
        case DER_OBJECT_ID: {
            int length = der_get_length(at, error, &advance);
            if(error && *error) return 0;
            at += advance;

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
            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_OBJECT_ID;
            node->length = length + advance;
            node->object_id.length = k;
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
            HoBigInt b = hobig_int_new_from_memory(at, length);
            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
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
            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_NULL;
            node->length = 2;
            return node;
        } break;
        default: {
            int length = der_get_length(at, error, &advance);
            at += advance;
            DER_Node* node = arena_alloc(arena, sizeof(DER_Node));
            node->kind = DER_NULL;
            node->length = advance + length;
            return node;
        } break;
    }
    return 0;
}

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
asn1_parse_pem_private(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        free(r.data);
        if(error) *error |= 1;
        return (PrivateKey){0};
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);
    PrivateKey key = {0};

    if(node->kind != DER_SEQUENCE) {
        if(error) *error |= 1;
        fprintf(stderr, "Could not parse private key, expected DER sequence\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }

    DER_Node* rsa_indication = node->sequence.data;
    if(rsa_indication->kind != DER_INTEGER) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid RSA version, expected integer entry\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    if(rsa_indication->integer.i.value != 0) {
        // must be 0 indicating 2 primes
        if(error) *error |= 1;
        fprintf(stderr, "Invalid RSA version, expected 0 value to indicate RSA key of 2 primes\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    DER_Node* next = node->next;
    if(next->kind == DER_INTEGER) {
        key.public.N = next->integer.i;
    } else {
        // error expected integer public modulus
        if(error) *error |= 1;
        fprintf(stderr, "Could not find public modulus in the file\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.public.E = next->integer.i;
    } else {
        // error expected integer public exponent
        if(error) *error |= 1;
        fprintf(stderr, "Could not find public exponent in the file\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.PrivateExponent = next->integer.i;
    } else {
        // error expected private exponent
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private exponent in the file\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.P = next->integer.i;
    } else {
        // error expected private P
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private prime P in the file\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    next = next->next;
    if(next->kind == DER_INTEGER) {
        key.Q = next->integer.i;
    } else {
        // error expected private Q
        if(error) *error |= 1;
        fprintf(stderr, "Could not find private prime Q in the file\n");
        free(r.data);
        arena_free(arena);
        private_key_free(key);
        return (PrivateKey){0};
    }
    next = next->next;
    while(next) {
        if(next->kind == DER_INTEGER) {
            hobig_free(next->integer.i);
        }
        next = next->next;
    }

    free(r.data);
    arena_free(arena);
    return key;
}

// Parses PEM file of a public key
// Reference: https://medium.com/@bn121rajesh/understanding-rsa-public-key-70d900b1033c
PublicKey
asn1_parse_pem_public(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        free(r.data);
        if(error) *error |= 1;
        return (PublicKey){0};
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);
    PublicKey pk = {0};

    if(node->kind != DER_SEQUENCE) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected sequence\n");
        free(r.data);
        arena_free(arena);
        return (PublicKey){0};
    }

    // TODO(psv): check if the correct key format is provided
    // Ignore node->sequence.data for now,
    // it is the information about which key it is

    if(!node->next || node->next->kind != DER_BIT_STRING) {
        // error
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected bit string\n");
        free(r.data);
        arena_free(arena);
        return (PublicKey){0};
    }

    if(node->next->bit_string.data->kind != DER_SEQUENCE) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected sequence of two integers\n");
        free(r.data);
        arena_free(arena);
        return (PublicKey){0};
    }

    DER_Node* modulus = node->next->bit_string.data->sequence.data;
    if(modulus->kind != DER_INTEGER) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected public modulus integer\n");
        free(r.data);
        arena_free(arena);
        return (PublicKey){0};
    }

    DER_Node* exponent = node->next->bit_string.data->next;
    if(exponent->kind != DER_INTEGER) {
        if(error) *error |= 1;
        fprintf(stderr, "Invalid format, expected public exponent integer\n");
        free(r.data);
        arena_free(arena);
        return (PublicKey){0};
    }

    pk.E = exponent->integer.i;
    pk.N = modulus->integer.i;

    free(r.data);
    arena_free(arena);
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

void
asn1_parse_pem_certificate(const u8* data, int length, u32* error) {
    Base64_Data r = base64_decode(data, length);
    if(r.error != 0) {
        fprintf(stderr, "Could not parse key base64 data\n");
        free(r.data);
        if(error) *error |= 1;
    }

    Light_Arena* arena = arena_create(65535);
    DER_Node* node = parse_der(arena, r.data, r.length, error);
    PublicKey pk = {0};


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
    PublicKey pubk = asn1_parse_public_key(start_data, at - start_data, error);

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

    PublicKey result = asn1_parse_pem_public(d, trimmed_length, error);

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

    PrivateKey result = asn1_parse_pem_private(d, trimmed_length, error);

    return result;
}