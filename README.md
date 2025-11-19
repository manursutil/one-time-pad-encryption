# OTP Algorithm

- Read a plaintext file and emit an encrypted `output.txt` plus a matching `secret_key.txt`.
- Reconstruct the plaintext when given the key file and encrypted output.

## Build

```bash
g++ -std=c++17 main.cpp -o main
```

## Usage

```bash
# Encrypt a text file
./main encrypt text.txt

# Decrypt using the generated key and ciphertext
./main decrypt secret_key.txt output.txt
```

## How It Works

1. `encrypt` reads every whitespace-delimited token from the input file, reconstructs the text (keeping spaces), and generates a random byte (0-255) per character as the key.
2. It XORs the plaintext with the random key to produce integers that represent the ciphertext.
3. The key values are written to `secret_key.txt`; the ciphertext integers go to `output.txt`.
4. `decrypt` performs the reverse XOR operation using the two files and prints the recovered message.

## Notes

- Because it uses `rand()` seeded with `time(NULL)`, encryption is not cryptographically secure; it simply demonstrates the OTP mechanics.
- All files are read from and written to the working directory; update the `encrypt`/`decrypt` arguments if you keep files elsewhere.

