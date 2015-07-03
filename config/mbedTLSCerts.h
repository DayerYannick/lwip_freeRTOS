/*
 * certs.h
 *
 *  Created on: 2 juil. 2015
 *      Author: yannick.dayer
 *
 *	This file contains the certificates for mbedTLS in PEM format
 *
 */

#ifndef MBEDTLSCERTS_H_
#define MBEDTLSCERTS_H_

#include "heivs/config.h"
#if USE_MBEDTLS

#if !defined(POLARSSL_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif


// If certificates are enabled
#if defined(POLARSSL_CERTS_C)




// If RSA is enabled
#if defined(POLARSSL_RSA_C)

// Self-signed CA certificate
#define CA_CRT_RSA                                                 \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIC3jCCAkegAwIBAgIJALIdw9gyJISFMA0GCSqGSIb3DQEBBQUAMIGHMQswCQYD\r\n" \
"VQQGEwJDSDEPMA0GA1UECAwGVmFsYWlzMQ0wCwYDVQQHDARTaW9uMRIwEAYDVQQK\r\n" \
"DAlIZXZzIGNlcnQxFTATBgNVBAMMDGNlcnQuaGV2cy5jaDEtMCsGCSqGSIb3DQEJ\r\n" \
"ARYeeWFubmljay5kYXllckBzdHVkZW50cy5oZXZzLmNoMB4XDTE1MDcwMjA3Mjg1\r\n" \
"M1oXDTE4MDcwMTA3Mjg1M1owgYcxCzAJBgNVBAYTAkNIMQ8wDQYDVQQIDAZWYWxh\r\n" \
"aXMxDTALBgNVBAcMBFNpb24xEjAQBgNVBAoMCUhldnMgY2VydDEVMBMGA1UEAwwM\r\n" \
"Y2VydC5oZXZzLmNoMS0wKwYJKoZIhvcNAQkBFh55YW5uaWNrLmRheWVyQHN0dWRl\r\n" \
"bnRzLmhldnMuY2gwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAKCHiVWvEPN3\r\n" \
"00aer4aEofHd/8LJE5vm/BDzFy+MoaNRhUutvlh0EL/mYBBaKN1F4aKiXYezuHOm\r\n" \
"i1iC0o44KC+s1p6Dane+jlu3NoBusrgLQ/l2ahDiGMhTqDfz57hZVop8Kfg6KWAq\r\n" \
"x1vywZd2/PK2WGdhNBwgQO8S17yjl+cpAgMBAAGjUDBOMB0GA1UdDgQWBBS+Rv+5\r\n" \
"0Bd3rzJU2SzyLRkXEG+tLjAfBgNVHSMEGDAWgBS+Rv+50Bd3rzJU2SzyLRkXEG+t\r\n" \
"LjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4GBAGzrf/0mTUYvrG4gwt6D\r\n" \
"7R4gUj3xaInGEXa843XfE7a7oxplxu7K/ws3xRVPoi6oYuVtPYd+WeX0/H6VbVLR\r\n" \
"ou4zZfoxLICmL4x2YTAQNLGzry8pupHgRUpZBYtq/ioehweT4LZywGYLGeQOtgl2\r\n" \
"6PR2MkramyGv5IkwTd37UIm1\r\n" \
"-----END CERTIFICATE-----\r\n"
const char ca_crt_rsa[] = CA_CRT_RSA;

