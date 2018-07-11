/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   stat.h
 * Author: USUARIO
 *
 * Created on 28 de febrero de 2017, 8:33
 */

#ifndef STAT_H
#define STAT_H

    extern int stat_weighting(char * palabra, int contador, int total);
    extern int stat_matching(char * palabra, char * palabraPrevia);
    extern void statLocal_file(struct SN_env * z, FILE * f_in, FILE * f_outRasgo, FILE * f_outPeso, int total);    
    extern int statting(char * in, char * outRasgo, char * outPeso, int total);
    
#endif /* STAT_H */

