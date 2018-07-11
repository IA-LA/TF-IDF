/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
// mezcla.c : Ordenamiento de archivos secuenciales
// Ordena ficheros de texto por orden alfabético de líneas
// Usando el algoritmo de mezcla natural
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Mostrar(FILE *fich);
void Mezcla(FILE *fich);
void Separar(FILE *fich, FILE **aux);
int Mezclar(FILE *fich, FILE **aux);

int mezclar(char * fich)
{
   FILE *fichero;

   fichero = fopen(fich, "r+");
   //puts("Fichero desordenado\n");
   //Mostrar(fichero);
   //puts("Ordenando fichero\n");
   Mezcla(fichero);
   //puts("Fichero ordenado\n");
   //Mostrar(fichero);
   fclose(fichero);
   //system("PAUSE");
   return 0;
}

// Muestra el contenido del fichero "fich"
void Mostrar(FILE *fich)
{
   char linea[128];

   rewind(fich);
   fgets(linea, 128, fich);
   while(!feof(fich)) {
      puts(linea);
      fgets(linea, 128, fich);
   }
}

// Algoritmo de mezcla:
void Mezcla(FILE *fich)
{
   int ordenado;
   FILE *aux[2];

   // Bucle que se repite hasta que el fichero esté ordenado:
   do {
      // Crea los dos ficheros auxiliares para separar los tramos:
      aux[0] = fopen("aux1.txt", "w+");
      aux[1] = fopen("aux2.txt", "w+");
      rewind(fich);
      Separar(fich, aux);
      rewind(aux[0]);
      rewind(aux[1]);
      rewind(fich);
      ordenado = Mezclar(fich, aux);
      fclose(aux[0]);
      fclose(aux[1]);
   } while(!ordenado);
   // Elimina los ficheros auxiliares:
   remove("aux1.txt");
   remove("aux2.txt");
}

// Separa los tramos ordenados alternando entre los ficheros auxiliares:
void Separar(FILE *fich, FILE **aux)
{
   char linea[128], anterior[2][128];
   int salida = 0;

   // Volores iniciales para los últimos valores 
   // almacenados en los ficheros auxiliares
   strcpy(anterior[0], "");
   strcpy(anterior[1], "");
   // Captura la primero línea:
   fgets(linea, 128, fich);
   while(!feof(fich)) {
      // Decide a qué fichero de salida corresponde la línea leída:
      if(salida == 0 && strcmp(linea, anterior[0]) < 0) salida = 1;
      else if(salida == 1 && strcmp(linea, anterior[1]) < 0) salida = 0;
      // Almacena la línea actual como la última añadida:
      strcpy(anterior[salida], linea);
      // Añade la línea al fichero auxiliar:
      fputs(linea, aux[salida]);
      // Lee la siguiente línea:
      fgets(linea, 128, fich);
   }
}

// Mezcla los ficheros auxiliares:
int Mezclar(FILE *fich, FILE **aux)
{
   char ultima[128], linea[2][128], anterior[2][128];
   int entrada;
   int tramos = 0;

   // Lee la primera línea de cada fichero auxiliar:
   fgets(linea[0], 128, aux[0]);
   fgets(linea[1], 128, aux[1]);
   // Valores iniciales;
   strcpy(ultima, "");
   strcpy(anterior[0], "");
   strcpy(anterior[1], "");
   // Bucle, mientras no se acabe ninguno de los ficheros auxiliares (quedan tramos por mezclar):
   while(!feof(aux[0]) && !feof(aux[1])) {
      // Selecciona la línea que se añadirá:
      if(strcmp(linea[0], linea[1]) <= 0) entrada = 0; else entrada = 1;
      // Almacena el valor como el último añadido:
      strcpy(anterior[entrada], linea[entrada]);
      // Añade la línea al fichero:
      fputs(linea[entrada], fich);
      // Lee la siguiente línea del fichero auxiliar:
      fgets(linea[entrada], 128, aux[entrada]);
      // Verificar fin de tramo, si es así copiar el resto del otro tramo:
      if(strcmp(anterior[entrada], linea[entrada]) > 0) {
         if(!entrada) entrada = 1; else entrada = 0;
         tramos++;
         // Copia lo que queda del tramo actual al fichero de salida:
         do {
            strcpy(anterior[entrada], linea[entrada]);
            fputs(linea[entrada], fich);
            fgets(linea[entrada], 128, aux[entrada]);
         } while(!feof(aux[entrada]) && strcmp(anterior[entrada], linea[entrada]) <= 0);
      }
   }

   // Añadir tramos que queden sin mezclar:
   if(!feof(aux[0])) tramos++;
   while(!feof(aux[0])) {
      fputs(linea[0], fich);
      fgets(linea[0], 128, aux[0]);
   }
   if(!feof(aux[1])) tramos++;
   while(!feof(aux[1])) {
      fputs(linea[1], fich);
      fgets(linea[1], 128, aux[1]);
   }
   return(tramos == 1);
}



