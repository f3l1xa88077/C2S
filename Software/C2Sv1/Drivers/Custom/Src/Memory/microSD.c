///*
// * microSD.c
// *
// *  Created on: Jun 25, 2026
// *      Author: felix
// *
// *     	Copied over from the v0.1 source code and adapted to work with SPI instead of SDMMC.
// *
// */
//
//#include"microSD.h"
//
//
//
///* FileX handles and memory */
//static FX_MEDIA        sd_disk;
//static FX_FILE         my_file;
//static uint8_t         media_memory[512];
//
//extern VOID fx_stm32_spi_sd_driver(FX_MEDIA *media_ptr);
//
//uint32_t uSD_Init(void) {
//    fx_system_initialize();
//
//    /* Initialises the SD driver and opens the media */
//    return fx_media_open(&sd_disk, "STM32_SPI_SD", fx_stm32_spi_sd_driver,
//                         (VOID *)FX_NULL, media_memory, sizeof(media_memory));
//}
//
//void uSD_Test(void) {
//    uint32_t status;
//
//    // 1. Initialise and Open Media
//    status = uSD_Init();
//    if (status != FX_SUCCESS) {
//        return;
//    }
//
//    // 2. Create and Open File
//    // fx_file_create will return an error if file exists, we ignore it and just open.
//    fx_file_create(&sd_disk, "TEST.TXT");
//    status = fx_file_open(&sd_disk, &my_file, "TEST.TXT", FX_OPEN_FOR_WRITE);
//
//    if (status == FX_SUCCESS) {
//        // 3. Write Data
//        char *msg = "Hello from STM32U5 to SD Card using FileX!\r\n";
//        fx_file_write(&my_file, msg, strlen(msg));
//
//        // 4. Close File and Flush Cache to Physical Media
//        fx_file_close(&my_file);
//        fx_media_flush(&sd_disk);
//    }
//
//    // 5. Safely Unmount
//    fx_media_close(&sd_disk);
//}
//
///**
// * @brief Streams data to a binary file on the SD card.
// *
// * @param filename: Name of the file (e.g., "DATA.BIN")
// * @param data: Pointer to the 8-bit or 16-bit buffer
// * @param size_in_bytes: Total size to write (use: num_elements * sizeof(element))
// * @param is_last_chunk: If 1, closes the file; if 0, keeps it open for more data.
// */
//uint32_t SD_Stream_Data(char* filename, void* data, uint32_t size_in_bytes, uint8_t is_last_chunk) {
//    uint32_t status;
//
//    // 1. Open the media if not already open
//    // Note: In a real app, you might move uSD_Init to your startup code
//    if (sd_disk.fx_media_id != FX_MEDIA_ID) {
//        status = uSD_Init();
//        if (status != FX_SUCCESS) return status;
//    }
//
//    // 2. Open or Create the file
//    // FX_OPEN_FOR_WRITE starts at the beginning.
//    // Use fx_file_relative_seek(&my_file, 0, FX_SEEK_END) if appending to existing data.
//    status = fx_file_open(&sd_disk, &my_file, filename, FX_OPEN_FOR_WRITE);
//    if (status == FX_NOT_FOUND) {
//        fx_file_create(&sd_disk, filename);
//        status = fx_file_open(&sd_disk, &my_file, filename, FX_OPEN_FOR_WRITE);
//    }
//
//    if (status != FX_SUCCESS) return status;
//
//    // 3. Write the buffer
//    // FileX handles the byte-alignment internally
//    status = fx_file_write(&my_file, data, size_in_bytes);
//
//    // 4. Finalize
//    if (is_last_chunk) {
//        fx_file_close(&my_file);
//        fx_media_flush(&sd_disk);
//        fx_media_close(&sd_disk);
//    } else {
//        // Optional: Flush periodically to prevent data loss if power is cut
//        fx_media_flush(&sd_disk);
//    }
//
//    return status;
//}
//
///**
// * @brief Simulates an image data stream and writes it to the SD card.
// */
//void Test_Image_Stream(void) {
//    uint32_t status;
//    uint16_t dummy_image_row[256]; // 256 pixels * 2 bytes = 512 bytes (one sector)
//    uint16_t num_rows = 100;       // Total "image" height
//
//    // 1. Initialise the SD card
//    status = uSD_Init();
//    if (status != FX_SUCCESS) return;
//
//    // 2. Create and Open a binary file
//    fx_file_create(&sd_disk, "IMAGE.BIN");
//    status = fx_file_open(&sd_disk, &my_file, "IMAGE.BIN", FX_OPEN_FOR_WRITE);
//    if (status != FX_SUCCESS) return;
//
//    // 3. Stream the "Image" row by row
//    for (uint16_t i = 0; i < num_rows; i++) {
//        // Generate a synthetic pattern (e.g., a gradient)
//        for (uint16_t j = 0; j < 256; j++) {
//            dummy_image_row[j] = (uint16_t)(i + j);
//        }
//
//        // Write the 512-byte row to the SD card
//        status = fx_file_write(&my_file, dummy_image_row, sizeof(dummy_image_row));
//
//        if (status != FX_SUCCESS) break;
//    }
//
//    // 4. Finalize and Flush
//    fx_file_close(&my_file);
//    fx_media_flush(&sd_disk);
//    fx_media_close(&sd_disk);
//}
