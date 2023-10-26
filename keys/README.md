# Keys Directory

Paste a public SHA256 key here so microcontroller can authenticate with the server.

## Generating a key pair

```bash
openssl genpkey -algorithm RSA -out private_key.pem
```

## Extracting the public key

```bash
openssl rsa -pubout -in private_key.pem -out public_key.pem
```
