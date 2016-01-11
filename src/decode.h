
#define No_of_chars 256                 /* Number of character symbols      */
#define EOF_symbol (No_of_chars+1)      /* Index of EOF symbol              */

#define No_of_symbols (No_of_chars+1)   /* Total number of symbols          */

#define Max_frequency 16383             /* Maximum allowed frequency count  */
#define Code_value_bits 16              /* Number of bits in a code value   */
#define Top_value (((long)1<<Code_value_bits)-1)      /* Largest code value */
#define First_qtr (Top_value/4+1)       /* Point after first quarter        */
#define Half      (2*First_qtr)         /* Point after first half           */
#define Third_qtr (3*First_qtr)         /* Point after third quarter        */
typedef long code_value;                /* Type of an arithmetic code value */

extern unsigned char *put_pointer;
extern int put_size;
extern int buffer;                     /* Bits waiting to be input                 */
extern int bits_to_go;                 /* Number of bits still in buffer           */
extern int garbage_bits;               /* Number of bits past end-of-file          */
extern long bits_to_follow;            /* Number of opposite bits to output after  */
extern code_value value;        /* Currently-seen code value                */
extern code_value low, high;    /* Ends of current code region              */
extern int char_to_index[];         /* To index from character          */
extern unsigned char index_to_char[]; /* To character from index    */
extern int cum_freq[];  /* Cumulative symbol frequencies            */


void start_model();
void update_model(int symbol);
int decode_symbol( int cum_freq[] );
inline int input_bit();
void encode_symbol(int symbol,int cum_freq[] );
void bit_plus_follow( int bit );


/**************** Burrows-Wheeler Transformation ***************/

extern unsigned char *rotlexcmp_buf;
extern int rottexcmp_bufsize;
int rotlexcmp(const void *l, const void *r);