// The CA
const char ca_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"Proc-Type: 4,ENCRYPTED\r\n"
"DEK-Info: AES-256-CBC,DFD45E493FECADDA3CC901D8CD14D8B3\r\n"
"\r\n"
"aHBAnbYN6OjgHn2wak/4wh7gZQLq1RSY73f3sr/arIUI05m3qpnwCicNlqVfnNAQ\r\n"
"L6/akhH7l+Yj6JGUQ51R7Nc1I4Rcg/8oR3SJ9AS62dhoUZjC14wATPbJQja+qrhr\r\n"
"pb83syomCH6AknX8sUUwJXIh4ObuHuYtYloGLVDTdVShm8ZK64/y72iZA5Ocgiit\r\n"
"TeFi37zxQsSAdvhDBJNoKvw3GnIekeujJDU47VsGYbEXXBeWSq4VOgrfuDuFu1Yx\r\n"
"Vs7u2gqzY6f4z/Wm+MbkxeWbIqKdRpW5u97uaIusCCcOe433MQymyH6ykQVQoFes\r\n"
"bsYyp8unlCpddWGZLPhkQYY1BA9ncsCWifSn2kgR4MxZWzLtoV99rrI+MbPoup47\r\n"
"zqnSb1WbwgoLaQMfzUmDaZq7DYHP9rNb6zSpkrFZQ/7Vr7khE1aQdghs4g8z5ww4\r\n"
"YtKPt3vuKq2AymxcQAIaqbqG9lIrl31pmvYTbJpJBQ5msqjuzF7NobBfk/aKFLvR\r\n"
"ktKm9Em/7XGhKA5uHh1uHhLNcDIFmuBhEn6yDJ33n8233m/T9had+zY9v1Oxm7q4\r\n"
"jRwCzardsYocljx6kUS3ARmqCcSV6MQV64/kAGS/IGVibHoEorbOLfd3ojY0V5OQ\r\n"
"E/vfDifsHqSX+wyDppAUEOaC0kvn0RNC2Ivc2I5a0v92zJyKGTExfJiV2bAN+Z7c\r\n"
"yY5FjqAP9vafE8NXTaUCfhs7q55M++C6glmZR4DkUAm+TOgKCPJXkqz7kYI764dD\r\n"
"hESP91zu9wEtleymMCwLvsLEJMFSaKwWbeAn3MSSZcRXWiyJH2coZ53NIYafgcyu\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

const char ca_pwd_rsa[] = "TestKey";

// The server certificate (sent to the client to identify us if required)
// Create it e.g. with 'openssl req' and send it to a trusted CA to certify
const char srv_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDbDCCAtWgAwIBAgIBAzANBgkqhkiG9w0BAQUFADCBhzELMAkGA1UEBhMCQ0gx\r\n"
"DzANBgNVBAgMBlZhbGFpczENMAsGA1UEBwwEU2lvbjESMBAGA1UECgwJSGV2cyBj\r\n"
"ZXJ0MRUwEwYDVQQDDAxjZXJ0LmhldnMuY2gxLTArBgkqhkiG9w0BCQEWHnlhbm5p\r\n"
"Y2suZGF5ZXJAc3R1ZGVudHMuaGV2cy5jaDAeFw0xNTA3MDIwOTAxNTZaFw0xODA3\r\n"
"MDEwOTAxNTZaMIGPMQswCQYDVQQGEwJDSDEPMA0GA1UECAwGVmFsYWlzMQ0wCwYD\r\n"
"VQQHDARTaW9uMRQwEgYDVQQKDAtIZXZzIChzZXJ2KTEbMBkGA1UEAwwSbHdpcF9k\r\n"
"YXllci5oZXZzLmNoMS0wKwYJKoZIhvcNAQkBFh55YW5uaWNrLmRheWVyQHN0dWRl\r\n"
"bnRzLmhldnMuY2gwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHlFdd\r\n"
"P129DXAMmk4YsGDvPWHO88cJv0IUG9woLZ4bp9ln9g7/EGnFVHMqdmqPpKFvuaeh\r\n"
"YJEij73+I1gLr53IQYNqoQoBEngdP4t9g0tN2dUYNDG87gI49Vgn4aOZCucN4c+M\r\n"
"Gu53CSJNLRwFUBER/NlQTXo1qM4ubd2Iq7g5DKs93LgibFYgwQXsZ7I4bFuEzuGP\r\n"
"XigRd6dA+Xni4BBie4VtZFPMEz+0bQqc+Ev1ro8E9PxkhqgZzXCnZE9Q975M6zLe\r\n"
"udKSMdi+c2olo15yGuDA4ND0lZmizN7msWz2lONaEH83qKwWUg6xuZmt7r3n8lyv\r\n"
"GeBFYqy/fJC1MRmTAgMBAAGjWjBYMB0GA1UdDgQWBBSlUuC6NzAvEDK7FjkBXz38\r\n"
"4sgdWTAfBgNVHSMEGDAWgBS+Rv+50Bd3rzJU2SzyLRkXEG+tLjAJBgNVHRMEAjAA\r\n"
"MAsGA1UdDwQEAwIFoDANBgkqhkiG9w0BAQUFAAOBgQBfaMHHHFMsultzp0zyG1Dx\r\n"
"iNHp/Ti0BoLCgU9ZpTh8u04qx0X2F3eYDdCAeGE9YhGzjsOKHxUL+gXXrnBi3Yyn\r\n"
"zlsGM48GO1V3xG59XtD34yAigG13CN4wilz2m7HSliG7EwBO+bfDwuF3fZ70eorX\r\n"
"turQ8FK/kSezCXb80M4YWg==\r\n"
"-----END CERTIFICATE-----\r\n";

