#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../light_array.h"
#include "hobig.h"
#include "asn1.h"

typedef unsigned long long int u64;
extern u64 random_integer(u64 min, u64 max);

typedef struct {
    char* data;
    int   length;
} Decrypt_Data;

#define BIG_ENDIAN_64(X) ((((X) & 0xff00000000000000) >> 56) | \
    (((X) & 0xff000000000000) >> 40) | \
    (((X) & 0xff0000000000) >> 24) | \
    (((X) & 0xff00000000) >> 8) | \
    (((X) & 0xff000000) << 8) | \
    (((X) & 0xff0000) << 24) | \
    (((X) & 0xff00) << 40) | \
    (((X) & 0xff) << 56))

Decrypt_Data
decrypt_pkcs1_v1_5(PrivateKey pk, HoBigInt encrypted, unsigned int* error) {
    HoBigInt decr = hobig_int_mod_div(&encrypted, &pk.PrivateExponent, &pk.public.N);
    if(array_length(decr.value) != 32) {
        // error, encrypted message does not contain 2048 bits
        fprintf(stderr, "Encrypted message must contain 2048 bits\n");
        if(error) *error |= 1;
        return (Decrypt_Data){0};
    }

    if(((decr.value[31] & 0xffff000000000000) >> 48) != 0x0002) {
        // format invalid, but do not accuse error to not be
        // vulnerable to attacks like the one described by
        // https://www.youtube.com/watch?v=y9n5FQlKA6g
    }

    int index = 0;
    for(int i = 31; i >= 0; --i) {
        // sweep every byte searching for 0xff
        u64 v = decr.value[i];
        for(int k = 56; k >= 0; k -= 8) {
            if(((v >> k) & 0xff) == 0xff) {
                index = (i * 64) + k;
                goto end_loop;
            }
        }
    }
end_loop:
    index /= 8; // index in bytes

    Decrypt_Data result = {0};
    // index has the bit count from the base
    result.data = calloc(1, index);
    result.length = index;

    // Copy into memory from little endian to big endian
    for(int i = 0; i < index; ++i) {
        char b = ((char*)decr.value)[i];
        result.data[index - i - 1] = b;
    }
    
    return result;
}

HoBigInt
encrypt_pkcs1_v1_5(PublicKey pk, const char* in, int length_bytes) {
    char out[256];

    // Cannot encrypt something bigger than 128 bits or 16 bytes
    if(length_bytes > 32) return (HoBigInt){0};

    // Always mode 2, meaning encryption
    // First 16 bits are 0x0002 for mode 2
    out[0] = 0;
    out[1] = 2;

    // Generate random padding not containing a byte 0xff
    int padding_byte_count = 256 - length_bytes - 2 - 1;
    for(int i = 0; i < padding_byte_count; ++i) {
        out[i + 2] = 66;//random_integer(0, 0xff);
    }
    out[padding_byte_count + 2] = 0x00;

    memcpy(out+padding_byte_count + 3, in, length_bytes);

    HoBigInt rsa_plain_text = hobig_int_new_from_memory((const char*)out, 256);
    hobig_int_print(rsa_plain_text);
    printf("\n");
    hobig_int_print(pk.E);
    printf("\n");
    hobig_int_print(pk.N);
    printf("\n");

    HoBigInt encrypted = hobig_int_mod_div(&rsa_plain_text, &pk.E, &pk.N);

    hobig_int_print(encrypted);
    printf("\n");

    return encrypted;
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

void foo_print_bytes(char* b, int length) {
    for(int i = 0; i < length; ++i) {
        printf("%d ", (unsigned char)b[i]);
    }
}

int main() {
    int error = 0;

    // Public Key
    char* key = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQAB";
    PublicKey public = asn1_parse_pem_public(key, strlen(key), &error);

    // Private Key
    char* priv = "MIIEowIBAAKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQABAoIBAQCKKq3UoI2Pq749MFjSdFjZHAMrF/AJennFPzy36dSA6qIahltKVEr45IOeG+h5S691fz0/jwRav/PTDEu0qfihtSVbL4NLggwhKG3GWUt4NshfsNouKNI8bPippHdIfW43drkla2ieZUp41o6eh+dGZe3EzkmQc7y3btTQF0XJdlwfjFhrR6ZoAw/Im8DCE11Z6+731uagHvHsFGWZ/HQ/oG7NG9c8evoTb5ILVaK+Vqwrkeu7HX2VL1VMiUZED6Tiiq1+rSR4dCoSqEy62ZWxybVG2rFHSOaU4shJDu4cA476ttwucts5Ia5Z3JGocjCBlG1DKl83LbnhEBaNFvNpAoGBANsQC9xnkrWwFYijBKUs1SXX3Q99QH6KSQW5FRiuwVDc43h878WpSIEBTS7qFRw33c3e8dkg9tsYmeEmWdOpUN5oFatSCaZSkh8w+HqA6aw68q4pkwQ8llUgyzl1DrfNpctFQY/POoKbTtehmO/WumJhv+i3TY3GTOYi4YaEJee/AoGBAMiSxzMA5TXs/mtfv8djUo99fWRNnX+uEENqh755VPIcuYWBbXa8UGLhcjcZFOqarQCNVA69VY9PFNQzYLkBggr28jhF6eIutjeQDDckIpvqywa3vLUtdzfTWPsrii825oNDUR9bGO+xOKhh3/tbWxRB4saDyOxtT0zq73/mrdLzAoGAN+veu0MNZqgutxS2aNwLBYAXhI662hK/FWDsC8MAwn3A688o/lJ6mcQVSfajsPJqAtX48y7BFakwDxPVNn0wkbYMYhGtOPI3LxM3Oz6RaFAcB23BhAFbdxvKBT7mpPEwc7WYSPfjvdebxtwPyJoONnMxpFy2xYxrsQwSel5dts0CgYA+nry8asIlFOnVwh4Q9Sx4ihhU8XqDu2dudNsOl7jyog815FO1p1N9m59aHmWOXV439ufQdkI5LNp26dd/yz27iJ/U+9bqe+T98eYubQS1Ixfh8Allk11OO5jjShOpa/2J68FvBbUCWJU01OHmCv6jk3Jmwgw/7Fy+yfaeOvn4CwKBgG50oT8HaR3kwfMl1i0w6tExVv4HctvHsYwEwEmTeLP4K5BGYP+hlrCZF6463+GAsrK/0lKA0FudqvdeJxDuhRDCprtug0W/KU/h4AueDalCTcuRSNrfkZFqzQz3Itj3T1JEY/u0WxwggatBEyV2jvgTZCIB9XZT+6PoXpCAInBR";
    PrivateKey private = asn1_parse_pem_private(priv, strlen(priv), &error);

    char* msg = "Hello";
    int msg_length = strlen(msg);
    HoBigInt encr = encrypt_pkcs1_v1_5(public, msg, msg_length);

    //foo_print_bytes((char*)encr.value, 256);
    printf("\n");
#if 0 
    int file_size = 0;
    void* file_data = load_entire_file("/home/hoshoyo/test/msg.encrypt", &file_size);
    HoBigInt encr = hobig_int_new_from_memory(file_data, file_size);
#endif


    //Decrypt_Data data = decrypt_pkcs1_v1_5(private, encr, &error);

    //printf("%.*s", data.length, data.data);

    return 0;
}