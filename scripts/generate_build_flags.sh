#!/bin/bash

# Function to create a key definition
create_key_definition() {
    local key_file="$1"
    local key_name="$2"
    local output_file="$3"

    # Begin the key definition
    echo -n "#define $key_name" >> "$output_file"

    # Read the content of the key file and format each line
    while IFS= read -r line; do
        echo " \\" >> "$output_file"
        echo -n "\"$line\\n\"" >> "$output_file"
    done < "$key_file"

    # Add extra newline for separation
    echo "" >> "$output_file"
    echo "" >> "$output_file"
}

# Start the header file
output_file="include/cypher_keys.h"
echo "#ifndef CYPHER_KEYS_H" > "$output_file"
echo "#define CYPHER_KEYS_H" >> "$output_file"
echo "" >> "$output_file"

# Create the public and private key definitions
create_key_definition "keys/public_key.pem" "CYPHER_PUBLIC_KEY" "$output_file"
create_key_definition "keys/private_key.pem" "CYPHER_PRIVATE_KEY" "$output_file"

# End the definitions
echo "#endif // CYPHER_KEYS_H" >> "$output_file"