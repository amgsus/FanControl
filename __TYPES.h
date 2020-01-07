#ifndef C51_TYPES_H
#define C51_TYPES_H

#define TRUE  (1)
#define FALSE (0)

typedef            void VOID;

// 8-bit

typedef unsigned   char BYTE;
typedef unsigned   char UINT8;
typedef unsigned   char U8;
typedef signed     char INT8;
typedef signed     char SINT8;
typedef signed     char S8;
typedef signed     char I8;
typedef signed     char SHORTINT;
typedef            char CHAR;

// 16-bit

typedef unsigned  short WORD;
typedef unsigned  short UINT16;
typedef signed    short SINT16;
typedef signed    short INT16;
typedef unsigned  short U16;
typedef signed    short S16;
typedef signed    short I16;
typedef signed    short SMALLINT;
typedef           short SHORT;

// 16(32)-bit (platform dependent)

typedef unsigned    int UINT;
typedef signed      int INTEGER;
typedef signed      int SINT;
typedef             int INT;

// 32-bit

typedef unsigned   long DWORD;
typedef unsigned   long LONGWORD;
typedef unsigned   long CARDINAL;
typedef unsigned   long UINT32;
typedef unsigned   long U32;
typedef signed     long INT32;
typedef signed     long SINT32;
typedef signed     long S32;
typedef signed     long I32;
typedef            long LONG;

// Floats

typedef           float FLOAT;
typedef           float SINGLE;
typedef          double DOUBLE;     // C51: Double is the same as float (single precision).

// Boolean (platform dependent)

#ifdef __C51__
typedef            BYTE BOOLEAN;
typedef             bit BIT;
#else
typedef           _Bool BOOLEAN;
typedef         BOOLEAN BIT;
#endif

typedef             BIT BOOL;

typedef           CHAR* PCHAR;
typedef           VOID* PVOID;
typedef           PVOID POINTER;
typedef           PVOID PTR;

// WinAPI-style long pointers:

typedef       VOID* LPVOID;
typedef       BYTE* LPBYTE;
typedef       CHAR* LPCHAR;
typedef       INT8* LPINT8;
typedef      SINT8* LPSINT8;
typedef      UINT8* LPUINT8;
typedef       WORD* LPWORD;
typedef      INT16* LPINT16;
typedef     SINT16* LPSINT16;
typedef     UINT16* LPUINT16;
typedef      DWORD* LPDWORD;
typedef   LONGWORD* LPLONGWORD;
typedef      INT32* LPINT32;
typedef     SINT32* LPSINT32;
typedef     UINT32* LPUINT32;
typedef const void* LPCVOID; // Cannot use 'typedef const LPVOID' here.

# endif // ------------------------------------------- End of Include Guard ---
