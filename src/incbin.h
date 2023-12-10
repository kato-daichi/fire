#pragma once
#include <climits>

#if defined(__AVX512BW__) || defined(__AVX512CD__) || defined(__AVX512DQ__) || \
    defined(__AVX512ER__) || defined(__AVX512PF__) || defined(__AVX512VL__) || \
    defined(__AVX512F__)
#define INCBIN_ALIGNMENT_INDEX 6
#elif defined(__AVX__) || defined(__AVX2__)
#define INCBIN_ALIGNMENT_INDEX 5
#elif defined(__SSE__) || defined(__SSE2__) || defined(__SSE3__) ||     \
    defined(__SSSE3__) || defined(__SSE4_1__) || defined(__SSE4_2__) || \
    defined(__neon__)
#define INCBIN_ALIGNMENT_INDEX 4
#elif ULONG_MAX != 0xffffffffu
#define INCBIN_ALIGNMENT_INDEX 3
#else
#define INCBIN_ALIGNMENT_INDEX 2
#endif

#define INCBIN_ALIGN_SHIFT_0 1
#define INCBIN_ALIGN_SHIFT_1 2
#define INCBIN_ALIGN_SHIFT_2 4
#define INCBIN_ALIGN_SHIFT_3 8
#define INCBIN_ALIGN_SHIFT_4 16
#define INCBIN_ALIGN_SHIFT_5 32
#define INCBIN_ALIGN_SHIFT_6 64

#define INCBIN_ALIGNMENT                                        \
  INCBIN_CONCATENATE(INCBIN_CONCATENATE(INCBIN_ALIGN_SHIFT, _), \
                     INCBIN_ALIGNMENT_INDEX)
#define INCBIN_STR(X) #X
#define INCBIN_STRINGIZE(X) INCBIN_STR(X)
#define INCBIN_CAT(X, Y) X##Y
#define INCBIN_CONCATENATE(X, Y) INCBIN_CAT(X, Y)
#define INCBIN_EVAL(X) X
#define INCBIN_INVOKE(N, ...) INCBIN_EVAL(N(__VA_ARGS__))

#define INCBIN_MACRO ".incbin"

#ifndef _MSC_VER
#define INCBIN_ALIGN __attribute__((aligned(INCBIN_ALIGNMENT)))
#else
#define INCBIN_ALIGN __declspec(align(INCBIN_ALIGNMENT))
#endif

#ifdef __GNUC__
#define INCBIN_ALIGN_HOST ".balign " INCBIN_STRINGIZE(INCBIN_ALIGNMENT) "\n"
#define INCBIN_ALIGN_BYTE ".balign 1\n"
#elif defined(INCBIN_ARM)
#define INCBIN_ALIGN_HOST \
  ".align " INCBIN_STRINGIZE(INCBIN_ALIGNMENT_INDEX) "\n"
#define INCBIN_ALIGN_BYTE ".align 0\n"
#else
#define INCBIN_ALIGN_HOST ".align " INCBIN_STRINGIZE(INCBIN_ALIGNMENT) "\n"
#define INCBIN_ALIGN_BYTE ".align 1\n"
#endif

#if defined(__cplusplus)
#define INCBIN_EXTERNAL extern "C"
#define INCBIN_CONST extern const
#else
#define INCBIN_EXTERNAL extern
#define INCBIN_CONST const
#endif

#if !defined(INCBIN_OUTPUT_SECTION)
#if defined(__APPLE__)
#define INCBIN_OUTPUT_SECTION ".const_data"
#else
#define INCBIN_OUTPUT_SECTION ".rodata"
#endif
#endif

#define INCBIN_SECTION ".section " INCBIN_OUTPUT_SECTION "\n"
#define INCBIN_GLOBAL(NAME) \
  ".global " INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME "\n"

#define INCBIN_INT ".int "

#if defined(__USER_LABEL_PREFIX__)
#define INCBIN_MANGLE INCBIN_STRINGIZE(__USER_LABEL_PREFIX__)
#else
#define INCBIN_MANGLE ""
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#define INCBIN_TYPE(NAME)
#else
#define INCBIN_TYPE(NAME) \
  ".type " INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME ", @object\n"
