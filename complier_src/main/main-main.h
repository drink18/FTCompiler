#ifndef MAIN_H
#define MAIN_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <setjmp.h>
#include <time.h>
#include "../lib/error.h"
#ifndef _WIN32
# include <unistd.h>
# include <sys/time.h>
# include <sys/ucontext.h>
# include <sys/mman.h>
# ifndef CONFIG_TCC_STATIC
#  include <dlfcn.h>
# endif
#else
#include <windows.h>
#include <sys/timeb.h>
#include <io.h> /* open, close etc. */
#include <direct.h> /* getcwd */
# ifdef __GNUC__
#  include <stdint.h>
# else
   typedef UINT_PTR uintptr_t;
# endif
# define inline __inline
# define inp next_inp
# ifdef LIBTCC_AS_DLL
#  define LIBTCCAPI __declspec(dllexport)
#  define PUB_FUNC LIBTCCAPI
# endif
#endif


#define TCC_TARGET_PE   //I want defined externed

#ifndef O_BINARY
# define O_BINARY 0
#endif
typedef unsigned short nwchar_t;

/* parser debug */
//#define PARSE_DEBUG
/* preprocessor debug */
//#define PP_DEBUG
/* include file debug */
//#define INC_DEBUG
/* memory leak debug */
//#define MEM_DEBUG


#define ST_INLN
#define ST_FUNC
#ifndef CONFIG_SYSROOT
# define CONFIG_SYSROOT ""
#endif
#ifndef CONFIG_TCCDIR
# define CONFIG_TCCDIR "."
#endif
#ifndef CONFIG_LDDIR
# define CONFIG_LDDIR "lib"
#endif
/* Below: {B} is substituted by CONFIG_TCCDIR (rsp. -B option) */

/* system include paths */
#ifndef CONFIG_TCC_SYSINCLUDEPATHS
# ifdef TCC_TARGET_PE
#  define CONFIG_TCC_SYSINCLUDEPATHS "{B}/include;{B}/include/winapi"
# elif defined CONFIG_MULTIARCHDIR
#  define CONFIG_TCC_SYSINCLUDEPATHS \
        CONFIG_SYSROOT "/usr/local/include" \
    ":" CONFIG_SYSROOT "/usr/local/include/" CONFIG_MULTIARCHDIR \
    ":" CONFIG_SYSROOT "/usr/include" \
    ":" CONFIG_SYSROOT "/usr/include/" CONFIG_MULTIARCHDIR \
    ":" "{B}/include"
# else
#  define CONFIG_TCC_SYSINCLUDEPATHS \
        CONFIG_SYSROOT "/usr/local/include" \
    ":" CONFIG_SYSROOT "/usr/include" \
    ":" "{B}/include"
# endif
#endif
#define ST_DATA extern
/* -------------------------------------------- */

#define FALSE 0
#define false 0
#define TRUE 1
#define true 1
typedef int BOOL;

//fun attr
#define FUNC_FOR_BOARD   0x01
#define FUNC_WITH_NO_FUNC 0x02 /*func is not call other func so can pass arg with reg*/

#define INCLUDE_STACK_SIZE  32
#define IFDEF_STACK_SIZE    64
#define VSTACK_SIZE         256
#define STRING_MAX_SIZE     1024
#define PACK_STACK_SIZE     8

#define TOK_HASH_SIZE       8192 /* must be a power of two */
#define TOK_ALLOC_INCR      512  /* must be a power of two */
#define TOK_MAX_SIZE        4 /* token max size in int unit when stored in string */

/* token symbol management */
typedef struct TokenSym {
    struct TokenSym *hash_next;
    struct Sym *sym_define; /* direct pointer to define */
    struct Sym *sym_label; /* direct pointer to label */
    struct Sym *sym_struct; /* direct pointer to structure */
    struct Sym *sym_identifier; /* direct pointer to identifier */
    int tok; /* token number */
    int len;
    char str[1];
} TokenSym;
typedef struct CString {
    int size; /* size in bytes */
    void *data; /* either 'char *' or 'nwchar_t *' */
    int size_allocated;
    void *data_allocated; /* if non NULL, data has been malloced */
} CString;

