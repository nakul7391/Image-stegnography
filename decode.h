#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/* to store encoding information */
typedef struct _DecodeInfo
{
    /* Source Image info */
    char *encoded_image_fname;
    FILE *fptr_encoded_image;
    long encoded_image_capacity;

    /* Secret File Info */
    char *user_fname;
    FILE *fptr_user_file;
    char *extn_user_file;
    long size_user_file;

    /*magic string*/
    char *user_magic_string;
    int user_magic_string_length;
} DecodeInfo;

/*read decode arguments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*open the encoded image file*/
Status open_encoded_image_file(DecodeInfo *decInfo);

/*get magic string length and magic string*/
Status get_check_magic_string(DecodeInfo *decInfo);

/*get secret file extension*/
Status get_file_extension(DecodeInfo *decInfo);

/*open secret file*/
Status open_secret_file(DecodeInfo *decInfo);

/*get secret file size*/
Status get_secret_file_size(DecodeInfo *decInfo);

/*get secret file data*/
Status get_secret_data(DecodeInfo *decInfo);

/*do decoding*/
Status do_decoding(DecodeInfo *decInfo);

/*close files*/
Status close_files_decode(DecodeInfo *decInfo);

#endif
