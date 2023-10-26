#include <Cypher.h>

Cypher::Cypher(
	const char *publicKeyPEM,
	const char *privateKeyPEM
)
	: publicKeyPEM(publicKeyPEM),
	  privateKeyPEM(privateKeyPEM)
{}

static void printError(int ret) {
	char error_buf[100]; // Buffer for error string
	mbedtls_strerror(ret, error_buf, sizeof(error_buf));
	Serial.println("Error:");
	Serial.println(error_buf);
}


void Cypher::init()
{
	mbedtls_pk_init(&pkPublic);
	int ret = mbedtls_pk_parse_public_key(
		&pkPublic,
		(const unsigned char *)publicKeyPEM,
		strlen(publicKeyPEM) + 1
	);
	if (ret != 0) {
		printError(ret);
		throw std::runtime_error("Failed to parse public key.");
	}
	mbedtls_pk_init(&pkPrivate);
	ret = mbedtls_pk_parse_key(
		&pkPrivate,
		(const unsigned char *)privateKeyPEM,
		strlen(privateKeyPEM) + 1,
		nullptr,
		0
	);
	if (ret != 0) {
		printError(ret);
		throw std::runtime_error("Failed to parse private key.");
	}
}

Cypher::~Cypher() {
	mbedtls_pk_free(&pkPublic);
}

bool Cypher::verify(
	const char *message,
	const char *signatureBase64
) {
    unsigned char hash[32]; // SHA-256 hash length.
    mbedtls_sha256(
        (const unsigned char *)message,
        strlen(message),
        hash,
        false // 0 indicates SHA-256, not SHA-224.
    ); 

    unsigned char decodedSignature[MBEDTLS_MPI_MAX_SIZE]; // Ensure the array is large enough.
    size_t decodedLength;
	const int err = mbedtls_base64_decode(
		decodedSignature,
		MBEDTLS_MPI_MAX_SIZE,
		&decodedLength,
		(const unsigned char *)signatureBase64,
		strlen(signatureBase64)
	);
	if (err != 0) {
		printError(err);
		return false;
	} 
    int ret = mbedtls_pk_verify(
        &pkPublic,
        MBEDTLS_MD_SHA256,
        hash,
        sizeof(hash),
        decodedSignature,
        decodedLength
    );
    return (ret == 0);
}

const String Cypher::sign(const char *message)
{
	unsigned char hash[32]; // SHA-256 hash length.
	mbedtls_sha256(
		(const unsigned char *)message,
		strlen(message),
		hash,
		false // 0 indicates SHA-256, not SHA-224.
	); 

	unsigned char signature[MBEDTLS_MPI_MAX_SIZE];
	size_t signatureLength;
	int ret = mbedtls_pk_sign(
		&pkPrivate,
		MBEDTLS_MD_SHA256,
		hash,
		sizeof(hash),
		signature,
		&signatureLength,
		NULL,
		NULL
	);
	if (ret != 0) {
		printError(ret);
		throw std::runtime_error("Failed to sign message.");
	}
	unsigned char encodedSignature[MBEDTLS_MPI_MAX_SIZE];
	size_t encodedLength;
	ret = mbedtls_base64_encode(
		encodedSignature,
		MBEDTLS_MPI_MAX_SIZE,
		&encodedLength,
		signature,
		signatureLength
	);
	if (ret != 0) {
		printError(ret);
		throw std::runtime_error("Failed to encode signature.");
	}
	return String((const char *)encodedSignature, encodedLength);	
}