/* type definition */
typedef struct CType {
    int t;
    struct Sym *ref;
} CType;
/* long double size and alignment, in bytes */
#define LDOUBLE_SIZE  8

/* constant value */
typedef union CValue {
    long double ld;
    double d;
    float f;
    int i;
    unsigned int ui;
    unsigned int ul; /* address (should be unsigned long on 64 bit cpu) */
    long long ll;
    unsigned long long ull;
    struct CString *cstr;
    void *ptr;
    int tab[LDOUBLE_SIZE/4];
} CValue;

/* value on stack */
typedef struct SValue {
    CType type;      /* type */
    unsigned short r;      /* register + flags */
    unsigned short r2;     /* second register, used for 'long long'
                              type. If not used, set to VT_CONST */
    CValue c;              /* constant, if VT_CONST */
    struct Sym *sym;       /* symbol, if (VT_SYM | VT_CONST) */
} SValue;

/* symbol management */
typedef struct Sym {
    int v;    /* symbol token */
    char *asm_label;    /* associated asm label */
    long r;    /* associated register */
    union {
        long c;    /* associated number */
        int *d;   /* define token stream */
    };
    CType type;    /* associated type */
    union {
        struct Sym *next; /* next related symbol */
        long jnext; /* next jump label */
    };
    struct Sym *prev; /* prev symbol in stack */
    struct Sym *prev_tok; /* previous symbol for this token */
} Sym;
/* -------------------------------------------------- */

#define SYM_STRUCT     0x40000000 /* struct/union/enum symbol space */
#define SYM_FIELD      0x20000000 /* struct/union field symbol space */
#define SYM_FIRST_ANOM 0x10000000 /* first anonymous sym */


/* field 'Sym.t' for macros */
#define MACRO_OBJ      0 /* object like macro */
#define MACRO_FUNC     1 /* function like macro */



#define IO_BUF_SIZE 16384

typedef struct BufferedFile {
    uint8_t *buf_ptr;
    uint8_t *buf_end;
    int fd;
    struct BufferedFile *prev;
    int line_num;    /* current line number - here to simplify code */
    int ifndef_macro;  /* #ifndef macro / #endif search */
    int ifndef_macro_saved; /* saved ifndef_macro */
    int *ifdef_stack_ptr; /* ifdef_stack value at the start of the file */
    char inc_type;          /* type of include */
    char inc_filename[512]; /* filename specified by the user */
    char filename[1024];    /* current filename - here to simplify code */
    unsigned char buffer[IO_BUF_SIZE + 1]; /* extra size for CH_EOB char */
} BufferedFile;

#define CH_EOB   '\\'       /* end of buffer or '\0' char in file */
#define CH_EOF   (-1)   /* end of file */

/* used to record tokens */
typedef struct TokenString {
    int *str;
    int len;
    int allocated_len;
    int last_line_num;
} TokenString;


/* inline functions */
typedef struct InlineFunc {
    int *token_str;
    Sym *sym;
    char filename[1];
} InlineFunc;


/* include file cache, used to find files faster and also to eliminate
   inclusion if the include file is protected by #ifndef ... #endif */
typedef struct CachedInclude {
    int ifndef_macro;
    int hash_next; /* -1 if none */
    char type; /* '"' or '>' to give include type */
    char filename[1]; /* path specified in #include */
} CachedInclude;
#define CACHED_INCLUDES_HASH_SIZE 512
struct TCCState {
    unsigned output_type    : 8;
    unsigned reloc_output   : 1;

    BufferedFile **include_stack_ptr;
    int *ifdef_stack_ptr;

    /* include file handling */
    char **include_paths;
    int nb_include_paths;
    char **sysinclude_paths;
    int nb_sysinclude_paths;
    CachedInclude **cached_includes;
    int nb_cached_includes;

    char **library_paths;
    int nb_library_paths;
    char **crt_paths;
    int nb_crt_paths;


    int nb_loaded_dlls;




    struct sym_attr *sym_attrs;
    int nb_sym_attrs;
    /* give the correspondance from symtab indexes to dynsym indexes */
    int *symtab_to_dynsym;



