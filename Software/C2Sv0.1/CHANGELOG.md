# Changelog

## Stream JPEG encoding band-by-band to support high-quality image inputs

### Added
- HAL_JPEG_GetDataCallback feeds the codec one band at a time via HAL_JPEG_ConfigInputBuffer.
- JPEG_Encode_Progress state struct + MAX_WIDTH image dimension bound.

### Changed
- 'JPEG_Encode_Gray' no longer tiles the whole frame into a 128x128 'mcu_buf'.

### Notes
- Still a static test image and harness, no live DCMI capture.
- Verified on board



## Fix JPEG encoded-length reporting

### Fixed
- `JPEG_Encode_Gray` reported `*out_len` from `hjpeg->OutDataLength` — the output
  buffer capacity (~20000), not the real compressed size, so SD files were
  written padded to the full buffer. The true length is now accumulated from
  `HAL_JPEG_DataReadyCallback` into a file-scope counter (reset before each
  encode) and returned via `*out_len`.
- `main.c`: `JEPG_OUT_CAP` → `JPEG_OUT_CAP` macro typo.

### Notes
- Verified on hardware

## Finalise v0.1 JPEG Encoding

### Added
- `jpeg_codec.c/.h` — grayscale JPEG encoder wrapping the STM32U5G9 hardware
  codec: configures the codec (size/quality/grayscale), tiles the raster image
  into 8x8 MCU blocks, runs a blocking encode, reports the compressed length,
  and guards both HAL calls.
- Copied in `jpeg_utils.c/.h` + `jpeg_utils_conf.h` (for the future RGB path).
- Pre-captured 128x128 grayscale test image into a `const` header to be used as encoder input.

### Changed
- `main.c` now calls `JPEG_Encode_Gray(...)` in place of the inline `HAL_JPEG_*`
  test; the SD write uses the returned `jpeg_size`.
- Replaced the synthetic stripe pattern with the pre-captured test image.

### Fixed
- `.gitignore` bug: the version directory's contents are now excluded
  (`/*`) with Core `Inc`/`Src`/`Startup` re-includes, so `Drivers/Custom` and Core
  sources are actually tracked instead of ignored.

### Notes
- Not yet hardware-verified. 
- No camera to encoding pipeline set up yet, only uses a pre-generated test image.