// The client private key in PEM format
// Create it e.g. with 'openssl genrsa'
const char srv_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEowIBAAKCAQEAx5RXXT9dvQ1wDJpOGLBg7z1hzvPHCb9CFBvcKC2eG6fZZ/YO\r\n"
"/xBpxVRzKnZqj6Shb7mnoWCRIo+9/iNYC6+dyEGDaqEKARJ4HT+LfYNLTdnVGDQx\r\n"
"vO4COPVYJ+GjmQrnDeHPjBrudwkiTS0cBVAREfzZUE16NajOLm3diKu4OQyrPdy4\r\n"
"ImxWIMEF7GeyOGxbhM7hj14oEXenQPl54uAQYnuFbWRTzBM/tG0KnPhL9a6PBPT8\r\n"
"ZIaoGc1wp2RPUPe+TOsy3rnSkjHYvnNqJaNechrgwODQ9JWZosze5rFs9pTjWhB/\r\n"
"N6isFlIOsbmZre695/JcrxngRWKsv3yQtTEZkwIDAQABAoIBAGjzMengK4wB5/1I\r\n"
"m27SOXc1QRzSBsWyZK4sZF5gdrc2UdYgh4xLdkOFgDZBJRI6UYZuC5+KJ2gUTKW6\r\n"
"sk8a347yHIhS/O7JZo6C3vh2tX/P4AYlVE50noiTQ/Nc7WnKWOxs5XZYpzPDgt0w\r\n"
"rn6AtylNXH2k8HINPbceoROAsZSVjc0JKFCO7RFsb9W9/GJRHIIEuXg80pxQ/oIi\r\n"
"Y+RPc1q+pcjzafpsVNSDbToKAANf662uaBZqombneh+u2aMNC60IQ2vE9TajZzeH\r\n"
"NcVzHKRw16ediFoqCWV1NVq0dHGNZ9+ExLbJJnXI9IWClFYUcbN9gEx45vXNKosQ\r\n"
"Slk6uJkCgYEA71lL2OqRt6K7p1r5MlsY27E/k9u473HZaB4zefvAm1jUE3y2Cqsx\r\n"
"vL3dTL41bcjvnRIdYVxacqSrO0mCrEUAOHbXWWA8L49T0c8kq4cjA5iDdYjfc0Ro\r\n"
"sCFQ2xH/an5T/gKA1WujM1mPP6U7CwczU5FJNEwxX76JFGbAEl/g158CgYEA1XbE\r\n"
"537Qd6trMaueGUr+RreNs+wZFLEm2oL/9GYP0rx9RVvW38ofvl44jP0gUKBngbfR\r\n"
"ECHcljEVkqJ9LL6813MtnWY2YKwlLvVwrp0gxlriVQMm4ua/eZK6dVIE1WcHS0QF\r\n"
"JZeX/+UbsWn+OlIiIKStoZI1uRZzvH2mBwJiSY0CgYBAiVxZLc6CrF9a3b3JpSSL\r\n"
"wDw7Wwh/j7wmRdZtcsNZX9c3Ne5D7CsqECvGkJcJdh+dw4+me+dreBkvkD+qwaQw\r\n"
"lWK1l6xCyqu4K9lgYry/MHVp0LEnfG+Z5jHOEAlFprBxEi243iAQLXrhl2TXMWbW\r\n"
"hpstfcnmXhLKLHZnvfB7UwKBgQCdoujD/51ZUkiF1LObAAuJE5HcdZUL98q2gRkd\r\n"
"VaWHvVRRPQIQH4aToabnTyJnCB2H6MPFwwvQCg1hTQddXLm97TuZSdsMRvPqmqwM\r\n"
"9tI2SPVtTDeSRk7OK7HwY5JYOY5kYaXfXfSMr5dky95HGVJEgvgKIhv0RAVSBAPw\r\n"
"ly0YHQKBgGdB0TORz0tG5zVQxTsiSV+6mmYt4O3Qbb2tnYi5n1mil3kngU25yLBV\r\n"
"QBUFpIbOgOTPWeDuPr1VmArD29BiU3zhtjPhYgyRWILcIeRpdc7Lfw8db3yK3zV6\r\n"
"WLDSJyD10XxCnMNP3pKcHfk0N/7Bk5NCiw5iutP+56sp+fdbAtxw\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

