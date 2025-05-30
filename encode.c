#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include"common.h"
/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_failure)
    {
        printf("INFO : OPEN FILE FUNCTION IS FAILED !!!!! \n\n");
        return e_failure;
    }
    printf("INFO : OPEN FILE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(check_capacity(encInfo)==e_failure)
    {
        printf("INFO : CHECK CAPACITY FUCTION IS FAILED !!!!!\n\n");
        return e_failure;
    }
    printf("INFO : CHECK CAPACITY FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        printf("INFO : COPY BMP HEADER FUNCTION IS FAILED !!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : COPY BMP HEADER FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
    {
        printf("INFO : ENCODE MAGIC STRING FUNCTION IS FAILED !!!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : ENCODE MAGIC STRING FUNCTION IS SUCCESSFULLY COMPLETED\n\n");

    if(encode_secret_file_extn_size(encInfo)==e_failure)
    {
        printf("INFO : ENCODE SECRET FILE EXTN SIZE FUNCTION IS FAILED !!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : ENCODE SECRET FILE EXTN SIZE FUNCTION IS SUCCESSFULLY COMPLETED\n\n");


    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
    {
        printf("INFO : ENCODE SECRET FILE EXTN FUNCTION IS FAILED !!!!!\n\n");
        return e_failure;
    }
    printf("INFO : ENCODE SECRET FILE EXTN FUNCTION IS SUCESSFULLY COMPLECTED\n\n");
    if(encode_secret_file_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
    {
        printf("INFO : ENCODE SECRET FILE SIZE FUNCTION IS FAILED !!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : ENCODE SECRET FILE SIZE FUNCTION IS SUCESSFULLY COMPLECTED\n\n");
    
    if(encode_secret_file_data(encInfo)==e_failure)
    {
        printf("INFO : ENCODE SECRET FILE DATA FUNCTION IS FAILED !!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : ENCODE SECRET FILE DATA FUNCTION IS SUCESSFULLY COMPLETED\n\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        printf("INFO : COPY REMAINING IMAGE DATA FUNCTION IS FAILED !!!!!!\n\n");
        return e_failure;
    }
    printf("INFO : COPY REMAINING IMAGE DATA FUNCTION IS SUCESSFULLY COMPLETED\n\n");
    return e_success;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("WIDTH = %u\n\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("HEIGHT = %u\n\n", height);

    // Return image capacity
    return width * height * 3;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR :UNABLE TO OPEN THE FILE %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	
        return e_failure;
    }
    
    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    int extn_size=strlen(encInfo->extn_secret_file);
    printf("SECRET FILE EXTN_SIZE : %d\n\n",extn_size);

    uint secret_size=get_file_size(encInfo->fptr_secret);
    printf("SECRET FILE SIZE : %u\n\n",secret_size);
    
    uint source_size=get_image_size_for_bmp(encInfo->fptr_src_image);

    long int capacity=(strlen(MAGIC_STRING)*8)+32+(extn_size*8)+32+(secret_size*8)+52;

    if(capacity<=source_size)
    {
        return e_success;
    }
    return e_failure;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint ret=ftell(fptr);
    rewind(fptr);
    return ret;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str3[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    fread(str3,sizeof(char),54,fptr_src_image);

    fwrite(str3,sizeof(char),54,fptr_dest_image);

    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int ret=encode_data_to_image((char*)magic_string,strlen(magic_string), encInfo ->fptr_src_image,encInfo ->fptr_stego_image);
    if(ret==e_failure)
    {
        //printf("ERROR : FAILED IN ENCODING DATA TO IMAGE");
        return e_failure;
    }
    return e_success;
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for(int i=0;i<size;i++)
    {
        fread(arr,sizeof(char),8,fptr_src_image);
        int ret =encode_byte_to_lsb(data[i],arr);
        if(ret==e_failure)
        {
            //printf("ERROR : FAILED IN ENCODE BYTE TO LSB");
            return e_failure;
        }
        fwrite(arr,sizeof(char),8,fptr_stego_image);
    }
    //printf("SUCCESSFULLY ENCODED DATA TO IMAGE");
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=7;i>=0;i--)
    {
        image_buffer[7-i]=((data>>i) & (0x01)) | ((image_buffer[7-i] )& (0xFE));
    }
    return e_success;
}
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    if((encode_size(strlen(encInfo-> extn_secret_file),encInfo-> fptr_src_image,encInfo-> fptr_stego_image))==e_failure)
    {
       // printf("ENCODE  SIZE FAILED!!!\n");
        return e_failure;
    }
    return e_success;
}
Status encode_size(int data,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr2[32];
    fread(arr2,1,32,fptr_src_image);
    for(int i=31;i>=0;i--)
    { 
        arr2[31-i]=((data>>i) & (0x01)) | ((arr2[31-i] )& (0xFE));
    }
    fwrite(arr2,1,32,fptr_stego_image);
}
Status encode_secret_file_extn(const char *extn_secret_file, EncodeInfo *encInfo)
{
    if(encode_data_to_image((char*)encInfo-> extn_secret_file,strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        //printf("ENCODE_SECRET_FILE_EXTN FAILED\n");
        return e_failure;
    }
    return e_success;
    
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encInfo->size_secret_file = get_file_size(encInfo ->fptr_secret);
    if(encode_size(encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        //printf("ERROR : ENCODEING SECRET FILE EXTN SIZE\n");
        return e_failure;
    }
    return e_success; 
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char * arr4 = malloc(encInfo->size_secret_file+1);
   // char arr4[encInfo->size_secret_file + 1];
    fread(arr4,1,encInfo->size_secret_file,encInfo->fptr_secret);
    //puts(arr4);
    if(encode_data_to_image(arr4,encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
        //printf("ERROR : ENCODE SECRET FILE DATA\n");
        return e_failure;
    }
    return e_success;
   //fwrite(arr,sizeof(char),encInfo->size_secret_file,encInfo->fptr_stego_image);
    
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch; 
    while(fread(&ch,sizeof(char),1,fptr_src)==1)
    {
        fwrite(&ch,sizeof(char),1,fptr_dest);
    }
    return e_success;
}