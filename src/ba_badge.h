/* Import a binary file */
#define IMPORT_BIN(sect, file, sym) asm (\
    ".balign 4\n"                           /* Word alignment */\
    ".global " #sym "\n"                    /* Export the object address */\
    #sym ":\n"                              /* Define the object label */\
    ".incbin \"" file "\"\n"                /* Import the file */\
    ".global _sizeof_" #sym "\n"            /* Export the object size */\
    ".set _sizeof_" #sym ", . - " #sym "\n" /* Define the object size */\
    ".balign 4\n"                           /* Word alignment */\
    )
    // ".section \".text\"\n")                 /* Restore section */

IMPORT_BIN("FLASH", "src/ba_badge.bin", ba_badge);
/* Declaration of symbols (any type can be used) */
extern const unsigned char ba_badge[], _sizeof_ba_badge[];