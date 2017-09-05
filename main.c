/*main program */
#include <stdio.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[] )
{
		EncodeInfo encInfo;
		DecodeInfo decInfo;
		/*check operation */
		if(check_operation_type(argv[1]) == e_encode)
		{
			printf("Started encoding...\n");
			if(read_and_validate_encode_args(argv,&encInfo) == e_failure )
			{
				printf("Inputs are not validated\n");
				return 1;
			}

			/*open files*/
			if(open_files(&encInfo) == e_failure)
			{
					printf("Failed to open files\n");
					return 2;
			}

			if(check_capacity(&encInfo) == e_failure)
			{
					printf("Input image file size is less than required\n");
					return 3;
			}

			if( do_encoding(&encInfo) == e_success )
			printf("Encoding successfull\n");
			else
			{
			printf("Encoding Failed\n");
			return e_failure;
			}

			printf("%s%ld%s byte written into %s%s%s\nTotal size %s%ld%s bytes\n",RED,encInfo.size_secret_file,BLACK,RED,encInfo.stego_image_fname,BLACK,RED,encInfo.image_capacity,BLACK);
			if(close_files(&encInfo) == e_failure)
			printf("Unable to close files\n");
			else
			printf("successfully closed files");
			return 4;

		}
		else if(check_operation_type(argv[1]) == e_decode)
		{
			printf("Started Decoding...\n");
			if(read_and_validate_decode_args(argv,&decInfo) == e_failure )
			{
				printf("Inputs are not validated\n");
				return 1;
			}

			/*open files*/
			if(open_encoded_image_file(&decInfo) == e_failure)
			{
					printf("Failed to open files\n");
					return 2;
			}

			if(get_check_magic_string(&decInfo) == e_failure)
			{
					printf("Wrong magic string\n");
					return 2;
			}
			else
			printf("%sMagic string Accepted%s\n",RED,BLACK);

			if(do_decoding(&decInfo) == e_failure)
			{
					printf("Decoding Failed\n");
					return 2;
			}
			else
			{
				printf("Decoding successfull\n");
			}

			printf("%s%ld%s bytes written into %s%s%s\n",RED,decInfo.size_user_file,BLACK,RED,decInfo.user_fname,BLACK);

			if(close_files_decode(&decInfo) == e_failure)
			{
					printf("Unable to close files\n");
					return 2;
			}
			else
			printf("Successfully closed files\n");

		}

		return 0;
}