    int nostdinc; /* if true, no standard headers are added */
    int nostdlib; /* if true, no standard libraries are added */
    int nocommon; /* if true, do not use common symbols for .bss data */

    /* if true, static linking is performed */
    int static_link;

    /* soname as specified on the command line (-soname) */
    const char *soname;
    /* rpath as specified on the command line (-Wl,-rpath=) */
    const char *rpath;

    /* if true, all symbols are exported */
    int rdynamic;

    /* if true, resolve symbols in the current module first (-Wl,Bsymbolic) */
    int symbolic;

    /* if true, only link in referenced objects from archive */
    int alacarte_link;

    /* address of text section */


    /* symbols to call at load-time / unload-time */
    const char *init_symbol;
    const char *fini_symbol;

    /* output format, see TCC_OUTPUT_FORMAT_xxx */
    int output_format;

    /* C language options */
    int char_is_unsigned;
    int leading_underscore;

    /* warning switches */
    int warn_write_strings;
    int warn_unsupported;
    int warn_error;
    int warn_none;
    int warn_implicit_function_declaration;

    /* display some information during compilation */
    int verbose;
    /* compile with debug symbol (and use them if error during execution) */
    int do_debug;
#ifdef CONFIG_TCC_BCHECK
    /* compile with built-in memory and bounds checker */
    int do_bounds_check;
#endif
    /* give the path of the tcc libraries */
    char *tcc_lib_path;

    /* error handling */
    void *error_opaque;
    void (*error_func)(void *opaque, const char *msg);
    int error_set_jmp_enabled;

    int nb_errors;
	
	/* output file for preprocessing (-E) */
    FILE *ppfp;
    /* tiny assembler state */
    Sym *asm_labels;

    /* see include_stack_ptr */
    BufferedFile *include_stack[INCLUDE_STACK_SIZE];

    /* see ifdef_stack_ptr */
    int ifdef_stack[IFDEF_STACK_SIZE];

    /* see cached_includes */
    int cached_includes_hash[CACHED_INCLUDES_HASH_SIZE];

    /* pack stack */
    int pack_stack[PACK_STACK_SIZE];
    int *pack_stack_ptr;

    /* output file for preprocessing */
     FILE *outfile;

    /* automatically collected dependencies for this compilation */
    char **target_deps;
    int nb_target_deps;

    /* for tcc_relocate */
    int runtime_added;
    void *runtime_mem;
#ifdef HAVE_SELINUX
    void *write_mem;
    unsigned long mem_size;
#endif

    struct InlineFunc **inline_fns;
    int nb_inline_fns;

#ifdef TCC_TARGET_I386
    int seg_size;
#endif

    /* section alignment */
    unsigned long section_align;

#ifdef TCC_TARGET_PE
    /* PE info */
    int pe_subsystem;
    unsigned long pe_file_align;
    unsigned long pe_stack_size;
#ifdef TCC_TARGET_X86_64
    Section *uw_pdata;
    int uw_sym;
    unsigned uw_offs;
#endif
#endif

#ifndef TCC_TARGET_PE
#if defined TCC_TARGET_X86_64 || defined TCC_TARGET_ARM
    /* write PLT and GOT here */
    char *runtime_plt_and_got;
    unsigned int runtime_plt_and_got_offset;
#endif
#endif
};

/* The current value can be: */
#define VT_VALMASK   0x003f  /* mask for value location, register or: */
#define VT_CONST     0x0030  /* constant in vc (must be first non register value) */
#define VT_LLOCAL    0x0031  /* lvalue, offset on stack */
#define VT_LOCAL     0x0032  /* offset on stack */
#define VT_CMP       0x0033  /* the value is stored in processor flags (in vc) */
#define VT_JMP       0x0034  /* value is the consequence of jmp true (even) */
#define VT_JMPI      0x0035  /* value is the consequence of jmp false (odd) */
#define VT_REF       0x0040  /* value is pointer to structure rather than address */
#define VT_LVAL      0x0100  /* var is an lvalue */
#define VT_SYM       0x0200  /* a symbol value is added */

#define VT_STATIC    0x0001

