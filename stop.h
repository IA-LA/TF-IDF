/* 
 * File:   stop.h
 * Author: LaurayJavi
 *
 * Created on 25 de febrero de 2017, 11:31
 */

#ifndef STOP_H
#define	STOP_H


    extern int stop_words(char * palabra);
    extern void stop_file(struct SN_env * z, FILE * f_in, FILE * f_out);
    extern int stopping(char * in, char * out);

    
#endif	/* STOP_H */

