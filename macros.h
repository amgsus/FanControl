#ifndef MACROS_H
#define MACROS_H

// ----------------------------------------------------------------------------

#ifndef BIT
#  define BIT(n) (1U << (n))
#endif

#define BIN(seq) (0b##seq##U)

#ifndef BCLR
#  define BCLR(value, bit) (value &= ~BIT(bit))
#endif

#ifndef BSET
#  define BSET(value, bit) (value |=  BIT(bit))
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define SWTIMER_TICK(var, reload) \
    (!(--var) ? (var = (reload), 1) : 0)

#define ASSIGN_IF_NOT_NULL(ptr, value) \
    do { \
        if (ptr) { \
            *(ptr) = (value); \
        } \
    } while (0)

#define LOBYTE(word) ((BYTE)((word)))
#define HIBYTE(word) ((BYTE)((WORD)(word) >> 8U))

# endif // ------------------------------------------- End of Include Guard ---
