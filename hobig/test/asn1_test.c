#include "../asn1.h"
#include <stdio.h>
#include <string.h>

void test_pem_cert() {
    unsigned int error = 0;

    char* key4 = "MIIFyTCCA7GgAwIBAgIJAJtJt/uDKPvhMA0GCSqGSIb3DQEBCwUAMHsxCzAJBgNVBAYTAkJSMQswCQYDVQQIDAJSUzEVMBMGA1UEBwwMUG9ydG8gQWxlZ3JlMRAwDgYDVQQKDAdIb3Nob3lvMRQwEgYDVQQDDAtzaGFwYXNzLmNvbTEgMB4GCSqGSIb3DQEJARYRaG9zaG95b0BnbWFpbC5jb20wHhcNMTkwNjIyMTgxMTQ3WhcNMjAwNjIxMTgxMTQ3WjB7MQswCQYDVQQGEwJCUjELMAkGA1UECAwCUlMxFTATBgNVBAcMDFBvcnRvIEFsZWdyZTEQMA4GA1UECgwHSG9zaG95bzEUMBIGA1UEAwwLc2hhcGFzcy5jb20xIDAeBgkqhkiG9w0BCQEWEWhvc2hveW9AZ21haWwuY29tMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA9MesQcmsqDgNFXX6/zsvC5haDB7ur47r7xEBl8b76mV8MxJb0W+GwRvJnQC0Q1g8PmJDc4mKbjlmYs2bvnrc4MXPaB1/Dy5rNhyPnQWfKVLvF2waYL/hNAY5upF9jjxKs3c2WzGRd6M/IoDM13KFSfVMIdGSlafz0BZvTNnFVcFOYIwqjEgfQC1vrPM1tbvxBpfMkjQj6MQhU8QNk/BqndhJMjI57Mb1cQ9PDr9L9B1IfYJY1m830NyWX5r488C3Rxq/3RpdipQni1pjXHGiYWJvweu52iC1Gzk8o1VB5O3mtWHSXC5OfDYyaDH21rxy8PrPKg5zDgNQzLhxKQtb12TaY8xrRb/IW1ZI3EsyKnig4dbm6/uMLkCmLp1nZXaVUGLox1CsxxrsjLK3YS0cWvn+zWJt0hBlXM5kiMTt14sXwmNxco0jiENLOwVOoVJjHk2p3pcjJZ2/6OWAVte51rQwsG15V2Gv3QHlRid8LK7tRGvc4CKV5nUKXSemsgRNTBLvcHDcpeNW6muyRn/ftBTS2cLoPizQryB6L+8gYHqkUV/M9ATFqQWjCqEKjklFWzTMoZMFJcZgYZyDyB2zI4W4seOB6sJ/lixl3ugKFb1R6KZ+Hmkn3bHxcs8xRqDXyLUnyrSqQ1UfnDVqMNPD42S3BsQ0ufuBRJLeBZMHfGkCAwEAAaNQME4wHQYDVR0OBBYEFF7ecg9mXeRR/cBI6tG+aR99F/DDMB8GA1UdIwQYMBaAFF7ecg9mXeRR/cBI6tG+aR99F/DDMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQELBQADggIBAJMwmQMkqLx37oa+ftRYAOc8U7/L5w09Gr8LH6iXYTYnPegJOjz6rT0EOrehEw5LRbEX12xk2yzPbUlNTu414iNu8kHgyD9k0p3enEDoO4rBk0HSvfxu3fJE/5S50iZuiYZvZrVbQtMPStlCXSAG3dEIfF+YTdCg26CX14YheTyL8BD7dTKcBXMGVfXak1A33oJdmbdmfdIzgJw1VcCsVjLkNkKFBhPSMI/kSozVtoyLQQWK9axLdwu9UkCOoa1GasiJd9y2HHpZTp8+oRHRZy0Kz8hPuuALjVNVnH1VxR9teA3ZRGIDy3mSbX7H5OQdK8fMexhVsuVuhz0zMdyAWASdGitX5s4VoAUB3hQ2skXxl6uZEbvfgSRY7z8aSKqbo91UmHbMAb9BcWuxlMbR6pCBVi3x+nCqlDppufJSE0yZCDfLIE3rYUzuCOZP4HSbaHxQvVDMJYro+MnX+vOXgS0IrAzy/W6fyU8lq48UAWl+MeLCgRIPIp0Y7OzEIvDZ+D2QhJRKEJIHnPHJtWC3ESpoPT6TsJTVxt/vIKwo/ys/WuqnsdBdIR9d976/OQeV+s8pt/HgVRTiuPvZ3/qEDv87gYB2Mr5WXnNT7erD3S2oBlf60V8hmk3bbA4JTnWgCjuc2B7rvCUr+QGIf3PaWf9OMKG/tiyKIGYvs8NQ2r8/";
    RSA_Certificate c = asn1_parse_pem_certificate(key4, strlen(key4), &error, 1);
    public_key_print(c.public_key);

}

