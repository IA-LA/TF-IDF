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

#ifndef STATGLOBAL_H
#define STATGLOBAL_H

    extern int appendGlobal(char * archivo, char * archivoSalida);
    extern int statGlobal_weighting(char * palabra, int contador, int total);
    extern int statGlobal_matching(char * palabra, char * palabraPrevia);
    extern void rasgosGlobal_file(struct SN_env * z, FILE * f_in, FILE * f_outRasgo, FILE * f_outPeso, int total);
    extern void statGlobal_file(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_inGlobalRasgo, FILE * f_inGlobalPeso, FILE * f_outRasgo, FILE * f_outPeso, int total);
    extern void inversoGlobal_file(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int total);
    extern void inversoGlobal_fileCrear(FILE * f_inGlobalRasgo, FILE * f_inGlobalPeso, FILE * f_outRasgo, FILE * f_outPeso, int num_doc);  
    extern int stattingGlobalRasgos(char * in, char * outRasgo, char * outPeso, int total);
    extern int stattingGlobal(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total);
    extern int stattingGlobalInversoCrear(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total);
    extern int stattingGlobalInverso(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total);

#endif /* STATGLOBAL_H */

