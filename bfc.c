#include <linux/elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "bfc.h"

#define MAX_BIN_LEN 655350
#define MAX_JUMPS 1000

unsigned int org = 0x08048000;

typedef struct {
    char op;
    int times;
} rle_bf_op;

int read_program(rle_bf_op *prog){
    char input;
    int len = 0;
    while((input = getchar()) != -1){
        if(input != '+' && input != '-' &&
           input != '>' && input != '<' &&
           input != '[' && input != ']' &&
           input != ',' && input != '.'){
            continue;
        }

        if(prog->op != input){
            len++;
            if(prog->op){
                prog++;
            }
            prog->op=input;
        }
        prog->times++;
    }
    return len;
}

int main(){

    rle_bf_op input[10000];
    int input_len;

    uint8_t text[MAX_BIN_LEN];
    uint8_t *txt_ptr = text;

    int32_t *loop_jmps[MAX_JUMPS];
    int32_t **loop_jmps_ptr = loop_jmps;

    Elf32_Off entry;

    entry = org + sizeof(Elf32_Ehdr) + 1 * sizeof(Elf32_Phdr);

    input_len = read_program(input);

    memcpy(txt_ptr,prelude,sizeof(prelude));
    txt_ptr += sizeof(prelude);

    for(int i = 0; i < input_len; i++){
        switch(input[i].op){
            case '>':
                if(input[i].times==1){
                    memcpy(txt_ptr,inc_ptr,sizeof(inc_ptr));
                    txt_ptr += sizeof(inc_ptr);
                } else if (input[i].times <= 0xff) {
                    memcpy(txt_ptr,add_ptr8,sizeof(add_ptr8));
                    txt_ptr += sizeof(add_ptr8);
                    *(uint8_t*)(txt_ptr-add_ptr8_arg)=(uint8_t)input[i].times;
                } else {
                    memcpy(txt_ptr,add_ptr32,sizeof(add_ptr32));
                    txt_ptr += sizeof(add_ptr32);
                    *(uint32_t*)(txt_ptr-add_ptr32_arg)=(uint32_t)input[i].times;
                }
                break;

            case '<':
                if(input[i].times==1){
                    memcpy(txt_ptr,dec_ptr,sizeof(dec_ptr));
                    txt_ptr += sizeof(dec_ptr);
                } else if (input[i].times <= 0xff) {
                    memcpy(txt_ptr,sub_ptr8,sizeof(sub_ptr8));
                    txt_ptr += sizeof(sub_ptr8);
                    *(uint8_t*)(txt_ptr-sub_ptr8_arg)=(uint8_t)input[i].times;
                } else {
                    memcpy(txt_ptr,sub_ptr32,sizeof(sub_ptr32));
                    txt_ptr += sizeof(sub_ptr32);
                    *(uint32_t*)(txt_ptr-sub_ptr32_arg)=(uint32_t)input[i].times;
                }
                break;

            case '+':
                if(input[i].times==1){
                    memcpy(txt_ptr,inc_cell,sizeof(inc_cell));
                    txt_ptr += sizeof(inc_cell);
                } else if (input[i].times <= 0xff) {
                    memcpy(txt_ptr,add_cell8,sizeof(add_cell8));
                    txt_ptr += sizeof(add_cell8);
                    *(uint8_t*)(txt_ptr-add_cell8_arg)=(uint8_t)input[i].times;
                } else {
                    memcpy(txt_ptr,add_cell32,sizeof(add_cell32));
                    txt_ptr += sizeof(add_cell32);
                    *(uint32_t*)(txt_ptr-add_cell32_arg)=(uint32_t)input[i].times;
                }
                break;

            case '-':
                if(input[i].times==1){
                    memcpy(txt_ptr,dec_cell,sizeof(dec_cell));
                    txt_ptr += sizeof(dec_cell);
                } else if (input[i].times <= 0xff) {
                    memcpy(txt_ptr,sub_cell8,sizeof(sub_cell8));
                    txt_ptr += sizeof(sub_cell8);
                    *(uint8_t*)(txt_ptr-sub_cell8_arg)=(uint8_t)input[i].times;
                } else {
                    memcpy(txt_ptr,sub_cell32,sizeof(sub_cell32));
                    txt_ptr += sizeof(sub_cell32);
                    *(uint32_t*)(txt_ptr-sub_cell32_arg)=(uint32_t)input[i].times;
                }
                break;


            case '.':
                memcpy(txt_ptr,print_char32,sizeof(print_char32));
                txt_ptr += sizeof(print_char32);
                *(uint32_t*)(txt_ptr-print_char32_arg)=(uint32_t)1;
                input[i].times--;
                if(input[i].times != 0){
                    i--;
                }
                break;

            case ',':
                memcpy(txt_ptr,read_char32,sizeof(read_char32));
                txt_ptr += sizeof(read_char32);
                *(uint32_t*)(txt_ptr-read_char32_arg)=(uint32_t)1;
                input[i].times--;
                if(input[i].times != 0){
                    i--;
                }
                break;

            case '[':
                memcpy(txt_ptr,loop_start32,sizeof(loop_start32));
                txt_ptr += sizeof(loop_start32);
                *loop_jmps_ptr = (int32_t*)(txt_ptr - loop_start32_arg);
                loop_jmps_ptr++;
                input[i].times--;
                if(input[i].times != 0){
                    i--;
                }
                break;

            case ']':
                memcpy(txt_ptr,loop_end32,sizeof(loop_end32));
                txt_ptr += sizeof(loop_end32);
                loop_jmps_ptr--;
                int32_t offset = ((uint8_t*)(*loop_jmps_ptr + 1)) - txt_ptr;
                *(int32_t*)(txt_ptr - loop_end32_arg) = offset;
                **loop_jmps_ptr = -offset;
                input[i].times--;
                if(input[i].times != 0){
                    i--;
                }
                break;
        }
    }

    memcpy(txt_ptr,call_exit,sizeof(call_exit));
    txt_ptr += sizeof(call_exit);

    *(int32_t*)(text + 1) = entry + (txt_ptr - text);

    Elf32_Ehdr ehdr = {
        {0x7F,'E','L','F',ELFCLASS32,ELFDATA2LSB,EV_CURRENT,0,0,0,0,0,0,0,0,0},
        ET_EXEC,
        EM_386,
        EV_CURRENT,
        entry,
        sizeof(Elf32_Ehdr),
        0,
        0,
        sizeof(Elf32_Ehdr),
        sizeof(Elf32_Phdr),
        1,
        0,
        0,
        SHN_UNDEF,
    };

    Elf32_Phdr phdr = {
        PT_LOAD,
        0,
        org,
        org,
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + (txt_ptr - text),
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + (txt_ptr - text),
        PF_R | PF_X | PF_W,
        0x1000,
    };

    int out = open("a.out",O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU);
    write(out,&ehdr,sizeof(Elf32_Ehdr));
    write(out,&phdr,sizeof(Elf32_Phdr));

    write(out,text,txt_ptr-text);
    write(out,tape,sizeof(tape));
    close(out);
}
