#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;

int next_multiple(int from, int of) { return (from / of) * of; }

void print_hex(uint8 *string, int length) {
    for (int i = 0; i < length; i++)
        printf("%02x", string[i]);
}

uint32 leftrotate(uint32 num, uint32 shift) { return ((num) << (shift)) | ((num) >> (32 - (shift))); }

/// @brief Compute md5 hash of message.
/// @param message The message.
/// @param length Length of message in bytes.
/// @param hash The output hash, must contain at least 16 bytes.
void md5(const char *message, int length, char *hash) {
    // These are just fixed constants used in the hash computation.
    uint32 S[64] = {
        7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 5,  9,  14, 20, 5,  9,
        14, 20, 5,  9,  14, 20, 5,  9,  14, 20, 4,  11, 16, 23, 4,  11, 16, 23, 4,  11, 16, 23,
        4,  11, 16, 23, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21,
    };
    uint32 K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };

    uint32 a0 = 0x67452301;
    uint32 b0 = 0xefcdab89;
    uint32 c0 = 0x98badcfe;
    uint32 d0 = 0x10325476;

    // Padded message.
    int padded_length = next_multiple(length, 64);
    if ((int)padded_length - length < 9)
        padded_length += 64;
    char *padded_message = (char *)calloc(padded_length, 1);
    memcpy(padded_message, message, length);

    // Add 1 bit.
    padded_message[length] = 0x80;

    // 0s are already there thanks to calloc.

    // Add length in bits as uint64.
    uint64 length_bits = length * 8;
    memcpy(padded_message + padded_length - 8, &length_bits, sizeof(uint64));

    // For each 512 bit chunk (64 bytes).
    for (int offset = 0; offset < padded_length; offset += 64) {
        uint32 *M = (uint32 *)(padded_message + offset);
        uint32 A = a0;
        uint32 B = b0;
        uint32 C = c0;
        uint32 D = d0;

        for (int i = 0; i < 64; i++) {
            uint32 F, g;
            if (i < 16) {
                F = (B & C) | ((~B) & D);
                g = i;
            } else if (i < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            } else {
                F = C ^ (B | (~D));
                g = (7 * i) % 16;
            }
            // Be wary of the below definitions of a,b,c,d
            F = F + A + K[i] + M[g];
            A = D;
            D = C;
            C = B;
            B = B + leftrotate(F, S[i]);
        }

        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    uint32 *hash32 = (uint32 *)hash;
    hash32[0] = a0;
    hash32[1] = b0;
    hash32[2] = c0;
    hash32[3] = d0;

    free(padded_message);
}

void print_hash(const char *string) {
    int len = strlen(string);
    char hash[17] = {0};
    md5(string, len, hash);
    print_hex(hash, 16);
}

int hash_hex_zeros(const char *hash) {
    int zeros = 0;
    for (int i = 0; i < 16; i++) {
        if (hash[i] == 0) {
            zeros += 2;
            continue;
        } else if ((hash[i] & 0xF0) == 0) {
            zeros++;
            break;
        }
        break;
    }
    return zeros;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_4.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char content[size + 1];
    content[size] = 0;
    size_t read = fread(content, 1, size, input);

    // Close file.
    fclose(input);

    // Examples.
    printf("Hash of \"%s\": ", "");
    print_hash("");
    printf("\n");

    printf("Hash of \"%s\": ", "a");
    print_hash("a");
    printf("\n");

    printf("Hash of \"%s\": ", "abc");
    print_hash("abc");
    printf("\n");

    // Buffer containing the string for which to compute the hash.
    char hash[17] = {0};
    char buffer[1024] = {0};
    memcpy(buffer, content, size);

    for (int i = 0; i < INT32_MAX; i++) {
        // Append number to string.
        int printed = sprintf(buffer + size, "%d", i);

        // Compute hash.
        md5(buffer, size + printed, hash);

        // Check if it starts with 5 zeros.
        if (hash_hex_zeros(hash) >= 5) {
            printf("%d starts with 5 zeros.\n", i);
            break;
        }
    }

    return 0;
}