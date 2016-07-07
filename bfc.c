#include <linux/elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_BIN_LEN 1024
#define MAX_JUMPS 100

unsigned int org = 0x08048000;



unsigned char move_right[] = {0x44};                    /*inc   esp         */

unsigned char move_left[]  = {0x4c};                    /*dec   esp         */

unsigned char inc_cell[]   = {0xfe,0x04,0x24};          /*inc   [esp]       */

unsigned char dec_cell[]   = {0xfe,0x0c,0x24};          /*dec   [esp]       */

unsigned char read_char[]  = {0xbb,0x00,0x00,0x00,0x00, /*mov   ebx,  0     */
                              0x89,0xe1,                /*mov   ecx,  esp   */
                              0xba,0x01,0x00,0x00,0x00, /*mov   edx,  1     */
                              0xb8,0x03,0x00,0x00,0x00, /*mov   eax,  3     */
                              0xcd,0x80};               /*int   0x80        */

unsigned char print_char[] = {0xbb,0x01,0x00,0x00,0x00, /*mov   ebx,  1     */
                              0x89,0xe1,                /*mov   ecx,  esp   */
                              0xba,0x01,0x00,0x00,0x00, /*mov   edx,  1     */
                              0xb8,0x04,0x00,0x00,0x00, /*mov   eax,  4     */
                              0xcd,0x80};               /*int   0x80        */


unsigned char loop_start[] = {0x8a,0x04,0x24,           /*mov   eax,  [esp] */
                              0x85,0xc0,                /*test  eax,  eax   */
                              0x74,0x00};               /*je    rel         */

unsigned char loop_end[]   = {0x8a,0x04,0x24,           /*mov   eax,  [esp] */
                              0x85,0xc0,                /*test  eax,  eax   */
                              0x75,0x00};               /*jne   rel         */

unsigned char call_exit[]  = {0xb8,0x01,0x00,0x00,0x00, /*mov   eax,  1     */
                              0xbb,0x00,0x00,0x00,0x00, /*mov   ebx,  0     */
                              0xcd,0x80};               /*int   0x80        */
unsigned char prelude[]    = {0xbc,0x00,0x00,0x00,0x00};/*mov   esp, int32_t*/

unsigned char tape[100];

int main(){
    unsigned char text[MAX_BIN_LEN];
    unsigned char *txt_ptr = text;

    int8_t *loop_jmps[MAX_JUMPS];
    int8_t **loop_jmps_ptr = loop_jmps;

    Elf32_Off entry;

    entry = org + sizeof(Elf32_Ehdr) + 1 * sizeof(Elf32_Phdr);

    memcpy(txt_ptr,prelude,sizeof(prelude));
    txt_ptr += sizeof(prelude);
    char input;
    while((input = getchar()) != -1){
        switch(input){
            case '>':
                memcpy(txt_ptr,move_right,sizeof(move_right));
                txt_ptr += sizeof(move_right);
                break;
            case '<':
                memcpy(txt_ptr,move_left,sizeof(move_left));
                txt_ptr += sizeof(move_left);
                break;
            case '+':
                memcpy(txt_ptr,inc_cell,sizeof(inc_cell));
                txt_ptr += sizeof(inc_cell);
                break;
            case '-':
                memcpy(txt_ptr,dec_cell,sizeof(dec_cell));
                txt_ptr += sizeof(dec_cell);
                break;
            case '.':
                memcpy(txt_ptr,print_char,sizeof(print_char));
                txt_ptr += sizeof(print_char);
                break;
            case ',':
                memcpy(txt_ptr,read_char,sizeof(read_char));
                txt_ptr += sizeof(read_char);
                break;
            case '[':
                memcpy(txt_ptr,loop_start,sizeof(loop_start));
                txt_ptr += sizeof(loop_start);
                *loop_jmps_ptr = (int8_t*) txt_ptr - 1;
                loop_jmps_ptr++;
                break;
            case ']':
                memcpy(txt_ptr,loop_end,sizeof(loop_end));
                txt_ptr += sizeof(loop_end);
                loop_jmps_ptr--;
                int8_t offset = ((unsigned char*) *loop_jmps_ptr) - txt_ptr + 1;
                printf("%x\n",offset);
                *(txt_ptr - 1) = -offset;
                **loop_jmps_ptr = offset;
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
