/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mezcla.h
 * Author: USUARIO
 *
 * Created on 1 de marzo de 2017, 18:57
 */

#ifndef MEZCLA_H
#define MEZCLA_H

int mezclar(char * fich);
void Mostrar(FILE *fich);
void Mezcla(FILE *fich);
void Separar(FILE *fich, FILE **aux);
int Mezclar(FILE *fich, FILE **aux);


#endif /* MEZCLA_H */

