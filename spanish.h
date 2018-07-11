
/* This file was generated automatically by the Snowball to ANSI C compiler */

#ifndef SPANISH_H
#define	SPANISH_H

    #ifdef __cplusplus
    extern "C" {
    #endif

        extern struct SN_env * create_env(void);
        extern void close_env(struct SN_env * z);

        extern int stem(struct SN_env * z);

    #ifdef __cplusplus
    }
    #endif

#endif	/* SPANISH */