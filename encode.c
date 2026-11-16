#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    /* Move to the location where the BMP width is stored */
    if (fseek(fptr_image, 18, SEEK_SET) != 0)
    {
        printf("ERROR: Unable to seek to BMP dimensions\n");
        return 0;
    }

    /* Read the image width */
    if (fread(&width, sizeof(width), 1, fptr_image) != 1)
    {
        printf("ERROR: Unable to read BMP width\n");
        return 0;
    }

    printf("width = %u\n", width);

    /* Read the image height */
    if (fread(&height, sizeof(height), 1, fptr_image) != 1)
    {
        printf("ERROR: Unable to read BMP height\n");
        return 0;
    }

    printf("height = %u\n", height);

    /* Return the image capacity in bytes */
    return width * height * 3;
}

/* Open required files
 * Input: EncodeInfo structure pointer
 * Output: e_success or e_failure
 * Description: Opens the source image, secret file, and stego image.
 *              Closes previously opened files if a later file fails to open.
 */
Status open_files(EncodeInfo *encInfo)
{
    // Open source image in binary read mode
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->src_image_fname);
        return e_failure;
    }

    // Open secret file in binary read mode
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");

    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->secret_fname);

        fclose(encInfo->fptr_src_image);
        encInfo->fptr_src_image = NULL;

        return e_failure;
    }

    // Input files opened successfully
    return e_success;
}
/*
 * Read and validate encoding command-line arguments
 * Input: Command-line arguments and EncodeInfo structure
 * Output: Valid file names and secret file extension stored in EncodeInfo
 * Return: e_success if arguments are valid,
 * otherwise e_failure
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *dot;

    // Validate source image filename
    dot = strrchr(argv[2], '.');

    if (dot != NULL && strcmp(dot, ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Source image should be a BMP file\n");
        return e_failure;
    }

    // Validate secret file extension
    dot = strrchr(argv[3], '.');

    if (dot != NULL &&
    dot != argv[3] &&
    dot[1] != '\0' &&
    strlen(dot) <= MAX_FILE_SUFFIX)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, dot);
    }
    else
    {
        printf("ERROR: Invalid secret file extension\n");
        return e_failure;
    }

    // Validate optional stego image filename
    if (argv[4] != NULL)
    {
        dot = strrchr(argv[4], '.');

        if (dot != NULL && strcmp(dot, ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("ERROR: Stego image should be a BMP file\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

/*
 * Check whether the source image has enough capacity
 * Input: EncodeInfo structure containing image and secret file information
 * Output: Secret file size is calculated and stored in EncodeInfo
 * Return: e_success if the image can hold the secret data,
 *         e_failure if the image capacity is insufficient
 */
