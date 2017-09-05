#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


/* to store encoding information */
typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    long image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char *extn_secret_file;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /*magic string*/
    char *magic_string;
    int magic_string_length;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

/*check operation type*/
OperationType check_operation_type(char *str);

/*validate the arguments*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/*return status of file operation*/
Status open_files(EncodeInfo *encInfo);

/*check the capacity of file and image*/
Status check_capacity(EncodeInfo *encInfo);

/*get image file size*/
long get_image_size_for_bmp(FILE *fptr_image);

/*get secret file size*/
long get_file_size(FILE *fptr_secret);

/*do encoding*/
Status do_encoding(EncodeInfo *encInfo);

/*copy header of bmp image file into new file*/
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_stego_image);

/*encode magic string*/
Status encode_magic_string(EncodeInfo *encInfo);

/*encode secret file extension*/
Status encode_secret_file_extn(EncodeInfo *encInfo);

/*encode secret file length*/
Status encode_secret_file_size(EncodeInfo *encInfo);

/*encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/*encode reamining data to image*/
Status copy_remaining_img_data(EncodeInfo *encInfo);
/*close the files*/
Status close_files(EncodeInfo *encInfo);

#endif
