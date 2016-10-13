#include <stdio.h>
#include <stdlib.h>

#include "decode.h"





//  This code performs an order-0 adaptive arithmetic decoding
//  function on an input file/stream, and sends the result to an
//  output file or stream.
//
//  This program contains the source code from the 1987 CACM
//  article by Witten, Neal, and Cleary.  I have taken the
//  source modules and combined them into this single file for
//  ease of distribution and compilation.  Other than that,
//  the code is essentially unchanged.

int char_to_index[No_of_chars];         /* To index from character          */
unsigned char index_to_char[No_of_symbols+1]; /* To character from index    */

code_value value;        /* Currently-seen code value                */
code_value low, high;    /* Ends of current code region              */
int freq[No_of_symbols+1];      /* Symbol frequencies                       */
int cum_freq[No_of_symbols+1];  /* Cumulative symbol frequencies            */
int buffer;                     /* Bits waiting to be input                 */
int bits_to_go;                 /* Number of bits still in buffer           */
int garbage_bits;               /* Number of bits past end-of-file          */
long bits_to_follow;            /* Number of opposite bits to output after  */


unsigned char *put_pointer;
int put_size;

int input_bit() {
  int t;
    if (bits_to_go==0) {                        /* Read the next byte if no */
        if(put_size>0) {
          buffer = *put_pointer&0xff;                   /* bits are left in buffer. */
          put_pointer++;
          put_size--;
        } else {
	  buffer=0;
            garbage_bits += 1;                      /* Return arbitrary bits*/
            if (garbage_bits>Code_value_bits-2) {   /* after eof, but check */
                printf("ARID: Bad input!\n"); /* for too many such.   */

            }
        }
        bits_to_go = 8;
    }
    t = buffer&1;                               /* Return the next bit from */
    buffer >>= 1;                               /* the bottom of the byte.  */
    bits_to_go--;
    return t;
}
static void output_bit(int bit){
  buffer>>=1;
  if (bit) buffer |= 0x80;
    bits_to_go--;
    if (bits_to_go==0) {
        *put_pointer=buffer;
	put_pointer++;
        put_size++;
        bits_to_go = 8;
    }
}
void bit_plus_follow( int bit ){
  output_bit(bit);                            /* Output the bit.          */
    while (bits_to_follow>0) {
        output_bit(!bit);                       /* Output bits_to_follow    */
        bits_to_follow -= 1;                    /* opposite bits. Set       */
    }                                           /* bits_to_follow to zero.  */
}

void encode_symbol(int symbol,int cum_freq[] )
{   long range;                 /* Size of the current code region          */
    range = (long)(high-low)+1;
    high = low +                                /* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1; /* to that allotted to this */
    low = low +                                 /* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {                                  /* Loop to output bits.     */
        if (high<Half) {
            bit_plus_follow(0);                 /* Output 0 if in low half. */
        }
        else if (low>=Half) {                   /* Output 1 if in high half.*/
            bit_plus_follow(1);
            low -= Half;
            high -= Half;                       /* Subtract offset to top.  */
        }
        else if (low>=First_qtr                 /* Output an opposite bit   */
              && high<Third_qtr) {              /* later if in middle half. */
            bits_to_follow += 1;
            low -= First_qtr;                   /* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;                             /* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;                        /* Scale up code range.     */
    }
}

int decode_symbol( int cum_freq[] )
{   long range;                 /* Size of current code region              */
    int cum;                    /* Cumulative frequency calculated          */
    int symbol;                 /* Symbol decoded                           */
    range = (long)(high-low)+1;
    cum = (int)                                 /* Find cum freq for value. */
      ((((long)(value-low)+1)*cum_freq[0]-1)/range);
    for (symbol = 1; cum_freq[symbol]>cum; symbol++) ; /* Then find symbol. */
    high = low +                                /* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1; /* to that allotted to this */
    low = low +                                 /* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {                                  /* Loop to get rid of bits. */
        if (high<Half) {
            /* nothing */                       /* Expand low half.         */
        }
        else if (low>=Half) {                   /* Expand high half.        */
            value -= Half;
            low -= Half;                        /* Subtract offset to top.  */
            high -= Half;
        }
        else if (low>=First_qtr                 /* Expand middle half.      */
              && high<Third_qtr) {
            value -= First_qtr;
            low -= First_qtr;                   /* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;                             /* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;                        /* Scale up code range.     */
        value = 2*value+input_bit();            /* Move in next input bit.  */
    }
    return symbol;
}


void start_model(){
  int i;
    for(i=0;i<No_of_chars;i++) {           /* Set up tables that       */
        char_to_index[i]=i+1;                 /* translate between symbol */
        index_to_char[i+1]=(unsigned char)i; /* indexes and characters.  */
    }
    for (i=0;i<=No_of_symbols;i++) {        /* Set up initial frequency */
        freq[i]=1;                            /* counts to be one for all */
        cum_freq[i]=No_of_symbols-i;          /* symbols.                 */
    }
    *freq=0;                                /* Freq[0] must not be the  */
}                                               /* same as freq[1].         */

void update_model(int symbol){
  int i;                      /* New index for symbol                     */
    if (cum_freq[0]==Max_frequency) {           /* See if frequency counts  */
        int cum;                                /* are at their maximum.    */
        cum = 0;
        for (i = No_of_symbols; i>=0; i--) {    /* If so, halve all the     */
            freq[i] = (freq[i]+1)/2;            /* counts (keeping them     */
            cum_freq[i] = cum;                  /* non-zero).               */
            cum += freq[i];
        }
    }
    for (i = symbol; freq[i]==freq[i-1]; i--) ; /* Find symbol's new index. */
    if (i<symbol) {
        int ch_i, ch_symbol;
        ch_i = index_to_char[i];                /* Update the translation   */
        ch_symbol = index_to_char[symbol];      /* tables if the symbol has */
        index_to_char[i] = (unsigned char) ch_symbol; /* moved.             */
        index_to_char[symbol] = (unsigned char) ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i] += 1;                               /* Increment the frequency  */
    while (i>0) {                               /* count for the symbol and */
        i -= 1;                                 /* update the cumulative    */
        cum_freq[i] += 1;                       /* frequencies.             */
    }
}

/**************** Burrows-Wheeler Transformation ***************/

 /* Hilfsfunktionen fuer Burrows-Wheeler transform*/

unsigned char *rotlexcmp_buf = NULL;
int rottexcmp_bufsize = 0;

int rotlexcmp(const void *l, const void *r) {
  int li=*(const int*)l,ri=*(const int*)r,ac=rottexcmp_bufsize;
  while(rotlexcmp_buf[li]==rotlexcmp_buf[ri]) {
        if(++li==rottexcmp_bufsize) li=0;
        if(++ri==rottexcmp_bufsize) ri=0;
        if(!--ac) return 0;
  }
  if(rotlexcmp_buf[li]>rotlexcmp_buf[ri]) return 1;
  else return -1;
}







