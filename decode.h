#ifndef DECODE_H
#define DECODE_H
#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4




typedef struct _DecodeInfo
{
    //stego image info
    char *src_img;
    FILE *fptr_stego_file;

    //magic string data    
    char *magic_data;

    //secret file
    FILE *fptr_d_secret;
    char *d_secret_filename;
    long secret_file_size;
    char *secret_data;


    //secret file exten 
    long extn_size;
    char *extn_name;





}DecodeInfo;


//function
OperationType check_operation_type(char *argv[]);

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_image_file_d(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_size(long *data,DecodeInfo *decInfo);

Status decode_secret_message(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *data,char *image_buffer);


Status decode_data_from_image(char *data, int size, FILE *fptr_stego_file);

#endif
