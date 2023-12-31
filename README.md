**1-Bit Graphics with Image/Video**

**Header:**
- Signature (4 bytes): "BAPL" to identify the file format.
- Width (2 bytes): Number of pixels in a row.
- Height (2 bytes): Number of pixels in a column.
- Frame Rate (2 bytes): Number of frames per second (FPS). If the frame rate is 0, it's treated as an image.

**Pixel Data:**
- Each pixel represented by 1 bit (0 or 1).
- Pixels stored row-wise, packed into bytes or suitable units.
- Image size: (Width * Height) bits or (Width * Height) / 8 bytes for storage, with each bit representing a pixel.

**Data Interpretation:**
- If the frame rate is 0, treat the content as a static image.
- If the frame rate is greater than 0, interpret it as a video with the specified FPS.