void test_pem_private() {
    unsigned int error = 0;

    char* key2 = "MIIEowIBAAKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQABAoIBAQCKKq3UoI2Pq749MFjSdFjZHAMrF/AJennFPzy36dSA6qIahltKVEr45IOeG+h5S691fz0/jwRav/PTDEu0qfihtSVbL4NLggwhKG3GWUt4NshfsNouKNI8bPippHdIfW43drkla2ieZUp41o6eh+dGZe3EzkmQc7y3btTQF0XJdlwfjFhrR6ZoAw/Im8DCE11Z6+731uagHvHsFGWZ/HQ/oG7NG9c8evoTb5ILVaK+Vqwrkeu7HX2VL1VMiUZED6Tiiq1+rSR4dCoSqEy62ZWxybVG2rFHSOaU4shJDu4cA476ttwucts5Ia5Z3JGocjCBlG1DKl83LbnhEBaNFvNpAoGBANsQC9xnkrWwFYijBKUs1SXX3Q99QH6KSQW5FRiuwVDc43h878WpSIEBTS7qFRw33c3e8dkg9tsYmeEmWdOpUN5oFatSCaZSkh8w+HqA6aw68q4pkwQ8llUgyzl1DrfNpctFQY/POoKbTtehmO/WumJhv+i3TY3GTOYi4YaEJee/AoGBAMiSxzMA5TXs/mtfv8djUo99fWRNnX+uEENqh755VPIcuYWBbXa8UGLhcjcZFOqarQCNVA69VY9PFNQzYLkBggr28jhF6eIutjeQDDckIpvqywa3vLUtdzfTWPsrii825oNDUR9bGO+xOKhh3/tbWxRB4saDyOxtT0zq73/mrdLzAoGAN+veu0MNZqgutxS2aNwLBYAXhI662hK/FWDsC8MAwn3A688o/lJ6mcQVSfajsPJqAtX48y7BFakwDxPVNn0wkbYMYhGtOPI3LxM3Oz6RaFAcB23BhAFbdxvKBT7mpPEwc7WYSPfjvdebxtwPyJoONnMxpFy2xYxrsQwSel5dts0CgYA+nry8asIlFOnVwh4Q9Sx4ihhU8XqDu2dudNsOl7jyog815FO1p1N9m59aHmWOXV439ufQdkI5LNp26dd/yz27iJ/U+9bqe+T98eYubQS1Ixfh8Allk11OO5jjShOpa/2J68FvBbUCWJU01OHmCv6jk3Jmwgw/7Fy+yfaeOvn4CwKBgG50oT8HaR3kwfMl1i0w6tExVv4HctvHsYwEwEmTeLP4K5BGYP+hlrCZF6463+GAsrK/0lKA0FudqvdeJxDuhRDCprtug0W/KU/h4AueDalCTcuRSNrfkZFqzQz3Itj3T1JEY/u0WxwggatBEyV2jvgTZCIB9XZT+6PoXpCAInBR";
    PrivateKey p = asn1_parse_pem_private(key2, strlen(key2), &error, 1);
    if(error != 0) {
        return;
    } else {
        private_key_print(p);
    }
    
    private_key_free(p);
}

