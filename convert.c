#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ImageMagick-6/wand/MagickWand.h>
#include "bapl.h" // Include the shared header for BAPL

int encodeImageAsBAPL(const char *inputFilename, const char *outputFilename) {
    MagickWand *wand;
    PixelIterator *iterator;
    PixelWand **pixels;

    // Initialize MagickWand
    MagickWandGenesis();
    
    // Create MagickWand
    wand = NewMagickWand();

    // Load the input image
    if (MagickReadImage(wand, inputFilename) == MagickFalse) {
        fprintf(stderr, "Error loading image '%s'\n", inputFilename);
        return 0;
    }

    // Get image dimensions
    size_t width = MagickGetImageWidth(wand);
    size_t height = MagickGetImageHeight(wand);

    // Create BAPL header
    BAPLHeader header;
    strncpy(header.signature, "BAPL", 4);
    header.width = (uint16_t)width;
    header.height = (uint16_t)height;
    header.frame_rate = 0; // Frame rate 0 signifies an image in BAPL

    // Open output file for writing the BAPL header and pixel data
    FILE *outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening output file '%s'\n", outputFilename);
        DestroyMagickWand(wand);
        MagickWandTerminus();
        return 0;
    }

    // Write BAPL header to the output file
    fwrite(&header, sizeof(BAPLHeader), 1, outputFile);

    // Allocate memory for pixel data (1 bit per pixel)
    size_t totalPixels = width * height;
    size_t totalBytes = totalPixels / 8 + ((totalPixels % 8 != 0) ? 1 : 0);
    uint8_t *pixelData = (uint8_t *)calloc(totalBytes, sizeof(uint8_t));
    if (pixelData == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(outputFile);
        DestroyMagickWand(wand);
        MagickWandTerminus();
        return 0;
    }

    // Iterate through the image pixels and encode as 1-bit data
    iterator = NewPixelIterator(wand);
    size_t byteIndex = 0;
    size_t bitIndex = 0;
    for (ssize_t y = 0; y < (ssize_t)height; ++y) {
        pixels = PixelGetNextIteratorRow(iterator, &width);
        if (pixels == NULL)
            break;

        for (ssize_t x = 0; x < (ssize_t)width; ++x) {
            Quantum pixelIntensity = PixelGetRed(pixels[x]) * 255; // Assuming grayscale; adjust for other formats

            // Use a threshold to determine 1-bit representation
            uint8_t bitValue = (pixelIntensity > 128) ? 1 : 0;
            pixelData[byteIndex] |= (bitValue << (7 - bitIndex));

            bitIndex++;
            if (bitIndex == 8) {
                bitIndex = 0;
                byteIndex++;
            }
        }
        PixelSyncIterator(iterator);
    }
    DestroyPixelIterator(iterator);

    // Write pixel data to the output file
    fwrite(pixelData, sizeof(uint8_t), totalBytes, outputFile);

    // Clean up resources
    free(pixelData);
    fclose(outputFile);
    DestroyMagickWand(wand);
    MagickWandTerminus();

    printf("Image encoded as BAPL successfully: %s\n", outputFilename);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_image_filename> <output_bapl_filename>\n", argv[0]);
        return 1;
    }

    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];

    if (!encodeImageAsBAPL(inputFilename, outputFilename)) {
        fprintf(stderr, "Failed to encode the image as BAPL.\n");
        return 1;
    }

    return 0;
}
