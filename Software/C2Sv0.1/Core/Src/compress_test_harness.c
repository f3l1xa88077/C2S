/*
 * compress_test_harness.c
 *
 *  Created on: 20 July 2026
 *      Author: Dimitrije Simic
 */

// TODO: Implement batched compressing and load/save to sd card

#include "compress_test_harness.h"

static icer_output_data_buf_typedef output;

// Configure source image dimensions here
#define SOURCE_IMG_WIDTH 1920
#define SOURCE_IMG_HEIGHT 1080



//#define Y_SIZE (WIDTH) * (HEIGHT)
//#define U_SIZE ((WIDTH) * (HEIGHT))
//#define V_SIZE ((WIDTH) * (HEIGHT))
//
//#define TEST_IMAGE_SIZE (Y_SIZE) + (U_SIZE) + (V_SIZE)



static uint16_t Get_Dimension_Divisor(void) {
	uint16_t divisor = 1;

	while (((SOURCE_IMG_WIDTH / divisor) * (SOURCE_IMG_HEIGHT / divisor)) > MAX_CHUNK_PIXELS)
	{
		divisor++;
	}

	return divisor;
}

/**
 * @brief Loads a rectangular image chunk from a file on the SD card.
 *
 * The image data is assumed to be stored in row-major order as uint16_t
 * pixels. The specified chunk is read row-by-row into the destination buffer.
 *
 * @param file          Pointer to the open SD card file.
 * @param chunk_row     Row index of the chunk within the image.
 * @param chunk_col     Column index of the chunk within the image.
 * @param chunk_width   Width of the chunk in pixels.
 * @param chunk_height  Height of the chunk in pixels.
 * @param buffer        Destination buffer for the loaded chunk.
 *
 * @return FX_SUCCESS on success, or a FileX error/status code on failure.
 */
uint32_t Load_Data_Chunk_From_SD(FX_FILE* file, int chunk_row, int chunk_col, uint32_t chunk_width,
		uint32_t chunk_height, uint16_t* buffer)
{
	uint32_t status = 0;
	ULONG actual;

	// Calculate starting row and column of the chunk within the full image
	uint32_t image_row_start = chunk_row * chunk_height;
	uint32_t image_col_start = chunk_col * chunk_width;

	// Calculate number of bytes for one row of pixels in the chunk
	uint32_t row_size = chunk_width * sizeof(uint16_t);

	// Read chunk one row at a time
	for (uint32_t row = 0; row < chunk_height; row++)
	{
		// Calculate current rows position within full image
		uint32_t image_row = image_row_start + row;

		// Calculate byte offset of the first pixel of this row within the image
		// image_row * SOURCE_IMG_WIDTH = number of pixels before image_row
		// + image_col_start = move x pixels into image_row
		uint32_t offset = (image_row * SOURCE_IMG_WIDTH + image_col_start) * sizeof(uint16_t);

		// Move file pointer to beginning of row we want to read
		status = fx_file_seek(file, offset);

		if (status != FX_SUCCESS) {
			return status;
		}

		// read one row of the chunk directly into the buffer
		status = fx_file_read(file, &buffer[row * chunk_width], row_size, &actual);

		if (status != FX_SUCCESS)
		{
			return status;
		}

		if (actual != row_size)
		{
			return FX_END_OF_FILE;
		}
	}
	return FX_SUCCESS;
}

/**
 * @brief Compresses a YUV image from SD card storage in independent chunks.
 *
 * The Y, U and V image planes are stored as separate files on the SD card.
 * For each chunk position, the corresponding Y, U and V regions are loaded
 * into memory and compressed independently using ICER. The compressed output
 * for each chunk can then be written back to the SD card.
 *
 * @param y_file Pointer to the open Y-plane image file.
 * @param u_file Pointer to the open U-plane image file.
 * @param v_file Pointer to the open V-plane image file.
 *
 * @return FX_SUCCESS on successful completion, a FileX status code if
 *         loading a chunk fails, or an ICER result code if initialisation
 *         or compression fails.
 */
