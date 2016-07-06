#include <linux/elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define MAX_BIN_LEN 1024

int org = 0x08048000;



char move_right[] = {0x44};                    /*inc   esp         */

char move_left[]  = {0x4c};                    /*dec   esp         */

char inc_cell[]   = {0xfe,0x04,0x24};          /*inc   [esp]       */

char dec_cell[]   = {0xfe,0x0c,0x24};          /*dec   [esp]       */

char read_char[]  = {0xbb,0x00,0x00,0x00,0x00, /*mov   ebx,  0     */
                     0x89,0xe1,                /*mov   ecx,  esp   */
                     0xba,0x01,0x00,0x00,0x00, /*mov   edx,  1     */
                     0xb8,0x03,0x00,0x00,0x00, /*mov   eax,  3     */
                     0xcd,0x80};               /*int   0x80        */

char print_char[] = {0xbb,0x01,0x00,0x00,0x00, /*mov   ebx,  1     */
                     0x89,0xe1,                /*mov   ecx,  esp   */
                     0xba,0x01,0x00,0x00,0x00, /*mov   edx,  1     */
                     0xb8,0x04,0x00,0x00,0x00, /*mov   eax,  4     */
                     0xcd,0x80};               /*int   0x80        */

char call_exit[]  = {0xb8,0x01,0x00,0x00,0x00, /*mov   eax,  1     */
                     0xbb,0x00,0x00,0x00,0x00, /*mov   ebx,  0     */
                     0xcd,0x80};               /*int   0x80        */

int main(int argc, char **argv){
    char text[MAX_BIN_LEN];
    Elf32_Off entry;

    entry = org + sizeof(Elf32_Ehdr) + 1 * sizeof(Elf32_Phdr);

    char input;
    int text_len = 0;
    while((input = getchar()) != -1){
        switch(input){
            case '>':
                memcpy(text + text_len,move_right,sizeof(move_right));
                text_len += sizeof(move_right);
                break;
            case '<':
                memcpy(text + text_len,move_left,sizeof(move_left));
                text_len += sizeof(move_left);
                break;
            case '+':
                memcpy(text + text_len,inc_cell,sizeof(inc_cell));
                text_len += sizeof(inc_cell);
                break;
            case '-':
                memcpy(text + text_len,dec_cell,sizeof(dec_cell));
                text_len += sizeof(dec_cell);
                break;
            case '.':
                memcpy(text + text_len,print_char,sizeof(print_char));
                text_len += sizeof(print_char);
                break;
            case ',':
                memcpy(text + text_len,read_char,sizeof(read_char));
                text_len += sizeof(read_char);
                break;
            case '[':
            case ']':
                break;
        }
    }

    memcpy(text + text_len,call_exit,sizeof(call_exit));
    text_len += sizeof(call_exit);

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
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + text_len,
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + text_len,
        PF_R | PF_X,
        0x1000,
    };

    int out = open("a.out",O_CREAT|O_WRONLY,S_IRWXU);
    write(out,&ehdr,sizeof(Elf32_Ehdr));
    write(out,&phdr,sizeof(Elf32_Phdr));

    write(out,text,text_len);
    close(out);
}