// The client certificate (sent to the server to identify us if required)
// Create it e.g. with 'openssl req' and send it to a trusted CA to certify
const char cli_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDazCCAtSgAwIBAgIBBDANBgkqhkiG9w0BAQUFADCBhzELMAkGA1UEBhMCQ0gx\r\n"
"DzANBgNVBAgMBlZhbGFpczENMAsGA1UEBwwEU2lvbjESMBAGA1UECgwJSGV2cyBj\r\n"
"ZXJ0MRUwEwYDVQQDDAxjZXJ0LmhldnMuY2gxLTArBgkqhkiG9w0BCQEWHnlhbm5p\r\n"
"Y2suZGF5ZXJAc3R1ZGVudHMuaGV2cy5jaDAeFw0xNTA3MDIwOTAyMTlaFw0xODA3\r\n"
"MDEwOTAyMTlaMIGOMQswCQYDVQQGEwJDSDEPMA0GA1UECAwGVmFsYWlzMQ0wCwYD\r\n"
"VQQHDARTaW9uMRMwEQYDVQQKDApIZXZzIChjbGkpMRswGQYDVQQDDBJsd2lwX2Rh\r\n"
"eWVyLmhldnMuY2gxLTArBgkqhkiG9w0BCQEWHnlhbm5pY2suZGF5ZXJAc3R1ZGVu\r\n"
"dHMuaGV2cy5jaDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMcZuWF3\r\n"
"tK9nF9jMkmBCHcjA3nI/Zxc9nrAWMbzOdK++83jMSDHdr6AXA+cmAAP3K3pEg71X\r\n"
"Vsci/w9rymsTXKUC3TQj2adQwoJ7H0MT5hsqq8oPXXF1ZrzTwxT/oK43e0atpAt9\r\n"
"IDzYyQR5V6z6QnnEptDi5SaFiy+/upHQJNZB/hCgDACO6fnLsETC5OYKd06g58f6\r\n"
"gumgjJaUZbaem1A05BXyKZ0yl1JxycNmRd8cKnKSwtHa0PjZ8RyGSqUWlJbrHtcr\r\n"
"gjxEJFjSr89OcdByoBP15kbXzjUfTaXH3V5aNzTgHTLePWIAazj9rtEFdD64y9nC\r\n"
"+o2N/6wH22GagkkCAwEAAaNaMFgwHQYDVR0OBBYEFNlAWQyzKl36N5jLCABbkJux\r\n"
"oxQjMB8GA1UdIwQYMBaAFL5G/7nQF3evMlTZLPItGRcQb60uMAkGA1UdEwQCMAAw\r\n"
"CwYDVR0PBAQDAgWgMA0GCSqGSIb3DQEBBQUAA4GBAHWkx9nKuS89N2efOkAuS2ip\r\n"
"XtxhX6yqDa8wA14Qy165Di8cizW/Ur6aiE877vvlF62p5Ok5k9Svqhhy/NwzfSsI\r\n"
"gjAb/J5vQ5cr/kmVLwu73x3IoMkDWHcbewqo0adyYA8xUFANLS3gGxhgRTe9oxvC\r\n"
"+TIWRMumuDJAZE973osF\r\n"
"-----END CERTIFICATE-----\r\n";