/* token values */
#define TOKEN_TYPE 0x91 
/* warning: the following compare tokens depend on i386 asm code */
#define TOK_ULT 0x92
#define TOK_UGE 0x93
#define TOK_EQ  0x94
#define TOK_NE  0x95
#define TOK_ULE 0x96
#define TOK_UGT 0x97
#define TOK_Nset 0x98
#define TOK_Nclear 0x99
#define TOK_LT  0x9c
#define TOK_GE  0x9d
#define TOK_LE  0x9e
#define TOK_GT  0x9f

#define TOK_LAND  0xa0
#define TOK_LOR   0xa1



#define TOK_DEC   0xa2
#define TOK_MID   0xa3 /* inc/dec, to void constant */
#define TOK_INC   0xa4
#define TOK_UDIV  0xb0 /* unsigned division */
#define TOK_UMOD  0xb1 /* unsigned modulo */
#define TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */
#define TOK_CINT   0xb3 /* number in tokc */
#define TOK_CCHAR 0xb4 /* char constant in tokc */
#define TOK_STR   0xb5 /* pointer to string in tokc */
#define TOK_TWOSHARPS 0xb6 /* ## preprocessing token */
#define TOK_LCHAR    0xb7
#define TOK_LSTR     0xb8
#define TOK_CFLOAT   0xb9 /* float constant */
#define TOK_LINENUM  0xba /* line number info */
#define TOK_CDOUBLE  0xc0 /* double constant */
#define TOK_CLDOUBLE 0xc1 /* long double constant */
#define TOK_UMULL    0xc2 /* unsigned 32x32 -> 64 mul */
#define TOK_ADDC1    0xc3 /* add with carry generation */
#define TOK_ADDC2    0xc4 /* add with carry use */
#define TOK_SUBC1    0xc5 /* add with carry generation */
#define TOK_SUBC2    0xc6 /* add with carry use */
#define TOK_CUINT    0xc8 /* unsigned int constant */
#define TOK_CLLONG   0xc9 /* long long constant */
#define TOK_CULLONG  0xca /* unsigned long long constant */
#define TOK_ARROW    0xcb
#define TOK_DOTS     0xcc /* three dots */
#define TOK_SHR      0xcd /* unsigned shift right */
#define TOK_PPNUM    0xce /* preprocessor number */
#define TOK_NOSUBST  0xcf /* means following token has already been pp'd */


#define TOK_SHL   0x01 /* shift left */
#define TOK_SAR   0x02 /* signed shift right */

/* assignement operators : normal operator or 0x80 */
#define TOK_A_MOD 0xa5
#define TOK_A_AND 0xa6
#define TOK_A_MUL 0xaa
#define TOK_A_ADD 0xab
#define TOK_A_SUB 0xad
#define TOK_A_DIV 0xaf
#define TOK_A_XOR 0xde
#define TOK_A_OR  0xfc
#define TOK_A_SHL 0x81
#define TOK_A_SAR 0x82

#ifndef offsetof
#define offsetof(type, field) ((size_t) &((type *)0)->field)
#endif

#ifndef countof
#define countof(tab) (sizeof(tab) / sizeof((tab)[0]))
#endif

#define TOK_EOF       (-1)  /* end of file */
#define TOK_LINEFEED  10    /* line feed */

/* all identificators and strings have token above that */
#define TOK_IDENT 256

/* field 'Sym.t' for macros */
#ifdef _WIN32
#define IS_DIRSEP(c) (c == '/' || c == '\\')
#define IS_ABSPATH(p) (IS_DIRSEP(p[0]) || (p[0] && p[1] == ':' && IS_DIRSEP(p[2])))
#define PATHCMP stricmp
#else
#define IS_DIRSEP(c) (c == '/')
#define IS_ABSPATH(p) IS_DIRSEP(p[0])
#define PATHCMP strcmp
#endif
#define PATHSEP ';'
#ifndef PUB_FUNC
# define PUB_FUNC
#endif
#define MACRO_OBJ      0 /* object like macro */
#define MACRO_FUNC     1 /* function like macro */

/* ------------ libtcc.c ------------ */

