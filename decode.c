
#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include"common.h"

   // Function Definitions 

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_image_file_d(decInfo)==e_failure)
    {
        printf("INFO : OPEN FILE FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : OPEN FILE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");
    
    if(decode_magic_string(decInfo)==e_failure)
    {
        printf("INFO : DECODE MAGIC STRING FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : DECODE MAGIC STRING FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(decode_secret_file_extn_size(decInfo)==e_failure)
    {
        printf("INFO : DECODE SECRET FILE EXTEN SIZE FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : DECODE SECRET FILE EXTEN SIZE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(decode_secret_file_extn(decInfo)==e_failure)
    {
        printf("INFO : DECODE SECRET FILE EXTEN FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : DECODE SECRET FILE EXTRN FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(decode_secret_file_size(decInfo)==e_failure)
    {
        printf("INFO : DECODE SECRET FILE SIZE FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : DECODE SECRET FILE SIZE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(decode_secret_message(decInfo)==e_failure)
    {
        printf("INFO : SECRET MESSAGE FUNCTION IS FAILED !!!!! \n\n");
    }
    printf("INFO : DECODE SECRET MESSAGE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    return e_success;
    
}
Status open_image_file_d(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_file = fopen(decInfo->src_img,"rb");
    // Do Error handling
    if (decInfo->fptr_stego_file == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_img);

    	return e_failure;
    }
    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_file, 54, SEEK_SET);

    int magic_string_length = strlen(MAGIC_STRING);
    decInfo->magic_data = malloc(magic_string_length + 1);

    if (decInfo->magic_data == NULL)
    {
        fprintf(stderr, "ERROR: Memory allocation for magic_data failed.\n");
        return e_failure;
    }

   if (decode_data_from_image(decInfo->magic_data, magic_string_length, decInfo->fptr_stego_file) == e_failure)
    {
        fprintf(stderr, "ERROR: Failed to decode data for magic string.\n");
        free(decInfo->magic_data);
        return e_failure;
    }

    //printf("INFO: Decoded Magic String: %s\n", decInfo->magic_data);

    if(strcmp(decInfo->magic_data,MAGIC_STRING)== 0)
    {
        free(decInfo->magic_data);
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_file) 
{

    char str[8];
    for(int i=0;i<size;i++)
    {
        fread(str,8,sizeof(char),fptr_stego_file);
        decode_byte_from_lsb(&data[i],str);
    }
    return e_success;
}
Status decode_byte_from_lsb(char *data,char *image_buffer)
{
    char arr=0;
    for(int i=0;i<8;i++)
    {
        arr = ((image_buffer[i] & 0x01) << (7 - i)| arr);
    }
    *data = arr;
    return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decInfo->extn_size=0;
    if((decode_size((&decInfo->extn_size),decInfo))==e_failure)
    {
        printf("decodeing exten size faile\n");
        return e_failure;
    }
    //printf("exten size sucess\n");
    return e_success;
}
Status decode_size(long *data,DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr,sizeof(char),32,decInfo->fptr_stego_file);
    for(int i=0;i<32;i++)
    {
        *data=(((arr[i]&(0x01))<<(31-i))|*data);
    }
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decInfo->extn_name=calloc(sizeof(char),(decInfo->extn_size+1));
    if((decode_data_from_image(decInfo->extn_name,decInfo->extn_size,decInfo->fptr_stego_file))==e_failure)
    {
        printf("decode extion failed \n");
        return e_failure;
    }
    strcat(decInfo->d_secret_filename,decInfo->extn_name);
    decInfo->fptr_d_secret=fopen(decInfo->d_secret_filename,"wb");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->secret_file_size=0;
    if(decode_size(&decInfo->secret_file_size, decInfo) == e_failure)
    {
        printf("secret file size faiiled\n");
        return e_failure;
    }
    return e_success;
}

Status decode_secret_message(DecodeInfo *decInfo)
{
    decInfo->secret_data=calloc(sizeof(char),((decInfo->secret_file_size)+1));
    if (decode_data_from_image(decInfo->secret_data, decInfo->secret_file_size, decInfo->fptr_stego_file) == e_failure)
    {
        printf("secret message failed\n");
        return e_failure;
    }
    fwrite(decInfo->secret_data, sizeof(char), decInfo->secret_file_size, decInfo->fptr_d_secret);

}
