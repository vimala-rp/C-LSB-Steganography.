#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Identify the operation selected through the command-line argument */
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

int main(int argc, char *argv[])
{
    OperationType res;

    if (argc < 2)
    {
        printf("ERROR: Insufficient arguments\n");
        return 1;
    }

    /* Determine whether the user wants to encode or decode */
    res = check_operation_type(argv);

    /* Validate encoding arguments and perform the encoding operation */
    if (res == e_encode)
    {
        EncodeInfo encInfo;

        if (argc < 4 || argc > 5)
        {
            printf("ERROR: Usage: %s -e <source.bmp> <secret_file> [output.bmp]\n", argv[0]);
            return 1;
        }

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            if (do_encoding(&encInfo) != e_success)
            {
                printf("Encoding failed\n");
                return 1;
            }
        }
        else
        {
            printf("Invalid encoding arguments\n");
            return 1;
        }
    }

    /* Validate decoding arguments and perform the decoding operation */
    else if (res == e_decode)
    {
        DecodeInfo decInfo;

        if (argc < 3 || argc > 4)
        {
            printf("ERROR: Usage: %s -d <stego.bmp> [output_file]\n", argv[0]);
            return 1;
        }

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            if (do_decoding(&decInfo) != e_success)
            {
                printf("Decoding failed\n");
                return 1;
            }
        }
        else
        {
            printf("Invalid decoding arguments\n");
            return 1;
        }
    }
    else
    {
        printf("Unsupported operation\n");
        return 1;
    }

    return 0;
}