## RGB565 + large-frame (SD-streamed) software encoding — 2026-08-19

### Changed
- v0.1 `img_core` now encodes RGB565 (4:2:0) instead of grayscale.
- Source is streamed band-by-band from an SD file (FileX) instead of a RAM
  pointer, so frames too large for flash/RAM (up to 2560x1440) can be encoded.
  API is now `img_encode_jpeg(FX_FILE *src, ...)`.

### Notes
- SD-streamed path hardware verified on 640x480 and 2560x1440 

## Software JPEG core module — 2026-08-16

### Added
- Vendored the **JPEGENC** encoder library into both projects:
  `C2Sv1/Drivers/Custom/JPEGENC/` and `C2Sv0.1/Drivers/Custom/JPEGENC/`.
- **`img_core`** module wrapping JPEGENC. It encodes one band at a time, so
  the full frame never has to be resident in RAM:

### Verified
- Software encode on the **C2Sv0.1** board: 1024×720 grayscale at `JPEGE_Q_HIGH`
  produced a 69223 byte JPEG that decodes correctly.

### Notes
- **v1 pipeline wiring is unfinished:** Image capture → DCMI → QSPI staging →
  encode is not yet built, however the core encoder is proven from v0.1.