/* use GNU C extensions */
ST_DATA int gnu_ext;
/* use Tiny C extensions */
ST_DATA int tcc_ext;
/* XXX: get rid of this ASAP */
ST_DATA struct TCCState *tcc_state;

#ifdef MEM_DEBUG
ST_DATA int mem_cur_size;
ST_DATA int mem_max_size;
#endif

#define AFF_PRINT_ERROR     0x0001 /* print error if file not found */
#define AFF_REFERENCED_DLL  0x0002 /* load a referenced dll from another dll */
#define AFF_PREPROCESS      0x0004 /* preprocess file */

/* public functions currently used by the tcc main function */
PUB_FUNC char *pstrcpy(char *buf, int buf_size, const char *s);
PUB_FUNC char *pstrcat(char *buf, int buf_size, const char *s);
PUB_FUNC char *pstrncpy(char *out, const char *in, size_t num);
PUB_FUNC char *tcc_basename(const char *name);
PUB_FUNC char *tcc_fileextension (const char *name);
PUB_FUNC void tcc_free(void *ptr);
PUB_FUNC void *tcc_malloc(unsigned long size);
PUB_FUNC void *tcc_mallocz(unsigned long size);
PUB_FUNC void *tcc_realloc(void *ptr, unsigned long size);
PUB_FUNC char *tcc_strdup(const char *str);
#define free(p) use_tcc_free(p)
#define malloc(s) use_tcc_malloc(s)
#define realloc(p, s) use_tcc_realloc(p, s)
#undef strdup
#define strdup(s) use_tcc_strdup(s)
PUB_FUNC void tcc_memstats(void);
PUB_FUNC void tcc_error_noabort(const char *fmt, ...);
PUB_FUNC void tcc_error(const char *fmt, ...);
PUB_FUNC void tcc_warning(const char *fmt, ...);

/* other utilities */
ST_FUNC void dynarray_add(void ***ptab, int *nb_ptr, void *data);
ST_FUNC void dynarray_reset(void *pp, int *n);
ST_FUNC void cstr_ccat(CString *cstr, int ch);
ST_FUNC void cstr_cat(CString *cstr, const char *str);
ST_FUNC void cstr_wccat(CString *cstr, int ch);
ST_FUNC void cstr_new(CString *cstr);
ST_FUNC void cstr_free(CString *cstr);
ST_FUNC void cstr_reset(CString *cstr);

/*
ST_FUNC Section *new_section(TCCState *s1, const char *name, int sh_type, int sh_flags);
ST_FUNC void section_realloc(Section *sec, unsigned long new_size);
ST_FUNC void *section_ptr_add(Section *sec, unsigned long size);
ST_FUNC void section_reserve(Section *sec, unsigned long size);
ST_FUNC Section *find_section(TCCState *s1, const char *name);

ST_FUNC void put_extern_sym2(Sym *sym, Section *section, addr_t value, unsigned long size, int can_add_underscore);
ST_FUNC void put_extern_sym(Sym *sym, Section *section, addr_t value, unsigned long size);
ST_FUNC void greloc(Section *s, Sym *sym, unsigned long offset, int type);
*/

ST_INLN void sym_free(Sym *sym);
ST_FUNC Sym *sym_push2(Sym **ps, int v, int t, long c);
ST_FUNC Sym *sym_find2(Sym *s, int v);
ST_FUNC Sym *sym_push(int v, CType *type, int r, int c);
ST_FUNC void sym_pop(Sym **ptop, Sym *b);
ST_INLN Sym *struct_find(int v);
ST_INLN Sym *sym_find(int v);
ST_FUNC Sym *global_identifier_push(int v, int t, int c);

ST_FUNC void tcc_open_bf(TCCState *s1, const char *filename, int initlen);
ST_FUNC int tcc_open(TCCState *s1, const char *filename);
ST_FUNC void tcc_close(void);

ST_FUNC int tcc_add_file_internal(TCCState *s1, const char *filename, int flags);
ST_FUNC int tcc_add_crt(TCCState *s, const char *filename);
ST_FUNC int tcc_add_dll(TCCState *s, const char *filename, int flags);