#endif

#define INCBIN_BYTE ".byte "

#define INCBIN_STYLE_CAMEL 0
#define INCBIN_STYLE_SNAKE 1

#if !defined(INCBIN_PREFIX)
#define INCBIN_PREFIX g
#endif

#if !defined(INCBIN_STYLE)
#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#endif

#define INCBIN_STYLE_0_DATA Data
#define INCBIN_STYLE_0_END End
#define INCBIN_STYLE_0_SIZE Size
#define INCBIN_STYLE_1_DATA _data
#define INCBIN_STYLE_1_END _end
#define INCBIN_STYLE_1_SIZE _size

#define INCBIN_STYLE_IDENT(TYPE)                                   \
  INCBIN_CONCATENATE(INCBIN_STYLE_,                                \
                     INCBIN_CONCATENATE(INCBIN_EVAL(INCBIN_STYLE), \
                     INCBIN_CONCATENATE(_, TYPE)))

#define INCBIN_STYLE_STRING(TYPE) INCBIN_STRINGIZE(INCBIN_STYLE_IDENT(TYPE))

#define INCBIN_GLOBAL_LABELS(NAME, TYPE)                                 \
  INCBIN_INVOKE(                                                         \
      INCBIN_GLOBAL,                                                     \
      INCBIN_CONCATENATE(NAME, INCBIN_INVOKE(INCBIN_STYLE_IDENT, TYPE))) \
  INCBIN_INVOKE(                                                         \
      INCBIN_TYPE,                                                       \
      INCBIN_CONCATENATE(NAME, INCBIN_INVOKE(INCBIN_STYLE_IDENT, TYPE)))

#define INCBIN_EXTERN(NAME)                                                   \
  INCBIN_EXTERNAL const INCBIN_ALIGN unsigned char INCBIN_CONCATENATE(        \
      INCBIN_CONCATENATE(INCBIN_PREFIX, NAME), INCBIN_STYLE_IDENT(DATA))[];   \
  INCBIN_EXTERNAL const INCBIN_ALIGN unsigned char *const INCBIN_CONCATENATE( \
      INCBIN_CONCATENATE(INCBIN_PREFIX, NAME), INCBIN_STYLE_IDENT(END));      \
  INCBIN_EXTERNAL const unsigned int INCBIN_CONCATENATE(                      \
      INCBIN_CONCATENATE(INCBIN_PREFIX, NAME), INCBIN_STYLE_IDENT(SIZE))

#ifdef _MSC_VER
#define INCBIN(NAME, FILENAME) INCBIN_EXTERN(NAME)
#else
#define INCBIN(NAME, FILENAME)                                                                     \
  __asm__(                                                                                         \
      INCBIN_SECTION INCBIN_GLOBAL_LABELS(                                                         \
          NAME, DATA) INCBIN_ALIGN_HOST INCBIN_MANGLE                                              \
          INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME INCBIN_STYLE_STRING(                               \
              DATA) ":\n" INCBIN_MACRO " \"" FILENAME                                              \
                    "\"\n" INCBIN_GLOBAL_LABELS(                                                   \
                        NAME, END) INCBIN_ALIGN_BYTE INCBIN_MANGLE                                 \
                        INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME INCBIN_STYLE_STRING(                 \
                            END) ":\n" INCBIN_BYTE "1\n" INCBIN_GLOBAL_LABELS(NAME,                \
                                                                              SIZE)                \
                            INCBIN_ALIGN_HOST INCBIN_MANGLE                                        \
                                INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME INCBIN_STYLE_STRING(         \
                                    SIZE) ":\n" INCBIN_INT INCBIN_MANGLE                           \
                                    INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME INCBIN_STYLE_STRING(     \
                                        END) " - " INCBIN_MANGLE                                   \
                                        INCBIN_STRINGIZE(INCBIN_PREFIX) #NAME INCBIN_STYLE_STRING( \
                                            DATA) "\n" INCBIN_ALIGN_HOST                           \
                                                  ".text\n");                                      \
  INCBIN_EXTERN(NAME)
#endif
