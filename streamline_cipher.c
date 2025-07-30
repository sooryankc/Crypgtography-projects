#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// --- LFSR-based keystream generator ---
uint8_t lfsr_next(uint8_t *lfsr) {
    uint8_t bit = ((*lfsr >> 7) ^ (*lfsr >> 5) ^ (*lfsr >> 4) ^ (*lfsr >> 3)) & 1;
    *lfsr = (*lfsr << 1) | bit;
    return *lfsr;
}

// --- XOR stream cipher (symmetric) ---
void stream_cipher_symmetric(uint8_t *data, size_t len, uint8_t key) {
    uint8_t lfsr = key;
    for (size_t i = 0; i < len; i++) {
        data[i] ^= lfsr_next(&lfsr);
    }
}

// --- Asymmetric demo: use 2 keys to simulate concept ---
void stream_cipher_asymmetric(uint8_t *data, size_t len, uint8_t pub_key, uint8_t priv_key, int encrypt) {
    uint8_t key = encrypt ? pub_key : priv_key;
    uint8_t lfsr = key;
    for (size_t i = 0; i < len; i++) {
        data[i] ^= lfsr_next(&lfsr);
    }
}

// --- Convert hex string to byte array ---
size_t hex_string_to_bytes(const char *hex_str, uint8_t *bytes, size_t max_len) {
    size_t count = 0;
    while (*hex_str && count < max_len) {
        while (*hex_str && isspace((unsigned char)*hex_str)) hex_str++;
        if (!isxdigit(hex_str[0]) || !isxdigit(hex_str[1])) break;
        unsigned int byte;
        if (sscanf(hex_str, "%2x", &byte) != 1) break;
        bytes[count++] = (uint8_t)byte;
        hex_str += 2;
    }
    return count;
}

int main() {
    char cipher_mode;
    char action_mode;
    char input[512];
    uint8_t data[256];
    size_t len = 0;

    printf("Choose cipher type: (S)ymmetric or (A)symmetric: ");
    scanf(" %c", &cipher_mode);
    getchar();

    printf("Encrypt (E) or Decrypt (D): ");
    scanf(" %c", &action_mode);
    getchar();

    if ((cipher_mode == 'S' || cipher_mode == 's') &&
        (action_mode == 'E' || action_mode == 'e')) {
        
        uint8_t key;
        printf("Enter symmetric key (0-255): ");
        scanf("%hhu", &key);
        getchar();

        printf("Enter plaintext: ");
        fgets((char*)data, sizeof(data), stdin);
        len = strlen((char*)data);
        if (data[len - 1] == '\n') data[--len] = '\0';

        stream_cipher_symmetric(data, len, key);

        printf("Encrypted (hex): ");
        for (size_t i = 0; i < len; i++)
            printf("%02X ", data[i]);
        printf("\n");

    } else if ((cipher_mode == 'S' || cipher_mode == 's') &&
               (action_mode == 'D' || action_mode == 'd')) {
        
        uint8_t key;
        printf("Enter symmetric key (0-255): ");
        scanf("%hhu", &key);
        getchar();

        printf("Enter hex input: ");
        fgets(input, sizeof(input), stdin);
        len = hex_string_to_bytes(input, data, sizeof(data));

        stream_cipher_symmetric(data, len, key);

        printf("Decrypted text: ");
        for (size_t i = 0; i < len; i++)
            printf("%c", isprint(data[i]) ? data[i] : '.');
        printf("\n");

    } else if ((cipher_mode == 'A' || cipher_mode == 'a') &&
               (action_mode == 'E' || action_mode == 'e')) {
        
        uint8_t pub_key;
        printf("Enter public key (0-255): ");
        scanf("%hhu", &pub_key);
        getchar();

        printf("Enter plaintext: ");
        fgets((char*)data, sizeof(data), stdin);
        len = strlen((char*)data);
        if (data[len - 1] == '\n') data[--len] = '\0';

        stream_cipher_asymmetric(data, len, pub_key, 0, 1); // encrypt with pub

        printf("Encrypted (hex): ");
        for (size_t i = 0; i < len; i++)
            printf("%02X ", data[i]);
        printf("\n");

    } else if ((cipher_mode == 'A' || cipher_mode == 'a') &&
               (action_mode == 'D' || action_mode == 'd')) {
        
        uint8_t priv_key;
        printf("Enter private key (0-255): ");
        scanf("%hhu", &priv_key);
        getchar();

        printf("Enter hex input: ");
        fgets(input, sizeof(input), stdin);
        len = hex_string_to_bytes(input, data, sizeof(data));

        stream_cipher_asymmetric(data, len, 0, priv_key, 0); // decrypt with priv

        printf("Decrypted text: ");
        for (size_t i = 0; i < len; i++)
            printf("%c", isprint(data[i]) ? data[i] : '.');
        printf("\n");

    } else {
        printf("Invalid option selected.\n");
    }

    system("pause");  // keep window open on Windows
    return 0;
}
