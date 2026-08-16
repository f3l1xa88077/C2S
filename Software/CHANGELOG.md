## Software JPEG core module — 2026-08-16

### Added
- Vendored the **JPEGENC** encoder library into both projects:
  `C2Sv1/Drivers/Custom/JPEGENC/` and `C2Sv0.1/Drivers/Custom/JPEGENC/`.
- **`img_core`** module wrapping JPEGENC. It encodes one band at a time, so
  the full frame never has to be resident in RAM:
  - **C2Sv1** (`Drivers/Custom/Src/Imaging/img_core.{c,h}`): encodes RGB565
    frames staged in external QSPI, into a buffer.
  - **C2Sv0.1** (`Drivers/Custom/Src/Imaging/img_core.{c,h}`): grayscale
    test copy using test image in RAM rather than full image flow intended for v1.

### Verified
- Software encode on the **C2Sv0.1** board: 1024×720 grayscale at `JPEGE_Q_HIGH`
  produced a 69223 byte JPEG that decodes correctly.

### Notes
- **v1 pipeline wiring is unfinished:** Image capture → DCMI → QSPI staging →
  encode is not yet built, however the core encoder is proven from v0.1.