#include <linux/elf.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_BIN_LEN 1024

int org = 0x08048000;

int main(int argc, char **argv){
    char text[MAX_BIN_LEN];
    Elf32_Off entry;

    char test[] = {0xb8,0x01,0x00,0x00,0x00,  /*mov eax,  1*/
                   0xbb,0x0a,0x00,0x00,0x00,  /*mov ebx, 10*/
                   0xcd,0x80};                /*int 0x80   */

    entry = org + sizeof(Elf32_Ehdr) + 1 * sizeof(Elf32_Phdr);

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
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + sizeof(test),
        sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + sizeof(test),
        PF_R | PF_X,
        0x1000,
    };

    int out = open("a.out",O_CREAT|O_WRONLY,0666);
    write(out,&ehdr,sizeof(Elf32_Ehdr));
    write(out,&phdr,sizeof(Elf32_Phdr));

    write(out,test,sizeof(test));
    close(out);
}