uint32_t Test_ICER_Compress_From_SD(FX_FILE* y_file, FX_FILE* u_file, FX_FILE* v_file) {
	uint32_t status = 0;
	int icer_res = 0;
	// fx_file for the compressed output binary
	FX_FILE output_file;
	const char* compressed_filename = "compressed.bin";

	// Clears old file if exists
	status = Init_Output_File(&output_file, compressed_filename);
	if (status != FX_SUCCESS) {
	    return status;
	}
	// Represents the amount of chunks per row and column
	uint16_t divisor = Get_Dimension_Divisor();

	uint16_t chunk_height = SOURCE_IMG_HEIGHT / divisor;
	uint16_t chunk_width = SOURCE_IMG_WIDTH / divisor;

	uint8_t is_last_chunk = 0;

	// Holds the compressed output from ICER
	static uint8_t compressed_output[COMPRESSED_BUFFER_SIZE];
	static uint16_t Y[MAX_CHUNK_PIXELS];
	static uint16_t U[MAX_CHUNK_PIXELS];
	static uint16_t V[MAX_CHUNK_PIXELS];

	file_header_t file_header = {
			.divisor = divisor,
			.chunk_width = chunk_width,
			.chunk_height = chunk_height
	};

	// Store header data in output binary file
	status = SD_Stream_Data(&output_file, compressed_filename, &file_header, sizeof(file_header), 0);
	if (status != FX_SUCCESS) {
	    return status;
	}
	// Configurable ICER compression flags, requires testing for optimal output
	const int stages = 4;
	const enum icer_filter_types filt = ICER_FILTER_A;
	int segments = 10;


	// Holds data for each seperate chunk, gets written to output file
	chunk_header_t chunk_header;

	// Loop through each chunk, load data from SD card, compressed then write back to SD
	for (int row = 0; row < divisor; row++) {
		for (int col = 0; col < divisor; col++) {

			if (row == divisor - 1 && col == divisor - 1) {
				is_last_chunk = 1;
			}
			// Must call this for ICER, resets output structure each iteration
			icer_res = icer_init_output_struct(&output, compressed_output, sizeof(compressed_output), ICER_BYTE_QUOTA);
			if (icer_res != ICER_RESULT_OK) return icer_res;

			status = Load_Data_Chunk_From_SD(y_file, row, col, chunk_width, chunk_height, Y);
			if (status != FX_SUCCESS) { return status; }

			status = Load_Data_Chunk_From_SD(u_file, row, col, chunk_width, chunk_height, U);
			if (status != FX_SUCCESS) { return status; }

			status = Load_Data_Chunk_From_SD(v_file, row, col, chunk_width, chunk_height, V);
			if (status != FX_SUCCESS) { return status; }

			// Perform actual compression
			icer_res = icer_compress_image_yuv_uint16(Y, U, V, chunk_width, chunk_height, stages, filt, segments, &output);
			if (icer_res != ICER_RESULT_OK && icer_res != ICER_BYTE_QUOTA_EXCEEDED) return icer_res;

			// Encode start of chunk with chunk header
			chunk_header.chunk_start = CHUNK_START_ID;
			chunk_header.chunk_id = (row * divisor) + col;
			chunk_header.compressed_size = output.size_used;
			// Save header to output file on SD
			status = SD_Stream_Data(&output_file, compressed_filename, &chunk_header, sizeof(chunk_header), 0);
			if (status != FX_SUCCESS) {
			    return status;
			}
			//Save compressed data to output file on SD
			status = SD_Stream_Data(&output_file, compressed_filename, output.rearrange_start, output.size_used, is_last_chunk);
			if (status != FX_SUCCESS) {
			    return status;
			}
		}
	}
	// Close output .bin file
//	fx_file_close(&output_file);

	return status;
}

uint32_t Benchmark_Test_Harness_Compress_From_SD(void) {
	uint32_t status = 0;

	FX_FILE  y_file;
	FX_FILE  u_file;
	FX_FILE  v_file;

	char * filenames[] = {"Y_1920x1080.bin", "U_1920x1080.bin", "V_1920x1080.bin"};
	// Open uncompressed Y  U and V files

	status = SD_Open_YUV_Files(filenames, &y_file, &u_file, &v_file);
	if (status != FX_SUCCESS) {
		return status;
	}

	// Start benchmark
	status = Test_ICER_Compress_From_SD(&y_file, &u_file, &v_file);
	// End benchmark

	fx_file_close(&y_file);
	fx_file_close(&u_file);
	fx_file_close(&v_file);
	return status;

}





//void Test_ICER_Compress_From_YUV_File_SD(void) {
//    uint32_t status;
//    uint8_t icer_res;
//    //static uint8_t rx_buf[TEST_IMAGE_SIZE];
//
//    uSD_Test();
//
//    static uint16_t Y[Y_SIZE];
//    static uint16_t U[U_SIZE];
//    static uint16_t V[V_SIZE];
//
//
//
//    // When extracting the compressed output, the data starts at output.rearrange_start and is of size output.size_used
//    static uint8_t compressed_output[COMPRESSED_BUFFER_SIZE];
//
//
//     // Can configure these for different settings, Look at documentation for the meaning behind these variables
//    const int stages = 4;
//    const enum icer_filter_types filt = ICER_FILTER_A;
//    int segments = 10;
//
//    icer_res = icer_init_output_struct(&output, compressed_output, sizeof(compressed_output), ICER_BYTE_QUOTA);
//    if (icer_res != ICER_RESULT_OK) return;
//
//    icer_res = icer_compress_image_yuv_uint16(Y, U, V, WIDTH, HEIGHT, stages, filt, segments, &output);
//    if (icer_res != ICER_RESULT_OK && icer_res != ICER_BYTE_QUOTA_EXCEEDED) return;
//
////    if (output.size_used > 0) {
////        status = SD_Stream_Data((char *)"compress_640_480.bin", output.data_start, (uint32_t)output.size_used, 1);
////        if (status != FX_SUCCESS) return;
////    }
//
//    fx_media_flush(&sd_disk);
//    fx_media_close(&sd_disk);
//}
