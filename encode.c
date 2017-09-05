#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"

/*check operation type*/
OperationType check_operation_type(char *str)
{
    if( strcmp(str,"-e") == 0 )
    return e_encode;
    else if( strcmp(str,"-d") == 0 )
    return e_decode;
    else
    return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char str[10];
    int i = 0, flag = 0, j = 0;
    if (check_operation_type(argv[1]) == e_unsupported )
    {
    printf("unsupported operation -e for encode -d for decode\n");
    return e_failure;
    }

    /*store the secret file name and extension*/
    if ( argv[2] != NULL )
    {
      //get the file name
      encInfo->secret_fname = argv[2];
      while ( argv[2][i] != '\0' )
      {
          if(flag == 1)
          {
          str[j] = argv[2][i];
          j++;
          }
          if( argv[2][i] == '.' )
          {
              flag++;
          }
          i++;
      }
      str[j] = '\0';
      encInfo->extn_secret_file = malloc(strlen(str));
      strcpy(encInfo->extn_secret_file,str);
    }
    else
    return e_failure;

    /*store stegno filename*/
    if (argv[3] != NULL )
    {
      encInfo->stego_image_fname = argv[3];
    }
    else
    e_failure;

    /*store source image file name*/
    if( argv[4] != NULL )
    {
      encInfo->src_image_fname = argv[4];
    }
    else
    return e_failure;

    /*store magic string and its length*/
    if (argv[5] != NULL )
    {
        encInfo->magic_string = argv[5];
        encInfo->magic_string_length = strlen(argv[5]);
    }
    else
    return e_failure;

/*  printf("TEXT file %s extnesion %s\n",encInfo->secret_fname,encInfo->extn_secret_file);
    printf("source file %s\n",encInfo->src_image_fname);
    printf("stego file %s\n",encInfo->stego_image_fname);
    printf("magic string %s %d\n",encInfo->magic_string,encInfo->magic_string_length);
*/
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    /*open source image file*/

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    /*open secret file*/
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
      perror("fopen");
      fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
      return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*determine the capacity*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

  //  printf("%ld %ld\n",encInfo->image_capacity,encInfo->size_secret_file);
    if(encInfo->image_capacity-54 > encInfo->size_secret_file * 8)
    return e_success;
    else
    return e_failure;
}

long get_image_size_for_bmp(FILE *fptr_image)
{
    fseek(fptr_image, 0L, SEEK_END );

     return ftell(fptr_image);
}

long get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0L, SEEK_END );

    return ftell(fptr_secret);
}

Status do_encoding(EncodeInfo *encInfo)
{
      if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
      {
        printf("Unable to copy the header\n");
        return e_failure;
      }

      if(encode_magic_string(encInfo) == e_failure)
      {
        printf("Unable to copy the magic string\n");
        return e_failure;
      }

      if(encode_secret_file_extn(encInfo) == e_failure)
      {
        printf("Unable to store extension\n");
        return e_failure;
      }

      if(encode_secret_file_size(encInfo) == e_failure)
      {
        printf("Unalbe to store file size\n");
        return e_failure;
      }

      if(encode_secret_file_data(encInfo) == e_failure)
      {
        printf("Unalbe to store file data\n");
        return e_failure;
      }

      if(copy_remaining_img_data(encInfo) == e_failure)
      {
        printf("Unable to store file data\n");
        return e_failure;
      }

      return e_success;
}

Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_stego_image)
{
    int i = 0;
    /*fseek with seek set will position the indicator at the begining of the files
    fseek(fptr_src_image,0L,SEEK_SET);
    similar thing can be acheieved by rewind*/
    rewind(fptr_src_image);
    //error condition
    if(ftell(fptr_src_image) != 0 && ftell(fptr_stego_image) != 0 )
    {
        printf("Error ! %s", __func__);
        return e_failure;
    }

    //copy header
    for( i = 0; i < 54 ; i++ )
    fprintf(fptr_stego_image, "%c", fgetc(fptr_src_image) );

    if(ftell(fptr_src_image) == 54 && ftell(fptr_stego_image) == 54 )
    return e_success;
    else
    return e_failure;
}

