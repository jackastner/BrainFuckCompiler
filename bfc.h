#ifndef BFC_H
#define BFC

#include <stdint.h>

/*Byte code implementations of >*/
const uint8_t inc_ptr[]   = {0x44};                          /*inc esp        */

const uint8_t add_ptr8[]  = {0x83,0xc4,0x00};                /*add esp,int8_t */
const int8_t add_ptr8_arg = 1;

const uint8_t add_ptr32[] = {0x81,0xc2,0x00,0x00,0x00,0x00}; /*add esp,int32_t*/
const int8_t add_ptr32_arg = 4;


/*Byte code implementations of <*/
const uint8_t dec_ptr[]   = {0x4c};                          /*dec esp        */

const uint8_t sub_ptr8[]  = {0x83,0xec,0x00};                /*sub esp,int8_t */
const int8_t sub_ptr8_arg = 1;

const uint8_t sub_ptr32[] = {0x81,0xec,0x00,0x00,0x00,0x00}; /*sub esp,int32_t*/
const int8_t sub_ptr32_arg = 4;


/*Byte code implementations of +*/
const uint8_t inc_cell[]   = {0xfe,0x04,0x24};                     /*inc [esp]        */

const uint8_t add_cell8[]  = {0x80,0x04,0x24,0x00};                /*add [esp],int8_t */
const int8_t add_cell8_arg = 1;

const uint8_t add_cell32[] = {0x81,0x04,0x32,0x00,0x00,0x00,0x00}; /*add [esp],int32_t*/
const int8_t add_cell32_arg = 4;


/*Byte code implementations of -*/
const uint8_t dec_cell[]   = {0xfe,0x0c,0x24};                     /*dec [esp]        */

const uint8_t sub_cell8[]  = {0x80,0x2c,0x24,0x00};                /*sub [esp],int8_t */
const int8_t sub_cell8_arg = 1;

const uint8_t sub_cell32[] = {0x81,0x2c,0x24,0x00,0x00,0x00,0x00}; /*sub [esp],int32_t*/
const int8_t sub_cell32_arg = 4;


/*Byte code implemenations of ,*/
const uint8_t read_char32[] = {0xbb,0x00,0x00,0x00,0x00, /*mov ebx, 0      */
                               0x89,0xe1,                /*mov ecx, esp    */
                               0xb8,0x03,0x00,0x00,0x00, /*mov eax, 3      */
                               0xba,0x00,0x00,0x00,0x00, /*mov edx, int32_t*/
                               0xcd,0x80};               /*int 0x80       */
const int8_t read_char32_arg = 6;


/*Byte code implementation of .*/
const uint8_t print_char32[] = {0xbb,0x01,0x00,0x00,0x00, /*mov ebx, 1      */
                                0x89,0xe1,                /*mov ecx, esp    */
                                0xb8,0x04,0x00,0x00,0x00, /*mov eax, 4      */
                                0xba,0x00,0x00,0x00,0x00, /*mov edx, int32_t*/
                                0xcd,0x80};               /*int 0x80        */
const int8_t print_char32_arg = 6;


/*Byte code implementations of [*/
const uint8_t loop_start8[]  = {0x8a,0x04,0x24,                 /*mov  eax,    [esp] */
                                0x84,0xc0,                      /*test eax,    eax   */
                                0x74,0x00};                     /*je   int8_t        */
const int8_t loop_start8_arg = 1;

const uint8_t loop_start32[] = {0x8a,0x04,0x24,                 /*mov  eax,    [esp] */
                                0x84,0xc0,                      /*test eax,    eax   */
                                0x0f,0x84,0x00,0x00,0x00,0x00}; /*je   int32_t       */
const int8_t loop_start32_arg = 4;


/*Byte code implementations of ]*/
const uint8_t loop_end8[]  = {0x8a,0x04,0x24,                 /*mov  eax,    [esp] */
                              0x84,0xc0,                      /*test eax,    eax   */
                              0x75,0x00};                     /*je   int8_t        */
const int8_t loop_end8_arg = 1;

const uint8_t loop_end32[] = {0x8a,0x04,0x24,                 /*mov  eax,    [esp] */
                              0x84,0xc0,                      /*test eax,    eax   */
                              0x0f,0x85,0x00,0x00,0x00,0x00}; /*jne  int32_t       */
const int8_t loop_end32_arg = 4;


/*Byte code implemations of pre and post code*/
const uint8_t call_exit[] = {0xb8,0x01,0x00,0x00,0x00,  /*mov   eax,  1     */
                             0xbb,0x00,0x00,0x00,0x00,  /*mov   ebx,  0     */
                             0xcd,0x80};                /*int   0x80        */

const uint8_t prelude[]   = {0xbc,0x00,0x00,0x00,0x00}; /*mov   esp, int32_t*/

const uint8_t tape[100];

#endif
