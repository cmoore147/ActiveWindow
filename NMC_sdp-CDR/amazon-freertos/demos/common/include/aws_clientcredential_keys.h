#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUUb5t+7x+b2DLS365Ko7p+2variQwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMTAyOTA0MDI1\n"\
"NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMBywduxAtt8oe/b0iw/\n"\
"EoqYO4+PG+kNRfQM66YQtduLFYgfYN6glKd+eVqlCFeZ8N5A8Gr8JgmPCyokY7AN\n"\
"RI1NCzo8QWcO6e6qdJj8ZnUCbkyJS9IgMcZF7AEc/5elB4Gjzwj5iohOa0Q6SOcs\n"\
"PTo7rXz66Ol7Aw1iqPGVrRzEKP7Wsj1rE7DMbiGa9lUzVJwdhD4KTgxEbydi/HWj\n"\
"SpS/vrYMuCANR6TJ9uq07UsUMvGpl0lMiEnAppMfgifALJVD7eFVsZqLYlKvZoAs\n"\
"8RueV1nOgh/N4n/sJ2L87fmcG3x/DtYhpKVdLNwFVQgh2Cl3tiB5BUpHV9IT3vMw\n"\
"7Y8CAwEAAaNgMF4wHwYDVR0jBBgwFoAURJPeo7vYWpmmJzTxE/WfGqM2u1gwHQYD\n"\
"VR0OBBYEFKRObeSeBGLzwi9//fBewBJh0TwXMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBbYeyU6sg85BBOEb9cbX8F2joQ\n"\
"XRhDlKmpd1pV7wIrlLF0iKfAf4wyweF1spihZ5ql9yazT+6dHAHiBBcAbTDl277x\n"\
"i8X/nm0rcXjWYemRD+0afkdFszTUns7URkLBjIlrKndXIiXcuWKma2ifCzMDFLyT\n"\
"lm+A78lGxN2n5HwmZ9sQhVrZ02joMFrwee0SowTEeIQsqb5BnxksiHFX6qsGZcXX\n"\
"w4Go9TMo5qNNdpPUVHFFp7cL5I9h/27QYXYX0iWMoR+5Juj5ffdyge2zA7nkvHsA\n"\
"4mva9WCyGkrSycBcMlMishZF07Ks3Ko5oMl+n/jyW5guTOdHuWMTp51F4Pfc\n"\
"-----END CERTIFICATE-----"

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpQIBAAKCAQEAwHLB27EC23yh79vSLD8Sipg7j48b6Q1F9AzrphC124sViB9g\n"\
"3qCUp355WqUIV5nw3kDwavwmCY8LKiRjsA1EjU0LOjxBZw7p7qp0mPxmdQJuTIlL\n"\
"0iAxxkXsARz/l6UHgaPPCPmKiE5rRDpI5yw9OjutfPro6XsDDWKo8ZWtHMQo/tay\n"\
"PWsTsMxuIZr2VTNUnB2EPgpODERvJ2L8daNKlL++tgy4IA1HpMn26rTtSxQy8amX\n"\
"SUyIScCmkx+CJ8AslUPt4VWxmotiUq9mgCzxG55XWc6CH83if+wnYvzt+ZwbfH8O\n"\
"1iGkpV0s3AVVCCHYKXe2IHkFSkdX0hPe8zDtjwIDAQABAoIBAQCqGsVPsr6b0yYw\n"\
"JVU41udEqhq7YfuBm8HhXipMtyiHzgBSOaoeHkJHantJq+YbyG4nha78YbQdbtqn\n"\
"fVLAInodi6QI7YnB1mTgGLrHHxJRCr0D2bezG9HjN5rLwR4KpExmQqyVej0BDo+x\n"\
"1vLpoyjbeDR47C9DcP/ZrQM3xt0lNpW12BwHEdIghdaIfli54JdB0T3MPaOaB900\n"\
"X0qEAOzuFZMhxj8cvo0BMRf64uyAyB2ddYaAPPxL2imU7gqiw3tIs84Os3Hw/HlX\n"\
"WCUJaEA8xFNyTYb/xMVN6P9J6Um0Tg5QkYO5StJLMzNj+hWUFe9UY4wu72m5M/HM\n"\
"nUCTQNYBAoGBAOGHoFwHFgeiOgBFMiOTralrtHDSAfF5j9rzlU3PzRjTWDD5+o/o\n"\
"H8qib95ZP2afPBOSr2itnKCsTNrXD/mvldS6i6kQfqfOWEU6GvbWwdByiNbYRXYp\n"\
"jEJOVvyBcchH2hfdlIEIDRATPXfwFnOYwDaMELhT6KZHl92efHRObAwvAoGBANpy\n"\
"8auqw2Hi+vnM5xprASfxjGV9hI/NhP7Y7FbZQwvBCjaN0k2uRMUrFqKJxj4SmTwE\n"\
"xlXTbONwhh7kS6MfeE7SD+HQF0YpwQPoLF6LIcKho7IdMhrbpz8reVIcgwj/U3w4\n"\
"WrbEpbGR7Dy4igsgNS0Kq2Y5eXVsH0ERDfjD8fyhAoGBALXqT2hL8vIfYgebyRC3\n"\
"a0oQbZDfZN/hvS7Qcis8peQTfbyrtvdjNlmd4vyIIsLlSHFFTaXy/DcCJFAVT3aI\n"\
"siPQgRzTCEbVA5B7iQp4Ogeoi0Y5VvY0F0XYqQOuahgyFPJRMhtOYjMIQfm/uGxv\n"\
"1wNowYfdbvjJIO+LijnKlA7TAoGBAI2XeXvK2I+nHwQxQw6Fkx8/2r+h5v2Kugyx\n"\
"sCbu9h01R5hqvoipT1v5fNdns/5h8b7MXX4d53NYaSa21SvrZwC3+aFlTuCkib4t\n"\
"jCzPw9czDDaT4/kyrKouVJnbbzeyv3tBz9KtjbSWH+iPuLJTcwQ56TT+RfLNXUjA\n"\
"dCztXPlhAoGAekd8d3Qw8RWTiyVt616CfDIUz15ufzy2zj1ZHnjRWRspnpAUPXkZ\n"\
"kSUiyCUn9OPb4wXCLH38DzZWtKhraUa5T47hYpPlirEPEmcXUFCbfzHsbHfgwwNg\n"\
"Fx33m1C7Mo71m6N2cQAJCBMMHHdWwVLK1y0FacBIjGPrCqiBdjhaaJw=\n"\
"-----END RSA PRIVATE KEY-----"

/*
 * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
 *
 * If used, must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
