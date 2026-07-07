# C2S v0.1 Firmware

Firmware for the **v0.1** board of the PAST **C2S** project. It runs on an STM32U5G9, captures camera images, and compresses them to JPEG on the MCU's hardware JPEG codec before storing them to microSD.

## Configuration

- Target MCU: **STM32U5G9ZJTxQ**.
- JPEG image size and quality are set at the `JPEG_Encode_Gray(...)` call in `Core/Src/main.c`.

## Usage

- Flash to the board via ST-Link.
- `JPEG_Encode_Gray()` (`Drivers/Custom/Src/Peripherals/jpeg_codec.c`) encodes a grayscale image buffer to JPEG; the result is written to microSD.

## Contact

gabe.8237 (Discord)

## License
