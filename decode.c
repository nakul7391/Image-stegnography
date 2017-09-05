#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "encode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char str[10];
    int i = 0, flag = 0, j = 0;
    if (check_operation_type(argv[1]) == e_unsupported )
    {
    printf("unsupported operation -e for encode -d for decode\n");
    return e_failure;
    }

    /*store the steged file name */
    if( argv[2] != NULL )
    {
        decInfo->encoded_image_fname = argv[2];
    }
    else
    e_failure;

    /*store new filename*/
    if (argv[3] != NULL )
    {
      decInfo->user_fname = argv[3];
    }
    else
    e_failure;

    /*store magic string*/
    if( argv[4] != NULL )
    {
      decInfo->user_magic_string = argv[4];
      decInfo->user_magic_string_length = strlen(argv[4]);
    }
    else
    return e_failure;

    return e_success;
}

Status open_encoded_image_file(DecodeInfo *decInfo)
{
    /*open source image file*/
    decInfo->fptr_encoded_image = fopen(decInfo->encoded_image_fname, "r");

    if (decInfo->fptr_encoded_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->encoded_image_fname);
    	return e_failure;
    }

    fseek(decInfo->fptr_encoded_image, 54L, SEEK_CUR );

    if(ftell(decInfo->fptr_encoded_image) != 54 )
    return e_failure;
    // No failure return e_success
    return e_success;
}

Status get_check_magic_string(DecodeInfo *decInfo)
{
    unsigned char imageByte, msb, magicLen = 0, tempByte;
    int i = 0, j = 0;
    /*get magic string length*/
    while( i < 8 )
    {
      imageByte = fgetc(decInfo->fptr_encoded_image);
      msb = imageByte&0x1;
      magicLen = (magicLen | msb);
      magicLen = magicLen << 1;
      i++;
    }
    magicLen = magicLen >> 1;

    if(magicLen != decInfo->user_magic_string_length)
    return e_failure;

    char str[magicLen];
    //get magic string from image
    i = 0;
    while(i < magicLen)
    {
        j = 0;
        while(j < 8)
        {
            imageByte = fgetc(decInfo->fptr_encoded_image);
            msb = imageByte&0x1;
            tempByte = tempByte|msb;
            tempByte = tempByte<<1;
            j++;
        }
        tempByte = tempByte >>1;
        str[i] = tempByte;
        i++;
    }
    str[i] = '\0';

    if( strcmp(decInfo->user_magic_string,str) == 0)
    return e_success;
    else
    return e_failure;
}

Status get_file_extension(DecodeInfo *decInfo)
{
    unsigned char imageByte, msb, extenLen = 0, tempByte;
    int i = 0, j = 0;
    char *ptr;
    /*get extension string length*/
    while( i < 8 )
    {
    imageByte = fgetc(decInfo->fptr_encoded_image);
    msb = imageByte&0x1;
    extenLen = (extenLen | msb);
    extenLen = extenLen << 1;
    i++;
    }
    extenLen = extenLen >> 1;

    char str[extenLen];
    //get file extension
    i = 0;
    while(i < extenLen)
    {
        j = 0;
        while(j < 8)
        {
            imageByte = fgetc(decInfo->fptr_encoded_image);
            msb = imageByte&0x1;
            tempByte = tempByte|msb;
            tempByte = tempByte<<1;
            j++;
        }
        tempByte = tempByte >>1;
        str[i] = tempByte;
        i++;
    }
    str[i] = '\0';

    if( strlen(str)!=extenLen )
    return e_failure;

    /*create file name*/
    ptr = malloc(strlen(decInfo->user_fname)+strlen(str)+1);
    ptr = strcat(decInfo->user_fname,".");
    decInfo->user_fname = strcat(ptr,str);
    return e_success;
}

Status open_secret_file(DecodeInfo *decInfo)
{
    decInfo->fptr_user_file = fopen(decInfo->user_fname, "w");

    if (decInfo->fptr_user_file == NULL)
    {
    perror("fopen");
    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->user_fname);
    return e_failure;
    }

    return e_success;
}

Status get_secret_file_size(DecodeInfo *decInfo)
{
    int i = 0,j = 0;
    unsigned char byte, msb;
    unsigned long size = 0;
    while(i < 64)
    {
        byte = fgetc(decInfo->fptr_encoded_image);
        msb = byte&0x1;
        size = size | msb;
        size = size << 1;
        i++;
    }
    size = size >> 1;
    decInfo->size_user_file = size;
}

Status get_secret_data(DecodeInfo *decInfo)
{
    long i = 0, j = 0;
    unsigned char byte, msb, tempByte = 0;

    rewind(decInfo->fptr_user_file);

    while(i < decInfo->size_user_file)
    {
      j = 0;
      tempByte = 0;
      while( j < 8 )
      {
        byte = fgetc(decInfo->fptr_encoded_image);
        msb = byte&0x1;
        tempByte = tempByte | msb;
        tempByte = tempByte << 1;
        j++;
      }
      tempByte = tempByte >> 1;
      //fprintf(decInfo->fptr_user_file,"%c",tempByte);
      fputc(tempByte,decInfo->fptr_user_file);
      i++;
    }

    fseek(decInfo->fptr_user_file, 0L, SEEK_END);

    if(ftell(decInfo->fptr_user_file) == decInfo->size_user_file)
    return e_success;
    else
    return e_failure;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(get_file_extension(decInfo) == e_failure)
    {
      printf("File extension corrupted\n");
      return e_failure;
    }

    if(open_secret_file(decInfo) == e_failure )
    {
    printf("Unable to create new file\n");
    return e_failure;
    }

    if(get_secret_file_size(decInfo) == e_failure)
    {
      printf("Unable to get file size\n");
      return e_failure;
    }

    if(get_secret_data(decInfo) == e_failure)
    {
      printf("Unable to get secret data\n");
      return e_failure;
    }

    return e_success;
}

/*close the files*/
Status close_files_decode(DecodeInfo *decInfo)
{
    if( fclose(decInfo->fptr_encoded_image) == 0 && fclose(decInfo->fptr_user_file) == 0 )
    return e_success;
    else
    return e_failure;
}
