/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/*------------------------------------------------------------------------*/

#ifndef _STRFUNC
#define _STRFUNC

#define _USE_XFUNC_OUT	1	/* 1: Use output functions */
#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */

#define _USE_XFUNC_IN	1	/* 1: Use input function */
#define	_LINE_ECHO		0	/* 1: Echo back input chars in xgets function */


#if _USE_XFUNC_OUT
    #define xdev_out(func) xfunc_out = (void(*)(unsigned char))(func)
    extern void (*xfunc_out)(unsigned char);
    void xputc(char c);
    void xputs(const char *str);
    void xfputs(void (*func)(unsigned char), const char *str);
    void xprintf(const char *fmt, ...);
    void xsprintf(char *buff, const char *fmt, ...);
    void xfprintf(void (*func)(unsigned char), const char	*fmt, ...);
    void put_dump(const void *buff, unsigned long addr, int len, int width);
    #define DW_CHAR		sizeof(char)
    #define DW_SHORT	sizeof(short)
    #define DW_LONG		sizeof(long)
#endif

#if _USE_XFUNC_IN
    #define xdev_in(func) xfunc_in = (unsigned char(*)(void))(func)
    extern unsigned char (*xfunc_in)(void);
    int xgets(char *buff, int len);
    int xfgets(unsigned char (*func)(void), char *buff, int len);
    int xatoi(char **str, long *res);
#endif

    /*----------------------------------------------/
    /  xprintf - Formatted string output
    /----------------------------------------------*/
    /*  xprintf("%d", 1234);            "1234"
        xprintf("%6d,%3d%%", -200, 5);  "  -200,  5%"
        xprintf("%-6u", 100);           "100   "
        xprintf("%ld", 12345678L);      "12345678"
        xprintf("%04x", 0xA3);          "00a3"
        xprintf("%08LX", 0x123ABC);     "00123ABC"
        xprintf("%016b", 0x550F);       "0101010100001111"
        xprintf("%s", "String");        "String"
        xprintf("%-4s", "abc");         "abc "
        xprintf("%4s", "abc");          " abc"
        xprintf("%c", 'a');             "a"
        xprintf("%f", 10.0);            <xprintf lacks floating point support>
    */

#endif