//PUB_FUNC void tcc_print_stats(TCCState *s, int64_t total_time);
//PUB_FUNC int tcc_parse_args(TCCState *s, int argc, char **argv);


enum tcc_token {
    TOK_LAST = TOK_IDENT - 1,
#define DEF(id, str) id,
#include "tcctok.h"
#undef DEF
};


/* space exlcuding newline */
static inline int is_space(int ch)
{
    return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static inline int isid(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline int isnum(int c)
{
    return c >= '0' && c <= '9';
}

static inline int isoct(int c)
{
    return c >= '0' && c <= '7';
}

static inline int toup(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

/* ------------ tccpp.c ------------ */

ST_DATA struct BufferedFile *file;
ST_DATA int ch, tok;
ST_DATA CValue tokc;
ST_DATA const int *macro_ptr;
ST_DATA int parse_flags;
ST_DATA int tok_flags;
ST_DATA CString tokcstr; /* current parsed string, if any */

/* display benchmark infos */
ST_DATA int total_lines;
ST_DATA int total_bytes;
ST_DATA int tok_ident;
ST_DATA TokenSym **table_ident;

#define TOK_FLAG_BOL   0x0001 /* beginning of line before */
#define TOK_FLAG_BOF   0x0002 /* beginning of file before */
#define TOK_FLAG_ENDIF 0x0004 /* a endif was found matching starting #ifdef */
#define TOK_FLAG_EOF   0x0008 /* end of file */

#define PARSE_FLAG_PREPROCESS 0x0001 /* activate preprocessing */
#define PARSE_FLAG_TOK_NUM    0x0002 /* return numbers instead of TOK_PPNUM */
#define PARSE_FLAG_LINEFEED   0x0004 /* line feed is returned as a
                                        token. line feed is also
                                        returned at eof */
#define PARSE_FLAG_ASM_COMMENTS 0x0008 /* '#' can be used for line comment */
#define PARSE_FLAG_SPACES     0x0010 /* next() returns space tokens (for -E) */

ST_FUNC TokenSym *tok_alloc(const char *str, int len);
ST_FUNC char *get_tok_str(int v, CValue *cv);
//ST_FUNC void save_parse_state(ParseState *s);
//ST_FUNC void restore_parse_state(ParseState *s);
ST_INLN void tok_str_new(TokenString *s);
ST_FUNC void tok_str_free(int *str);
ST_FUNC void tok_str_add(TokenString *s, int t);
ST_FUNC void tok_str_add_tok(TokenString *s);
ST_INLN void define_push(int v, int macro_type, int *str, Sym *first_arg);
ST_FUNC void define_undef(Sym *s);
ST_INLN Sym *define_find(int v);
ST_FUNC void free_defines(Sym *b);
ST_FUNC Sym *label_find(int v);
ST_FUNC Sym *label_push(Sym **ptop, int v, int flags);
ST_FUNC void label_pop(Sym **ptop, Sym *slast);
ST_FUNC void parse_define(void);
ST_FUNC void preprocess(int is_bof);
ST_FUNC void next_nomacro(void);
ST_FUNC void next(void);
ST_INLN void unget_tok(int last_tok);
ST_FUNC void preprocess_init(TCCState *s1);
ST_FUNC void preprocess_new(void);
ST_FUNC int tcc_preprocess(TCCState *s1);
ST_FUNC void skip(int c);
ST_FUNC void expect(const char *msg);
ST_FUNC void vpop(void);


/* ------------ tccgen.c ------------ */

#define SYM_POOL_NB (8192 / sizeof(Sym))
 Sym *sym_free_first;
 void **sym_pools;
 int nb_sym_pools;
 Sym *sym_free_first;


 Sym *global_stack;
 Sym *local_stack;
 Sym *local_label_stack;
 Sym *global_label_stack;
 Sym *define_stack;
 Sym *scope_stack_bottom;
 CType char_pointer_type, func_old_type, int_type, size_type;
 SValue __vstack[1+/*to make bcheck happy*/ VSTACK_SIZE], *vtop;
 
#define vstack  (__vstack + 1)


int Main_main (int argc, char **argv);
#undef ST_DATA
#define ST_DATA

#endif
