#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))

void sha1_transform(uint32_t state[5], const uint8_t data[]) {
    uint32_t a, b, c, d, e, f, k, temp, w[80];
    int i;

    for (i = 0; i < 16; ++i) {
        w[i]  = (data[i * 4]     << 24);
        w[i] |= (data[i * 4 + 1] << 16);
        w[i] |= (data[i * 4 + 2] << 8);
        w[i] |= (data[i * 4 + 3]);
    }
    for (; i < 80; ++i) {
        w[i] = ROTLEFT(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    for (i = 0; i < 80; ++i) {
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        temp = ROTLEFT(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = temp;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void sha1(const uint8_t *message, size_t len, uint8_t hash[20]) {
    uint32_t state[5] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476,
        0xC3D2E1F0
    };

    uint8_t data[64];
    uint64_t bitlen = len * 8;
    size_t i;

    while (len >= 64) {
        memcpy(data, message, 64);
        sha1_transform(state, data);
        message += 64;
        len -= 64;
    }

    memset(data, 0, 64);
    memcpy(data, message, len);
    data[len] = 0x80;

    if (len >= 56) {
        sha1_transform(state, data);
        memset(data, 0, 64);
    }

    // Append original length in bits (big-endian)
    data[63] = bitlen;
    data[62] = bitlen >> 8;
    data[61] = bitlen >> 16;
    data[60] = bitlen >> 24;
    data[59] = bitlen >> 32;
    data[58] = bitlen >> 40;
    data[57] = bitlen >> 48;
    data[56] = bitlen >> 56;

    sha1_transform(state, data);

    for (i = 0; i < 5; ++i) {
        hash[i * 4]     = (state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (state[i] >> 8)  & 0xFF;
        hash[i * 4 + 3] = (state[i])       & 0xFF;
    }
}

int main() {
    char input[1024];
    uint8_t hash[20];

    printf("Enter a message to hash (max 1023 characters): ");
    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "Input error\n");
        return 1;
    }

    // Remove newline if present
    size_t len = strlen(input);
    if (input[len - 1] == '\n') {
        input[len - 1] = '\0';
        len--;
    }

    sha1((const uint8_t *)input, len, hash);

    printf("SHA-1 hash: ");
    for (int i = 0; i < 20; ++i) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}
