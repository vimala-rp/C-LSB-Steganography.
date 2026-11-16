#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"
#include "encode.h"

typedef struct _DecodeInfo
{
/* Stego Image Info */
char *stego_image_fname;
FILE *fptr_stego_image;

/* Secret File Info */
char secret_fname[256];
char extn_secret_file[MAX_FILE_SUFFIX + 1];
char output_fname[261];
FILE *fptr_secret;
int extn_size;
long size_secret_file;

} DecodeInfo;

/* Validate command-line arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open the stego image */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip the BMP header */
Status skip_54_bytes_header(FILE *fptr_stego_image);

/* Decode a string from image LSBs */
Status decode_string_from_image(char *data, int size,
FILE *fptr_stego_image);

/* Decode the integer stored in image LSBs */
Status decode_int_from_lsb(int *data, FILE *fptr_stego_image);

/* Decode the magic string */
Status decode_magic_string(const char *magic_string,
DecodeInfo *decInfo);

/* Decode the size of the secret file extension */
Status decode_size_exten(DecodeInfo *decInfo);

/* Decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode the secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode the actual secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Perform the complete decoding process */
Status do_decoding(DecodeInfo *decInfo);

#endif
