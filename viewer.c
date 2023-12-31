#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "bapl.h" // Include the shared header for BAPL

#define SCREEN_TITLE "BAPL Viewer"

// Function to read BAPL header
int readBAPLHeader(const char *filename, BAPLHeader *header) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        return 0;
    }

    // Read the header information
    fread(header, sizeof(BAPLHeader), 1, file);

    fclose(file);
    return 1;
}

// Function to display BAPL content using SDL
void displayBAPL(const char *filename) {
    BAPLHeader header;

    // Read BAPL header
    if (!readBAPLHeader(filename, &header)) {
        fprintf(stderr, "Failed to read BAPL header.\n");
        return;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return;
    }

    // Create SDL window
    SDL_Window *window = SDL_CreateWindow(
        SCREEN_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        header.width,
        header.height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Add BAPL header info to window title
    char title[100];
    snprintf(title, sizeof(title), "%s - Width: %d Height: %d Frame Rate: %d", SCREEN_TITLE, header.width, header.height, header.frame_rate);
    SDL_SetWindowTitle(window, title);

    // Create SDL renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "SDL renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Calculate total bytes for pixel data
    size_t totalPixels = header.width * header.height;
    size_t totalBytes = totalPixels / 8 + ((totalPixels % 8 != 0) ? 1 : 0);

    // Allocate memory for pixel data
    uint8_t *pixelData = (uint8_t *)calloc(totalBytes, sizeof(uint8_t));
    if (pixelData == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Open BAPL file for reading pixel data
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        free(pixelData);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Seek past the header to read pixel data
    fseek(file, sizeof(BAPLHeader), SEEK_SET);
    fread(pixelData, sizeof(uint8_t), totalBytes, file);
    fclose(file);

    // Create SDL surface and texture
    SDL_Surface *surface = SDL_CreateRGBSurface(0, header.width, header.height, 32, 0, 0, 0, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, header.width, header.height);

    if (!surface || !texture) {
        fprintf(stderr, "SDL surface/texture creation failed: %s\n", SDL_GetError());
        free(pixelData);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Set pixels on surface
    SDL_LockSurface(surface);
    uint32_t *pixels = (uint32_t *)surface->pixels;

    for (size_t i = 0; i < totalPixels; ++i) {
        uint8_t bit = (pixelData[i / 8] >> (7 - (i % 8))) & 0x01;
        pixels[i] = bit ? 0xFFFFFFFF : 0xFF000000; // Set pixel color: White for bit=1, Black for bit=0
    }

    SDL_UnlockSurface(surface);

    // Main event loop
    int running = 1;
    SDL_Event event;
    uint32_t delay = (header.frame_rate != 0) ? (1000 / header.frame_rate) : 0; // Delay between frames based on frame rate

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Clear renderer
        SDL_RenderClear(renderer);

        // Update texture with pixel data from surface
        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

        // Render texture to window
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Render the frame
        SDL_RenderPresent(renderer);

        // Delay to respect frame rate (avoid division by zero)
        if (delay != 0) {
            SDL_Delay(delay);
        }
    }

    // Clean up resources
    free(pixelData);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_bapl_filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Display BAPL content using SDL
    displayBAPL(filename);

    return 0;
}