// The client private key in PEM format
// Create it e.g. with 'openssl genrsa'
const char cli_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEowIBAAKCAQEAxxm5YXe0r2cX2MySYEIdyMDecj9nFz2esBYxvM50r77zeMxI\r\n"
"Md2voBcD5yYAA/crekSDvVdWxyL/D2vKaxNcpQLdNCPZp1DCgnsfQxPmGyqryg9d\r\n"
"cXVmvNPDFP+grjd7Rq2kC30gPNjJBHlXrPpCecSm0OLlJoWLL7+6kdAk1kH+EKAM\r\n"
"AI7p+cuwRMLk5gp3TqDnx/qC6aCMlpRltp6bUDTkFfIpnTKXUnHJw2ZF3xwqcpLC\r\n"
"0drQ+NnxHIZKpRaUluse1yuCPEQkWNKvz05x0HKgE/XmRtfONR9NpcfdXlo3NOAd\r\n"
"Mt49YgBrOP2u0QV0PrjL2cL6jY3/rAfbYZqCSQIDAQABAoIBABtJhUS8eUEI6hXG\r\n"
"g9E5foeDF3mlAuYGhfcwM4Ti7s8fZpMJvGNJNMbGILzPUnU4rPbG6R9MFxKX00kz\r\n"
"xrxCHRJogBSjTTNHOEFVyoUQ9xqGEKFS1cW1B22dwEDigA0ggX7AUZlZfs+YDoTU\r\n"
"dJW6ft3de9dpIWrUJq1Wo6VUV5gdmSnaCTauo9jldPJofLl4Gul028Mq87aySn7W\r\n"
"le/nL/YQ0cUIQu5+oZFv8K7teOA42DmtN96M3jcV4CRxY9YWyzithsOeDZuSt7Hu\r\n"
"vn5KUE/1wmzoaezeRGlKtPqkveyMkipdQwsMlirtj/rPTnX/C1MY37r2zs/cGtpv\r\n"
"HY2aqNkCgYEA9rKhJWAquhvagDhU4B3xIaguMsBsjTqwz4immXp9czBlrX+tXxmp\r\n"
"TM20JmJBuXJ4l7BG/DW0xa+tat169kSwYNyq8/3j6kriBkU7waobEO1qpDn32vYz\r\n"
"PjoM7JbLpFqDgYyMtIhyfsJlxtmEppsHpyPvx9tlMnEMjUnQk23cDPsCgYEAzpuj\r\n"
"frCVl+kT2ZQ6DofGq2UhGeyDyb5/hdfqlWJYrnHB4RY9RIkT+jMR0/prS3MlL2NY\r\n"
"VFNkU5Eb5szS44aSvbCJ8e1g3i5d+iVrQP9/sCuJbSd+DIhURBDCDejqTsDYoJjH\r\n"
"xMYFVS6TNLGTyYcQShuidUNTEk9iEyxn9j8YgosCgYAT2z7jPkFSJ4nHxkD2pRJS\r\n"
"jjHhveE+4Nl4atxWACj2y49zjpJK1ErvNqNob4Iwxgj01KhyBnSdcM15i4gpKFA2\r\n"
"C6zq5KWipxJYRNxRczqoeyOZQl0zz5tCR/PgWyba/LoXBFQS9+SXVVhR0BJAQrwz\r\n"
"9M2I/ZzAmZ7UBHv4UeBjXwKBgBQCWLwtR9+VcBmzVEgR+DijK6iCEmVYcKs6tPpQ\r\n"
"svv6AmfVKovXprqldYWNDBUbiq9jNKu+A8pgFE6u6YGlh9KdJU7ryWTbIrjWJOP/\r\n"
"mpMqND4BBCEcluScpt6H/yEvZgtvoHE7KEsOnq0MwZ5T2U0EDiDDK2lrjLkEpPDh\r\n"
"PXNvAoGBAOQTd5kC/zID/u07wpfFvmtY448mZVPq2MHM1W1BSqRLVfalKelIRcl3\r\n"
"iWKDVlNUNyzY1yfWAvlSFtRX2CmzN+Qg5hDwvIqOgJwT+IicZ1ArabfpFrN5AkSz\r\n"
"jrUZ5zcKRA/DbI/nDPeCN0wbjbPL/RbG6EARM7C76uuxgvEdOMVU\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

#else
#define CA_CRT_RSA
#endif /* POLARSSL_RSA_C */



// If elliptic curves are enabled (used if RSA is disabled)
#if defined(POLARSSL_ECDSA_C)

// Use these fields only if you use ECDSA instead of RSA
#define CA_CRT_EC                                                  \
""
const char ca_crt_ec[] = CA_CRT_EC;

const char ca_key_ec[] =
"";

const char ca_pwd_ec[] = "";

const char srv_crt_ec[] =
"";

const char srv_key_ec[] =
"";

const char cli_crt_ec[] =
"";

const char cli_key_ec[] =
"";
#else
#define CA_CRT_EC
#endif /* POLARSSL_ECDSA_C */




/* Concatenation of all available CA certificates */
const char ca_list[] = CA_CRT_RSA CA_CRT_EC;

#if defined(POLARSSL_RSA_C)
const char *ca_crt = ca_crt_rsa;
const char *ca_key = ca_key_rsa;
const char *ca_pwd = ca_pwd_rsa;
const char *srv_crt = srv_crt_rsa;
const char *srv_key = srv_key_rsa;
const char *cli_crt = cli_crt_rsa;
const char *cli_key = cli_key_rsa;
#else /* ! POLARSSL_RSA_C, so POLARSSL_ECDSA_C */
const char *ca_crt = ca_crt_ec;
const char *ca_key = ca_key_ec;
const char *ca_pwd = ca_pwd_ec;
const char *srv_crt = srv_crt_ec;
const char *srv_key = srv_key_ec;
const char *cli_crt = cli_crt_ec;
const char *cli_key = cli_key_ec;
#endif /* POLARSSL_RSA_C */

#endif /* POLARSSL_CERTS_C */

#endif /* USE_MBEDTLS */



#endif /* MBEDTLSCERTS_H_ */