void test_key_public() {
    unsigned int error = 0;
    char* key1 = "AAAAB3NzaC1yc2EAAAADAQABAAACAQDOL2ikG34/XVsFyKJJhOUECUAKNjKcMp/CeDfFOxoIXXFmTAOCYwRjW5DtDKMYxslwrfGSUFAtYRCNR0pEU7z3LgQ8NKjKp9kr5SKA6LhvH3yYpDfDoIbJ7cDu6Tvu9VOb8g2+AZP5JdAisrpKMkKsjYgaWCfrEcf6XSjUH8DSFfAgBYN0RfNHhOeQf3CeOzpL4F5vJpK3lyjwOVLaBk2nMm4xCCW4bisJZGMkcTBgmjYcOpu8rBVtuHWmtM+ckbHTeX8ASalZaSHBMejAKoepy9W/cEYrYoaHqmBb5qIOezlaR0nhutC5QIJWIC56ptI12tMqvMUKpfP4ybwHFGRaCpwMFvGLtBLjCPYMhHiok3rbBovqLa/aydKzZYPm4wmGgELL3qwVFvJDdFRv2Ja707wljeRonbTtKmnuyEwphr1G6zpbBLl9p34LoC433kqJTZl5IcFNq2CSrRMQn9W1jgkKHSsHZ6cX+1yQnJedcSlV0Tf8JM3a3g1dsjA0TssENAwPC8oD9ZQIdKE2Mi92PG/Ogl/+oq0lzRYoikyziakTU2LauN6ZE6DfbPAMUjWw+y/Q5HR1MmpnQ9hg5TdjZDm4cYGXTNITA8mQ9RhFPIgd7R1FUFj3qp065wBjjRUO1v44ND24plPRhPzBV+jNEWs/AdNUM9Im5ySJhg8AEQ==";
    PublicKey pk1 = asn1_parse_public_key(key1, strlen(key1), &error, 1);
    if(error != 0) {
        return;
    } else {
        public_key_print(pk1);
    }
    public_key_free(pk1);
}

void test_pem_public() {
    unsigned int error = 0;
    char* key3 = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAq6Ii4BLMhIiX0x5gL+N79w5znglZ4cqyvVfnqBGHWGBc4SapnzQSbN1dzv0wP3M1QV/J9V1tNyc6qPWPuK29WL0U5VN2cOQ2thAL32NCOPBKfFyhcUIylj6A5BXIJPpzzPowmg4P8aB9m8Fr6WZ7Bx6PlZ4SX+N+SHjw52seqSPz9hfMYD+4D4XTtkXNzaKJraInpfqfx27Ar7sCwvlcuNP7I2GJajLCvwhdktwlmwdPbbW1xxw3QT8izKKQOcYExzVigiNkgiPBN+mMA/iX+aPZRoMDorgwAEnihl8T7QqtJVUBbbrlma0fZDaQwLt5+fuN7pp2hXbNfQ+qviioTQIDAQAB";
    PublicKey pk2 = asn1_parse_pem_public(key3, strlen(key3), &error, 1);
    public_key_print(pk2);

    public_key_free(pk2);
}

int main(int argc, char** argv) {
    unsigned int error = 0;

    //test_pem_public();
    //test_pem_private();
    //test_pem_cert();

    //RSA_Certificate cert = asn1_parse_pem_certificate_from_file("/home/hoshoyo/Downloads/sha.jpg", &error);
#if 0
    PublicKey key0 = asn1_parse_public_key_from_file("/home/hoshoyo/.ssh/id_rsa.pub", &error);
    PublicKey key1 = asn1_parse_pem_public_key_from_file("/home/hoshoyo/test/public.pem", &error);
#endif

    return 0;
}