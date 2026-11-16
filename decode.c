#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "decode.h"
#include "common.h"

/* Function Definitions */

/*
 * Read and validate the command-line arguments required for decoding.
 * Store the stego image file name and output secret file name.
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /* Check whether the stego image file name is provided */
    if (argv[2] == NULL)
    {
        printf("ERROR: Missing stego image file\n");
        return e_failure;
    }

    /* Validate that the filename ends with .bmp */
    int len = strlen(argv[2]);

    if (len < 4 ||
        tolower((unsigned char)argv[2][len - 4]) != '.' ||
        tolower((unsigned char)argv[2][len - 3]) != 'b' ||
        tolower((unsigned char)argv[2][len - 2]) != 'm' ||
        tolower((unsigned char)argv[2][len - 1]) != 'p')
    {
        printf("ERROR: Stego image should be a BMP file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    /* Initialize file pointers */
    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_secret = NULL;

    /* Initialize extension size */
    decInfo->extn_size = 0;

    /* Initialize output filename */
    decInfo->output_fname[0] = '\0';

    /* Store the output filename and separate its extension */
    if (argv[3] != NULL)
    {
        if (strlen(argv[3]) >= sizeof(decInfo->secret_fname))
        {
            printf("ERROR: Output filename is too long\n");
            return e_failure;
        }

        strcpy(decInfo->secret_fname, argv[3]);

        /* Separate the filename from its extension using strtok */
        char *token = strtok(decInfo->secret_fname, ".");

        if (token == NULL || token[0] == '\0')
        {
            printf("ERROR: Invalid output filename\n");
            return e_failure;
        }
    }
    else
    {
        /* Use output as the default filename */
        strcpy(decInfo->secret_fname, "output");
    }

    return e_success;
}


/*
 * Open the stego image for reading and create the secret file
 * for writing the decoded data.
 *
 * This function is retained for the project interface.
 * The output file must be opened only after its extension is decoded.
 */
Status open_decode_files(DecodeInfo *decInfo)
{
    /* Open the stego image in binary read mode */
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    printf("Stego image opened successfully\n");

    return e_success;
}


/*
 * Skip the 54-byte BMP header to reach the image pixel data.
 * Input: Pointer to the stego image file.
 * Return: e_success if the header is skipped successfully,
 *         otherwise e_failure.
 */
Status skip_54_bytes_header(FILE *fptr_stego_image)
{
    /* Move the file position past the 54-byte BMP header */
    if (fseek(fptr_stego_image, 54, SEEK_SET) != 0)
    {
        printf("ERROR: Unable to skip BMP header\n");
        return e_failure;
    }

    return e_success;
}


/*
 * Decode the magic string from the stego image
 * and compare it with the expected magic string.
 */
Status decode_magic_string(const char *magic_string,
                           DecodeInfo *decInfo)
{
    size_t length = strlen(magic_string);
    char data[length + 1];

    /* Decode the magic string from the image */
    if (decode_string_from_image(data, (int)length,
                                 decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode magic string\n");
        return e_failure;
    }

    /* Add null character to terminate the string */
    data[length] = '\0';

    /* Compare the decoded string with the expected magic string */
    if (strcmp(data, magic_string) == 0)
    {
        printf("Magic string verified successfully\n");
        return e_success;
    }

    printf("ERROR: Magic string does not match\n");
    return e_failure;
}


/*
 * Decode the size of the secret file extension
 * from the stego image.
 */
Status decode_size_exten(DecodeInfo *decInfo)
{
    int extn_size;

    /* Decode the extension size from the image */
    if (decode_int_from_lsb(&extn_size,
                            decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode extension size\n");
        return e_failure;
    }

    /* Validate the extension size */
    if (extn_size <= 0 || extn_size > MAX_FILE_SUFFIX)
    {
        printf("ERROR: Invalid extension size\n");
        return e_failure;
    }

    /* Store the decoded extension size */
    decInfo->extn_size = extn_size;

    printf("Secret file extension size decoded successfully\n");

    return e_success;
}


/*
 * Decode the secret file extension from the stego image.
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    /* Decode the extension characters */
    if (decode_string_from_image(decInfo->extn_secret_file,
                                 decInfo->extn_size,
                                 decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode secret file extension\n");
        return e_failure;
    }

    /* Terminate the extension string */
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';

    printf("Secret file extension decoded successfully\n");

    return e_success;
}


/*
 * Decode a string from the LSBs of the stego image.
 * Input: Destination buffer, string length, and stego image file pointer.
 * Return: e_success if the string is decoded successfully,
 *         otherwise e_failure.
 */
Status decode_string_from_image(char *data, int size,
                                FILE *fptr_stego_image)
{
    /* Read and decode each character from the image */
    for (int i = 0; i < size; i++)
    {
        unsigned char arr[8];
        unsigned char ch = 0;

        /* Read 8 bytes to reconstruct one character */
        if (fread(arr, 1, 8, fptr_stego_image) != 8)
        {
            return e_failure;
        }

        /* Extract one LSB from each byte */
        for (int j = 0; j < 8; j++)
        {
            unsigned char bit = arr[j] & 1U;
            ch |= bit << j;
        }

        data[i] = (char)ch;
    }

    return e_success;
}


/*
 * Decode an integer from the LSBs of the stego image.
 */
Status decode_int_from_lsb(int *data, FILE *fptr_stego_image)
{
    unsigned char arr[32];
    unsigned int value = 0;

    /* Read 32 bytes from the stego image */
    if (fread(arr, 1, 32, fptr_stego_image) != 32)
    {
        return e_failure;
    }

    /* Extract one LSB from each byte */
    for (int i = 0; i < 32; i++)
    {
        unsigned int bit = arr[i] & 1U;
        value |= bit << i;
    }

    /* Store the decoded integer */
    *data = (int)value;

    return e_success;
}


/*
 * Decode the size of the secret file from the stego image.
 */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    int size;

    /* Decode the secret file size using the helper function */
    if (decode_int_from_lsb(&size,
                            decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode secret file size\n");
        return e_failure;
    }

    /* Check whether the decoded size is valid */
    if (size < 0)
    {
        printf("ERROR: Invalid secret file size\n");
        return e_failure;
    }

    /* Store the decoded size in DecodeInfo */
    decInfo->size_secret_file = size;

    printf("Secret file size decoded successfully\n");

    return e_success;
}


/*
 * Decode the actual secret file data from the stego image
 * and write it into the output secret file.
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;

    /* Decode each byte of the secret file */
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        /* Decode one byte from the stego image */
        if (decode_string_from_image(&ch, 1,
                                     decInfo->fptr_stego_image) == e_failure)
        {
            printf("ERROR: Unable to decode secret file data\n");
            return e_failure;
        }

        /* Write the decoded byte into the output file */
        if (fwrite(&ch, 1, 1, decInfo->fptr_secret) != 1)
        {
            printf("ERROR: Unable to write secret file data\n");
            return e_failure;
        }
    }

    return e_success;
}


/*
 * Perform the complete decoding process.
 */
Status do_decoding(DecodeInfo *decInfo)
{
    /* Open the stego image */
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    /* Skip the 54-byte BMP header */
    if (skip_54_bytes_header(decInfo->fptr_stego_image) == e_failure)
    {
        goto decode_failure;
    }

    /* Decode and verify the magic string */
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        goto decode_failure;
    }

    /* Decode the size of the secret file extension */
    if (decode_size_exten(decInfo) == e_failure)
    {
        goto decode_failure;
    }

    /* Decode the secret file extension */
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        goto decode_failure;
    }

    /* Construct the output filename using the decoded extension */
    decInfo->output_fname[0] = '\0';

    if (snprintf(decInfo->output_fname,
                 sizeof(decInfo->output_fname),
                 "%s%s",
                 decInfo->secret_fname,
                 decInfo->extn_secret_file) >=
        (int)sizeof(decInfo->output_fname))
    {
        printf("ERROR: Output filename is too long\n");
        goto decode_failure;
    }

    /* Open the output file after constructing its filename */
    decInfo->fptr_secret = fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_secret == NULL)
    {
        printf("ERROR: Unable to create secret output file\n");
        goto decode_failure;
    }

    printf("Files opened successfully\n");

    /* Decode the size of the secret file */
    if (decode_secret_file_size(decInfo) == e_failure)
    {
        goto decode_failure;
    }

    /* Decode the actual secret file data */
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        goto decode_failure;
    }

    /* Close both files after decoding is complete */
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_secret);
    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_secret = NULL;

    printf("Decoding completed successfully\n");

    return e_success;

decode_failure:

    /* Close any files that are still open after an error */
    if (decInfo->fptr_stego_image != NULL)
    {
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
    }

    if (decInfo->fptr_secret != NULL)
    {
        fclose(decInfo->fptr_secret);
        decInfo->fptr_secret = NULL;
    }

    return e_failure;
}

