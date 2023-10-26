#ifndef CRYPT_H
# define CRYPT_H

# include <Arduino.h>
# include <exception>
# include "mbedtls/pk.h"
# include "mbedtls/sha256.h"
# include "mbedtls/error.h"
# include "mbedtls/base64.h"

/*
	Wrapper for mbedtls.
	offers sing and verify methods. For base64 signatures.
*/
class Cypher {
	private:
		mbedtls_pk_context pkPublic;
		mbedtls_pk_context pkPrivate;
		const char *publicKeyPEM;
		const char *privateKeyPEM;
		// Maybe you want to add a mbedtls error buffer, etc.

	public:
		Cypher(
			const char *publicKeyPEM,
			const char *privateKeyPEM
		);
		virtual ~Cypher();

		void init();

		/*
			Verifies base64-encoded signature against message.
		*/
		bool verify(
			const char *message,
			const char *signatureBase64
		);

		/*
			Signs message and returns base64-encoded signature.
		*/
		const String sign(
			const char *message
		);
};

#endif