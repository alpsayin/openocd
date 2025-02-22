
/**
 * @brief alptrace.h
 * @code Example usage:
    #define ALP_LOG_FUNC log_info
    #include "../../alptrace.h"
    alp_trace("in here");
 */

 #pragma once

 #include <string.h>
 #include <inttypes.h>
 #include <stdint.h>
 
 #undef CMAKE_SOURCE_DIR
 #define CMAKE_SOURCE_DIR ""
 
 #ifndef ALP_ROOT_SRC_DIR
   #if defined(CONFIG_ARCH_POSIX)
     #define ALP_ROOT_SRC_DIR "/"
   #else
     #define ALP_ROOT_SRC_DIR ""
   #endif
 #endif
 #ifndef ALP_LOG_FUNC
   #include <stdio.h>
   #define ALP_LOG_FUNC printf
 #endif
 
 #define __FILENAME__ (strrchr (__FILE__, '/') ? strrchr (__FILE__, '/') + 1 : __FILE__)
 #define __RELFILE__ (strchr (__FILE__, '/') ? strchr (__FILE__, '/') + 1 : __FILE__)
 
 #define __ALPFILE__ __RELFILE__
 
 /* necessary for pre-processor */
 #define alp_stringify(s) alp_tostring(s)
 #define alp_tostring(s)  #s
 
 
 #define alp_mfgpr(rn) \
   ({ \
     unsigned int _rval = 0U; \
     __asm__ __volatile__("or\t%0,r0," alp_stringify(rn) "\n" : "=d"(_rval)); \
     _rval; \
   })
 
 #define alp_trace(_fmt_str, ...)                     ALP_LOG_FUNC(_fmt_str "\tin\t%s()\tra %p\tin\t" ALP_ROOT_SRC_DIR "%s:%d\n", __VA_OPT__(__VA_ARGS__,) __func__, __builtin_return_address(0), __ALPFILE__, __LINE__)
 #define alp_trace_vars_1(_fmt, _x1)                  alp_trace(alp_stringify(_x1)"=%" _fmt , _x1)
 #define alp_trace_vars_2(_fmt, _x1, _x2)             alp_trace(alp_stringify(_x1)"=%" _fmt ", "alp_stringify(_x2)"=%" _fmt , _x1, _x2)
 #define alp_trace_vars_3(_fmt, _x1, _x2, _x3)        alp_trace(alp_stringify(_x1)"=%" _fmt ", "alp_stringify(_x2)"=%" _fmt ", "alp_stringify(_x3)"=%" _fmt , _x1, _x2, _x3)
 
 #define alp_trace_GET_MACRO(_fmt, _1, _2, _3, _macro, ...)     _macro
 
 /* If VA_ARGS has 2 element, 4th arg (_macro) is STAT_MAP_ROW_1
  * If VA_ARGS has 3 elements, 4th arg (_macro) is STAT_MAP_ROW_2
  * Because of the way __VA_ARGS__ pushes the alp_trace_GET_MACRO's last args.
  */
 #define _alp_trace_vars(...)                         alp_trace_GET_MACRO(__VA_ARGS__, alp_trace_vars_3, alp_trace_vars_2, alp_trace_vars_1)
 
 /**
  * @brief Variadic macro that provides python-like print similar to '{var=}'
  * Examples:   alp_trace_vars(PRIx32, var1);
  *             alp_trace_vars(PRIx32, var1, var2);
  *             alp_trace_vars(PRIx32, var1, var2, var3);
  */
 #define alp_trace_vars(...)                          _alp_trace_vars(__VA_ARGS__)(__VA_ARGS__)
 
 #define alp_trace_x32(...)                           alp_trace_vars(PRIx32, __VA_ARGS__)
 #define alp_trace_u32(...)                           alp_trace_vars(PRIu32, __VA_ARGS__)
 #define alp_trace_d32(...)                           alp_trace_vars(PRId32, __VA_ARGS__)
 
 #define alp_currentThread() \
   do { \
     if (IS_ENABLED(CONFIG_CORO_OVER_ZEPHYR)) { \
       printf("%s\n", k_thread_name_get(k_current_get())); \
     } else { \
       printf("%p\n", CURRENT_CORO()); \
     } \
   } while (0)
 
 #define alp_PRIx64          "%0x%0x"
 
 #define alp_PRIx64_HI32(_x) ((uint32_t)((((_x) & 0xffffffff00000000) >> 32) & 0xffffffff))
 #define alp_PRIx64_LO32(_x) ((uint32_t)((_x) & 0xffffffff))
 
 #ifndef BYTES_PER_LINE
 #define BYTES_PER_COL_32BIT  (4)
 #define BYTES_PER_COL_64BIT  (8)
 #define BYTES_PER_LINE_32BIT (16)
 #define BYTES_PER_LINE_64BIT (32)
 #endif
 
 #define alp_dump                alp_dump_le32
 
 #define alp_dump_header(x, len) alp_trace("Dumping " alp_tostring(len) " (%d) bytes of " alp_tostring(x), len)
 
 #define alp_dump_be32(x, len) \
   do { \
     const uint8_t const *c; \
     alp_dump_header(x, len); \
     c = (uint8_t const *)x; \
     for (size_t i = 0; i < len; i++) { \
       if ((i) % BYTES_PER_LINE_32BIT == 0) { \
         ALP_LOG_FUNC("%08" PRIx32 ": ", (uint32_t)i); \
       } \
       ALP_LOG_FUNC("%02" PRIx8, c[i]); \
       if (i != 0 && (i + 1) % BYTES_PER_COL_32BIT == 0) { \
         ALP_LOG_FUNC(" "); \
       } \
       if ((i + 1) % BYTES_PER_LINE_32BIT == 0) { \
         ALP_LOG_FUNC("\n"); \
       } \
     } \
     ALP_LOG_FUNC("\n"); \
   } while (0)
 
 
 #define alp_dump_le32(x, len) \
   do { \
     const uint8_t const *c; \
     alp_dump_header(x, len); \
     c = (const uint8_t *)x; \
     for (size_t i = 0; i < len; i++) { \
       if ((i) % BYTES_PER_LINE_32BIT == 0) { \
         ALP_LOG_FUNC("%08" PRIx32 ": ", (uint32_t)i); \
       } \
       ALP_LOG_FUNC("%02" PRIx8, c[(i & (~0b11)) | ((~i) & 0b11)]); \
       if (i != 0 && (i + 1) % BYTES_PER_COL_32BIT == 0) { \
         ALP_LOG_FUNC(" "); \
       } \
       if ((i + 1) % BYTES_PER_LINE_32BIT == 0) { \
         ALP_LOG_FUNC("\n"); \
       } \
     } \
     ALP_LOG_FUNC("\n"); \
   } while (0)
 
 #if defined(CONFIG_ARCH_RISCV)
 #define RISCV_WRITE_MSCRATCH_IMM(_wr_val) \
   do { /* 5-bit field check */ \
     _Static_assert(_wr_val <= 0x1f); \
     asm volatile ("csrwi mscratch, "#_wr_val); \
   } while (0)
 
 #define RISCV_WRITE_MSCRATCH(_wr_from) \
   do { \
     asm volatile ("csrw mscratch, %0" :: "r"(_wr_from)); \
   } while (0)
 
 #define RISCV_READ_MSCRATCH(_rd_into) \
   do { \
     asm volatile ("csrr %0, mscratch" : "=r"(_rd_into)); \
   } while (0)
 #else
 #define RISCV_WRITE_MSCRATCH_IMM(_wr_val) printf("*** hit line %d\n", _wr_val)
 #define RISCV_WRITE_MSCRATCH(_wr_from) printf("*** hit variable "#_wr_from"=%d\n", _wr_from)
 #define RISCV_READ_MSCRATCH(_rd_into)
 #endif