/*encode the magic string in file*/
Status encode_magic_string(EncodeInfo *encInfo)
{
    int mlen, i = 0, j = 0;
    char str[10];
    unsigned char srcImageByte, msb, strchar;

    mlen = encInfo->magic_string_length;
    strcpy(str,encInfo->magic_string);

    while(  i < 8 )
    {
        srcImageByte = fgetc(encInfo->fptr_src_image);
        //get msb
        //modify last bit of data
        msb = 0x80&(mlen<<i)?1:0;
        srcImageByte = srcImageByte&0xFE | msb;
        fprintf(encInfo->fptr_stego_image,"%c", srcImageByte );
        i++;
    }

    i = 0;
    while( str[i] != '\0' )
    {
        strchar = str[i];
        j = 0;
        while( j < 8 )
        {
            srcImageByte = fgetc(encInfo->fptr_src_image);
            msb = 0x80&(strchar<<j)?1:0;
            srcImageByte = srcImageByte&0xFE | msb;
            fprintf(encInfo->fptr_stego_image,"%c",srcImageByte);
            j++;
        }
        i++;
    }

    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
    return e_success;
    else
    return e_failure;
}

/*encode secret file extension*/
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    int extnlen, i = 0, j = 0;
    char str[10];
    unsigned char srcImageByte, msb, strchar;
    strcpy(str,encInfo->extn_secret_file);
    extnlen = strlen(encInfo->extn_secret_file);

    /*encode length of extension of secret file*/

    while(  i < 8 )
    {
        srcImageByte = fgetc(encInfo->fptr_src_image);
        //get msb
        //modify last bit of data
        msb = 0x80&(extnlen<<i)?1:0;
        srcImageByte = srcImageByte&0xFE | msb;
        fprintf(encInfo->fptr_stego_image,"%c", srcImageByte );
        i++;
    }

    /*encode exntension*/
    i = 0;
    while( str[i] != '\0' )
    {
        strchar = str[i];
        j = 0;
        while( j < 8 )
        {
            srcImageByte = fgetc(encInfo->fptr_src_image);
            msb = 0x80&(strchar<<j)?1:0;
            srcImageByte = srcImageByte&0xFE | msb;
            fprintf(encInfo->fptr_stego_image,"%c",srcImageByte);
            j++;
        }
        i++;
    }

    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
    return e_success;
    else
    return e_failure;
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    int i = 0;
    unsigned char srcImageByte,msb;
    while(  i < 64 )
    {
      srcImageByte = fgetc(encInfo->fptr_src_image);
      //get msb
      //modify last bit of data
      msb = 0x8000000000000000&(encInfo->size_secret_file<<i)?1:0;
      srcImageByte = srcImageByte&0xFE | msb;
      fprintf(encInfo->fptr_stego_image,"%c", srcImageByte );
      i++;
    }

    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
    return e_success;
    else
    return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    long size = encInfo->size_secret_file;
    int i;
    unsigned char srcImageByte, secretByte, msb;
    //unsigned int srcImageByte;
    /*position the pointer to start*/

    rewind(encInfo->fptr_secret);

    while(size--)
    {
        i = 0;
        secretByte = fgetc(encInfo->fptr_secret);
        while(i < 8 )
        {
          srcImageByte = fgetc(encInfo->fptr_src_image);
          //get msb
          //modify last bit of data
          msb = 0x80&(secretByte<<i)?1:0;
          srcImageByte = srcImageByte&0xFE | msb;
        //  fprintf(encInfo->fptr_stego_image,"%c", srcImageByte );
        fputc(srcImageByte,encInfo->fptr_stego_image);
          i++;
        }
    }

    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
    return e_success;
    else
    return e_failure;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{

    unsigned char c;

    while( ftell(encInfo->fptr_src_image) < encInfo->image_capacity )
    {
        c = fgetc(encInfo->fptr_src_image);
        fprintf(encInfo->fptr_stego_image,"%c",c);
    }

    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
    return e_success;
    else
    return e_failure;

}

/*close the files*/
Status close_files(EncodeInfo *encInfo)
{
    if( fclose(encInfo->fptr_secret) == 0 && fclose(encInfo->fptr_src_image) == 0 && fclose(encInfo->fptr_stego_image) ==0 )
    return e_success;
    else
    return e_failure;
}