Status check_capacity(EncodeInfo *encInfo)
{
    // Find the size of the secret file
    if (fseek(encInfo->fptr_secret, 0, SEEK_END) != 0)
    {
        printf("ERROR: Failed to seek secret file\n");
        return e_failure;
    }

    encInfo->size_secret_file = ftell(encInfo->fptr_secret);

    if (encInfo->size_secret_file < 0)
    {
        printf("ERROR: Failed to determine secret file size\n");
        return e_failure;
    }

    rewind(encInfo->fptr_secret);

    // Calculate the total size of information to be encoded
    long size_of_info = strlen(MAGIC_STRING) +
                        sizeof(int) +
                        strlen(encInfo->extn_secret_file) +
                        sizeof(int) +
                        encInfo->size_secret_file;

    // Get the source image capacity
    long image_size = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Check whether the image can hold all the secret information
    if (image_size >= (size_of_info * 8))
    {
        return e_success;
    }

    return e_failure;
}
/*
 * Copy the BMP header from source image to stego image
 * Input: Source image and destination image file pointers
 * Output: 54-byte BMP header copied to the destination image
 * Return: e_success if copying succeeds, otherwise e_failure
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];

    if (fread(arr, 54, 1, fptr_src_image) != 1)
    {
        printf("ERROR: Failed to read BMP header\n");
        return e_failure;
    }

    if (fwrite(arr, 54, 1, fptr_dest_image) != 1)
    {
        printf("ERROR: Failed to copy BMP header\n");
        return e_failure;
    }

    printf("BMP header copied successfully\n");
    return e_success;
}

/*
 * Encode the magic string into the image
 * Input: Magic string and EncodeInfo structure
 * Output: Magic string encoded into the image using LSB technique
 * Return: e_success after encoding the magic string
 */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if (encode_string_to_image(magic_string,
                               strlen(magic_string),
                               encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("Magic string encoded successfully\n");

    return e_success;
}

/*
 * Perform the complete encoding process
 * Input: EncodeInfo structure containing all encoding information
 * Output: Stego image containing the hidden secret file
 * Return: e_success if encoding is completed successfully,
 *         e_failure if any required operation fails
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Open source image and secret file
    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: Unable to open input files\n");
        return e_failure;
    }

    printf("Input files opened successfully\n");

    // Check capacity before creating the output file
    if (check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Insufficient image capacity\n");
        goto cleanup;
    }

    printf("Image capacity verified successfully\n");

    // Open output image only after capacity is verified
    encInfo->fptr_stego_image =
        fopen(encInfo->stego_image_fname, "wb");

    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->stego_image_fname);
        goto cleanup;
    }

    // Start reading the source image from the beginning
    rewind(encInfo->fptr_src_image);

    // Copy the 54-byte BMP header
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        goto cleanup;
    }

    // Encode the magic string
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        goto cleanup;
    }

    // Encode the secret file extension
    if (encode_secret_file_extn(encInfo->extn_secret_file,
                                encInfo) == e_failure)
    {
        goto cleanup;
    }

    // Encode the secret file size
    if (encode_secret_file_size(encInfo->size_secret_file,
                                encInfo) == e_failure)
    {
        goto cleanup;
    }

    // Encode the secret file contents
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        goto cleanup;
    }

    // Copy the remaining image data
    if (copy_remaining_img_data(encInfo->fptr_src_image,
                                encInfo->fptr_stego_image) == e_failure)
    {
        goto cleanup;
    }

    printf("Encoding completed successfully\n");

    // Close all files
    fclose(encInfo->fptr_src_image);
    encInfo->fptr_src_image = NULL;

    fclose(encInfo->fptr_secret);
    encInfo->fptr_secret = NULL;

    if (fclose(encInfo->fptr_stego_image) != 0)
    {
        encInfo->fptr_stego_image = NULL;
        printf("ERROR: Failed to close output image\n");
        return e_failure;
    }

    encInfo->fptr_stego_image = NULL;

    return e_success;

cleanup:
    if (encInfo->fptr_src_image != NULL)
    {
        fclose(encInfo->fptr_src_image);
        encInfo->fptr_src_image = NULL;
    }

    if (encInfo->fptr_secret != NULL)
    {
        fclose(encInfo->fptr_secret);
        encInfo->fptr_secret = NULL;
    }

    if (encInfo->fptr_stego_image != NULL)
    {
        fclose(encInfo->fptr_stego_image);
        encInfo->fptr_stego_image = NULL;
    }

    return e_failure;
}
/*
 * Encode a string into the LSBs of image data
 * Input: String, string length, and EncodeInfo structure
 * Output: String encoded into the image using the LSB technique
 * Description: Each character is encoded into 8 consecutive image bytes.
 */
Status encode_string_to_image(char *str, int size, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        unsigned char arr[8];

        if (fread(arr, 1, 8, encInfo->fptr_src_image) != 8)
        {
            printf("ERROR: Failed to read image bytes while encoding string\n");
            return e_failure;
        }

        for (int j = 0; j < 8; j++)
        {
            unsigned char bit = ((unsigned char)str[i] >> j) & 1;
            arr[j] = (arr[j] & 0xFE) | bit;
        }

        if (fwrite(arr, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Failed to write encoded string to stego image\n");
            return e_failure;
        }
    }

    return e_success;
}

/*
 * Encode an integer into the LSBs of image data
 * Input: Integer data and source and destination image file pointers
 * Output: The 32 bits of the integer are encoded into 32 image bytes
 * Description: Each bit of the integer is stored in the LSB of one image byte.
 */
Status encode_int_to_lsb(int data, FILE *fptr_src, FILE *fptr_stego)
{
    unsigned char arr[32];

    if (fread(arr, 1, 32, fptr_src) != 32)
    {
        printf("ERROR: Failed to read image bytes for integer encoding\n");
        return e_failure;
    }

    for (int i = 0; i < 32; i++)
    {
        unsigned char bit = (data >> i) & 1;
        arr[i] = (arr[i] & 0xFE) | bit;
    }

    if (fwrite(arr, 1, 32, fptr_stego) != 32)
    {
        printf("ERROR: Failed to write encoded integer to stego image\n");
        return e_failure;
    }

    return e_success;
}
/*
 * Encode the secret file extension
 * Input: Secret file extension and EncodeInfo structure
 * Output: Extension length and extension data encoded into the image
 * Description: The extension length is encoded first, followed by
 * the extension characters.
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_int_to_lsb((int)strlen(file_extn),
                          encInfo->fptr_src_image,
                          encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if (encode_string_to_image((char *)file_extn,
                               (int)strlen(file_extn),
                               encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("Secret file extension encoded successfully\n");

    return e_success;
}
/*
 * Encode the secret file size
 * Input: Secret file size and EncodeInfo structure
 * Output: Secret file size encoded into the image using LSB technique
 * Return: e_success if encoding succeeds, otherwise e_failure
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    if (encode_int_to_lsb((int)file_size,
                          encInfo->fptr_src_image,
                          encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to encode secret file size\n");
        return e_failure;
    }

    printf("Secret file size encoded successfully\n");

    return e_success;
}

/*
 * Encode the secret file data
 * Input: EncodeInfo structure containing the secret file pointer
 * Output: Complete secret file content encoded into the image
 * Description: The secret file is read one byte at a time and
 * each byte is encoded using the LSB technique.
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    rewind(encInfo->fptr_secret);

    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        if (fread(&ch, 1, 1, encInfo->fptr_secret) != 1)
        {
            printf("ERROR: Failed to read secret file data\n");
            return e_failure;
        }

        if (encode_string_to_image(&ch, 1, encInfo) == e_failure)
        {
            return e_failure;
        }
    }

    printf("Secret file data encoded successfully\n");

    return e_success;
}
/*
 * Copy the remaining image data
 * Input: Source image and destination image file pointers
 * Output: Remaining image data copied unchanged
 * Description: After all secret information is encoded, the
 * remaining bytes of the source image are copied directly.
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != 1)
        {
            printf("ERROR: Failed to copy remaining image data\n");
            return e_failure;
        }
    }

    if (ferror(fptr_src))
    {
        printf("ERROR: Failed to read remaining image data\n");
        return e_failure;
    }

    printf("Remaining image data copied successfully\n");

    return e_success;
}