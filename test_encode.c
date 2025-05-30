#include<stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<stdlib.h>
#include<string.h>


int main(int argc,char*argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //function calling for check operation type
    if(check_operation_type(argv) == e_encode)
    {
        printf("INFO : SELECTED ENCODING \n\n");
        if(argc >= 4)
        {
        // read and validate encode arguments
            if(read_and_validate_encode_args(argv, &encInfo)==e_failure)
            {
                printf("INFO : READ AND VALIDATE ENCODE FUCTION IS FAILED\n");
            }
            else
            {
                printf("INFO : READ AND VALIDATE ENCODE FUNCTION IS SUCCESS\n\n");
                printf("<-------------------------------ENCORDING IS STARTED------------------------>\n\n");
                if(do_encoding(&encInfo)==e_failure)
                {
                    printf("INFO : ENCORDING STOPPED!!!!!!\n\n");
                    return 0;
                }
                else
                {
                    printf("<-----------------------------ENCORDING IS COMPLETED----------------------------->\n\n");
                }
            }
        }
        else
        {
            printf("INFO : INSUFFICIENT ARGUMENTS\n");
        }
    }
    else if(check_operation_type(argv)==e_decode)
    {
        printf("INFO : SELECTED ENCODING \n\n");
        if(argc<3)
        {
            printf("INFO : READ AND VALIDATE FAILED\n\n");
            return 0;
        }
        else
        {
            
            if(read_and_validate_decode_args(argv, &decInfo)==e_success)
            {
                printf("INFO : READ AND VALIDATE DECODE FUNCTION IS SUCCESS\n\n");
                printf("<-------------------------------DECODE IS STARTED------------------------>\n\n");
                
                if(do_decoding(&decInfo)==e_success)
                {
                    printf("<-----------------------------DECODE IS COMPLETED----------------------------->\n\n");
                }
                else if(do_decoding(&decInfo)==e_failure)
                {
                    printf("ERROR:DECODING FAILED\n");
                    return 0;
                }
                
            }
            else
            {
                printf("ERROR:READ AND VALIDATE FAILED\n");
                return 0;
            }
        }
    }
    else
    {
        printf("UNSUPPOERTED\n");
    }
    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if((strcmp(argv[1],"-e"))==0)
    {
        return e_encode;
    }
    else if((strcmp(argv[1],"-d"))==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char str[2];
    FILE *fp=fopen("beautiful.bmp","rb");
    if(fp==NULL)
    {
        perror("Error opening BMP file");
        return e_failure;
    }
    fread(str,sizeof(char),2,fp);
    fclose(fp);
    if(str[0]==0x42 && str[1]==0x4d)
    {
        encInfo->src_image_fname = malloc(strlen(argv[2]+1));
        strcpy(encInfo->src_image_fname , argv[2]);
       // printf("encInfo->src_image_fname: %s\n",encInfo->src_image_fname);

        encInfo->secret_fname = malloc(strlen(argv[3]+1));
        strcpy(encInfo->secret_fname , argv[3]);
       // printf("encInfo->secret_fname: %s\n",encInfo->secret_fname);

        strcpy(encInfo->extn_secret_file,strrchr(argv[3],'.'));

        if(argv[4]!=NULL)
        {
            char *str2=strrchr(argv[4],'.');
           // printf("Extension of output file: %s\n",str2);

            if(str2 && (strcmp(str2,".bmp")==0))
            {
                encInfo->stego_image_fname = malloc(strlen(argv[4]+1));
                strcpy((encInfo->stego_image_fname),argv[4]);
               // printf("encInfo->stego_image_fname: %s\n",encInfo->stego_image_fname);
            }
            else
            {
                printf("THE EXTENSION IS NOT  .bmp \n");
                return e_failure;
            }
        }
        else
        {
            strcpy((encInfo->stego_image_fname),"stego.bmp");
            //return e_success;
        }
        return e_success;
    }
    else
    {
        printf("INFO : INPUT FILE NOT A BMP FORMAT !!!!!\n");
        return e_failure;
    }
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    FILE *fptr_stego = fopen(argv[2], "rb");
    if (fptr_stego == NULL)
    {
        perror("Error opening file");
        return e_failure;
    }
    char src_name[2];
    
    fread(src_name,sizeof(char),2,fptr_stego);
    fclose(fptr_stego);
    if(src_name[0]==0x42 && src_name[1]==0x4d)
    {
        decInfo->src_img=malloc((strlen(argv[2])+1)*sizeof(char));
        strcpy(decInfo->src_img,argv[2]);
        //printf("%s",decInfo->src_img);
        if(argv[3] != NULL)
        {
            decInfo->d_secret_filename=malloc((strlen(argv[3])+1)*sizeof(char));
            strcpy(decInfo->d_secret_filename,argv[3]);
        }
        else
        {
            decInfo->d_secret_filename=malloc(12*sizeof(char));
            strcpy(decInfo->d_secret_filename ,"secretcode");
        }
    }
    else
    {
        return e_failure;
    }
    return e_success;
}