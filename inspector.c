#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bapl.h"

int readBAPLHeader(const char *filename, BAPLHeader *header) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }
    // Read the header information
    fread(header, sizeof(BAPLHeader), 1, file);
    fclose(file);
    // Check if file has BAPL signature
    if (!(strcmp(header->signature, "BAPL"))) {
        perror("File is not in BAPL format");
	return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <BAPL filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    BAPLHeader header;

    // Read the header from the specified BAPL file
    if (readBAPLHeader(filename, &header)) {
        printf("BAPL File Information:\n");
        printf("Signature: %c%c%c%c\n", header.signature[0], header.signature[1], header.signature[2], header.signature[3]);
        printf("Width: %d pixels\n", header.width);
        printf("Height: %d pixels\n", header.height);
        printf("Frame Rate: %d FPS\n", header.frame_rate);
    }

    return 0;
}
