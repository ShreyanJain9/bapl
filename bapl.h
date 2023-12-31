// bapl.h - Shared header file

#ifndef BAPL_H
#define BAPL_H

// Define the structure for the BAPL header
typedef struct {
    char signature[4];
    uint16_t width;
    uint16_t height;
    uint16_t frame_rate;
} BAPLHeader;

// Function prototype to read the header of the BAPL file
int readBAPLHeader(const char *filename, BAPLHeader *header);

#endif // BAPL_H
