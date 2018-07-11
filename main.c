/* 
 * File:   main.c
 * Author: LaurayJavi
 *
 * Created on 23 de febrero de 2017, 20:36
 */

 #include <stdio.h>
    #include <stdlib.h> /* for malloc, free */
    #include <string.h> /* for memmove */

#include <sys/types.h>
#include <dirent.h> 

#include <regex.h>

#include <sys/stat.h>

#include "token.h"
#include "stop.h"
#include "stem.h"
#include "mezcla.h"
#include "stat.h"
#include "statGlobal.h"


    /* Función para devolver un error en caso de que ocurra */
    /* perror() devuelve la cadena S y el error (en cadena de caracteres) que tenga errno */
    void error(const char *s)
    {
      perror (s);
      exit(EXIT_FAILURE);
    }
    
    // Cuenta el numero de lineas del fichero
    // Muestra el contenido del fichero "fich"
    int contarLineas(char * fichero)
    {
       FILE *fich;
       char linea[128];
       int contador=0;

       fich = fopen(fichero, "r+");
       rewind(fich);
       
       fgets(linea, 128, fich);
       contador=1;
       while(!feof(fich)) {
          //puts(linea);
          fgets(linea, 128, fich);
          contador++;
       }
       
       fclose(fich);
       return contador;
    }
    
/* Función que procesa cada archivo */
    /* Para "procesar", o al menos, hacer algo con el archivo, vamos a decir su tamaño en bytes */
    // PROCESADO 1: Tokenizado
    // PROCESADO 2: Stopping
    // PROCESADO 3: Stemming
    // PROCESADO 4: Stating
    /* para ello haremos lo que se ve aquí: http://totaki.com/poesiabinaria/2010/04/tamano-de-un-fichero-en-c/ */
    void procesadoArchivo(char *archivo, int procesado, int num_doc)
    {
      FILE *fich;
      //FILE *foch;
      long ftam;
      char *archivoSalida=(char *)calloc(strlen(archivo), sizeof(char));
      char *archivoSalida2=(char *)calloc(strlen(archivo), sizeof(char));
      
      char *archivoVoc=(char *)calloc(strlen(archivo), sizeof(char));
      char *archivoRep=(char *)calloc(strlen(archivo), sizeof(char));
      char *archivoGlobalRasgo=(char *)calloc(strlen("./global/global.ras"), sizeof(char));
      char *archivoGlobalPeso=(char *)calloc(strlen("./global/global.dtf"), sizeof(char));
      
      /* Procesado usando expresiones regulares */
      {
        regex_t regex;
        regmatch_t regexmatch; 
        int reti,i;
        char msgbuf[100];
        char *archivoBase=(char *)calloc(strlen(archivo), sizeof(char));
        //(symbol *) malloc(lim * sizeof(symbol))
        char *nombreArchivo=0;
        char *extArchivo=0;
        
        strcpy(archivoBase, archivo);
        //nombreArchivo = strtok(archivoBase, "ºª\\!|""@·#$¢%∞&¬/÷(“)”=≠?´¿‚^`[]*+¨´{}Çç,;„.:…<>≤");
        nombreArchivo = strtok(archivoBase, "/.");
        extArchivo = strtok(NULL, "");// ºª\\!|""@·#$¢%∞&¬/÷(“)”=≠?´¿‚^`[]*+¨´{}Çç,;„.:…<>≤");
        
        /* Compile regular expression EERR */
        reti = regcomp(&regex, "^a[[:alnum:]]", 0);
        reti = regcomp(&regex, "^\\w*", 0);
        
        //
        // FICHEROS ENTRADA PROCESADO
        //
        switch (procesado) {
            case 1:
                reti = regcomp(&regex, "\\.txt", 0);
                break;
            case 2:
                reti = regcomp(&regex, "\\.tok", 0);
                break;
            case 3:
                reti = regcomp(&regex, "\\.stp", 0);
                break;
            case 4:
                reti = regcomp(&regex, "\\.stm", 0);
                break;
            case 5:
                reti = regcomp(&regex, "\\.voc", 0);
                break;
            case 6:
                reti = regcomp(&regex, "\\.ter", 0);
                break;
            case 7:
                reti = regcomp(&regex, "\\.voc", 0);
                break;
            case 8:
                reti = regcomp(&regex, "\\.voc", 0);
                break;
            default:
                reti = -1;
                break;
        }
        if (reti) {
            fprintf(stderr, "Could not compile regex\n");
            return;
        }
        else
            printf("Ok. Regexp correct\n");
        
        //
        // MATCHING FICHEROS ENTRADA PROCESADO
        //
        
        /* Execute regular expression */
        //reti = regexec(&regex, "abc", 0, NULL, 0);
        reti = regexec(&regex, archivo, 0, &regexmatch, i);
        if (!reti) {
            puts("Ok. Match");
            
            fich=fopen(archivo, "r");
            if (fich)
              {
                fseek(fich, 0L, SEEK_END);
                ftam=ftell(fich);
                fclose(fich);
                /* Si todo va bien, decimos el tamaño */
                printf ("%30s - %s %s (%ld bytes)\n", archivo, nombreArchivo, extArchivo, ftam);
              }
            else
              /* Si ha pasado algo, sólo decimos el nombre */
              printf ("%30s (No info.)\n", archivo);  
            
        //
        // FICHEROS SALIDA PROCESADO
        //
            /* Nombra al fichero salida */
            strcpy(archivoSalida, archivo);
            strcpy(archivoSalida2, archivo);
            archivoSalida=strtok(archivoSalida, ".");
            archivoSalida2=strtok(archivoSalida2, ".");
            switch (procesado) {
                case 1:
                    archivoSalida=strcat(archivoSalida, ".tok");
                    //foch=fopen(archivoSalida, "w");
                    break;
                case 2:
                    archivoSalida=strcat(archivoSalida, ".stp");
                    break;
                case 3:
                    archivoSalida=strcat(archivoSalida, ".stm");
                    break;
                case 4:
                    archivoSalida=strcat(archivoSalida, ".voc");
                    archivoSalida2=strcat(archivoSalida2, ".rep");
                    break;
                case 5:
                    strcpy(archivoSalida, "");
                    //strcpy(archivoSalida2, "");
                    archivoSalida=strcat(archivoSalida, "./local/global.ter");
                    //archivoSalida2=strcat(archivoSalida2, "./local/global.dft");
                    break;
                case 6:
                    strcpy(archivoSalida, "");
                    strcpy(archivoSalida2, "");
                                        
                    archivoSalida=strcat(archivoSalida, "./local/global.ras");
                    archivoSalida2=strcat(archivoSalida2, "./local/global.dft");                    
                    break;
                case 7:
                    
                    nombreArchivo = strtok(extArchivo, ".");
                    
                    strcpy(archivoVoc, "");
                    strcpy(archivoRep, "");
                    strcpy(archivoGlobalRasgo, "");
                    strcpy(archivoGlobalPeso, "");
                    
                    strcpy(archivoSalida, "");
                    strcpy(archivoSalida2, "");
                    
                    archivoVoc=strcat(archivoVoc, "./local/");                     
                    archivoVoc=strcat(archivoVoc, nombreArchivo);                
                    archivoVoc=strcat(archivoVoc, ".voc");
                    archivoRep=strcat(archivoRep, "./local/");
                    archivoRep=strcat(archivoRep, nombreArchivo);
                    archivoRep=strcat(archivoRep, ".rep");
                    archivoGlobalRasgo=strcat(archivoGlobalRasgo, "./global/global.ras");
                    archivoGlobalPeso=strcat(archivoGlobalPeso, "./global/global.dft");
                    
                    archivoSalida=strcat(archivoSalida, "./global/");
                    archivoSalida=strcat(archivoSalida, nombreArchivo);
                    archivoSalida=strcat(archivoSalida, ".voc");
                    archivoSalida2=strcat(archivoSalida2, "./global/");
                    archivoSalida2=strcat(archivoSalida2, nombreArchivo);   
                    archivoSalida2=strcat(archivoSalida2, ".rep");                 
                    break;
                case 8:
                    
                    nombreArchivo = strtok(extArchivo, ".");
                    
                    strcpy(archivoVoc, "");
                    strcpy(archivoRep, "");
                    strcpy(archivoGlobalRasgo, "");
                    strcpy(archivoGlobalPeso, "");
                    
                    strcpy(archivoSalida, "");
                    strcpy(archivoSalida2, "");
                    
                    archivoVoc=strcat(archivoVoc, "./local/");                     
                    archivoVoc=strcat(archivoVoc, nombreArchivo);                
                    archivoVoc=strcat(archivoVoc, ".voc");
                    archivoRep=strcat(archivoRep, "./local/");
                    archivoRep=strcat(archivoRep, nombreArchivo);
                    archivoRep=strcat(archivoRep, ".rep");
                    archivoGlobalRasgo=strcat(archivoGlobalRasgo, "./global/global.ras");
                    archivoGlobalPeso=strcat(archivoGlobalPeso, "./global/global.dft");
                    
                    archivoSalida=strcat(archivoSalida, "./global/");
                    archivoSalida=strcat(archivoSalida, "finverso");
                    archivoSalida=strcat(archivoSalida, ".voc");
                    archivoSalida2=strcat(archivoSalida2, "./global/");
                    archivoSalida2=strcat(archivoSalida2, "finverso");   
                    archivoSalida2=strcat(archivoSalida2, ".rep");                 
                    break;
                default:
                    break;
            }
            
        //
        // FICHEROS SALIDA PROCESADO
        //
            
        //
        // PROCESADO DEL FICHERO
        //         
            
            /* Initialise the process: */
            switch (procesado) {
                case 1:
                    tokening(archivo,archivoSalida);
                    break;
                case 2:
                    stopping(archivo,archivoSalida);
                    break;
                case 3:
                    mezclar(archivo);
                    stemming(archivo,archivoSalida);
                    mezclar(archivoSalida);
                    mezclar(archivoSalida);
                    printf("Numero de lineas %d\n",contarLineas(archivo)-1);
                    break;
                case 4:
                    //vocabularioLocal();
                    //representacionLocal();
                    statting(archivo,archivoSalida,archivoSalida2,contarLineas(archivo)-1);
                    break;
                case 5:
                    mezclar(archivo);
                    appendGlobal(archivo,archivoSalida);
                    mezclar(archivoSalida);
                    break;
                case 6:
                    //vocabularioGlobal();
                    stattingGlobalRasgos(archivo,archivoSalida,archivoSalida2,contarLineas(archivo)-1);
                    break;
                case 7:
                    //weighting();
                    //representacionGlobal();
                    stattingGlobal(archivoVoc,archivoRep,archivoGlobalRasgo,archivoGlobalPeso,archivoSalida,archivoSalida2,contarLineas(archivo)-1);
                    break;
                case 8:
                    //ficheroInvertidoLocal();
                    //ficheroInvertidoGlobal();
                    stattingGlobalInverso(archivoVoc,archivoRep,archivoGlobalRasgo,archivoGlobalPeso,archivoSalida,archivoSalida2,num_doc);
                    break;
                default:
                    break;
            }
            /* Finishise the process: */

        //
        // PROCESADO DEL FICHERO
        //

        }
        else if (reti == REG_NOMATCH) {
            puts("No match");
        }
        else {
            regerror(reti, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
            exit(1);
        }

        /* Free memory allocated to the pattern buffer by regcomp() */
        regfree(&regex);
        free(archivoBase);
        free(archivoSalida);
        //free(archivoSalida2);
      }
    }

    static int eq(char * s1, char * s2) {
        int s1_len = strlen(s1);
        int s2_len = strlen(s2);
        return s1_len == s2_len && memcmp(s1, s2, s1_len) == 0;
    }

    static void show_options(int n) {
        printf("options are: file [-o[utput] file] [-h[elp]]\n");
        exit(n);
    }

int main(int argc, char** argv) {
    char * in = 0;
    char * out = 0;
    {
        char * s;
        int i = 1;
        while(1) {
            if (i >= argc) break;
            s = argv[i++];
            if (s[0] == '-') {

                if (eq(s, (char *)"-output") || eq(s, (char *)"-o")) {
                    if (i >= argc) {
                        fprintf(stderr, "%s requires an argument\n", s);
                        exit(1);
                    }
                    out = argv[i++];
                } else if (eq(s, (char *)"-help") || eq(s, (char *)"-h")) {
                    show_options(0);
                } else {
                    fprintf(stderr, "%s unknown\n", s);
                    show_options(1);
                }
            }
            else in = s;
        }
    }
    
    /**********************************************************************
     *
     *  PROCESADO LOCAL (Token, Stop, Stem, Vocabulario y representaciones)
     * 
    ***********************************************************************/
    {        
        //Variables de manejo de directorios y archivos        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir (".");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(ent->d_name), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");

                printf ("Procesando LOCAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);
                if ( (extArchivo!=NULL) && (strcmp(extArchivo, "txt")==0) )
                {
                  printf ("Procesando ......... [%s %hhu]\n", ent->d_name, ent->d_type);
                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  //for (i = 1; i < 5; i++) procesadoArchivo(ent->d_name,i);
                  
                  //procesadoArchivoToken(ent->d_name);//, "./fuentes/*.txt");
                  strcpy(archivoBase, ent->d_name);
                  procesadoArchivo(ent->d_name, 1, 0);
                  //procesadoArchivoStop(ent->d_name);//, "./token/*.tok");
                  strcpy(archivoBase, ent->d_name);
                  nombreArchivo = strtok(archivoBase, ".");
                  strcat(nombreArchivo,".tok");
                  procesadoArchivo(nombreArchivo, 2, 0);
                  //procesadoArchivoStop(ent->d_name);//, "./stop/*.stp");
                  strcpy(archivoBase, ent->d_name);
                  nombreArchivo = strtok(archivoBase, ".");
                  strcat(nombreArchivo,".stp");
                  procesadoArchivo(nombreArchivo, 3, 0);
                  //procesadoArchivoStem(ent->d_name);//, "./stem/*.stm");
                  strcpy(archivoBase, ent->d_name);
                  nombreArchivo = strtok(archivoBase, ".");
                  strcat(nombreArchivo,".stm");
                  procesadoArchivo(nombreArchivo, 4, 0);
                  //procesadoArchivoStatsLocal(ent->d_name);//, "./stats/*.voc");
                  //procesadoArchivoStatsGlobal(ent->d_name);//, "./stats/.ras");

                  //procesadoArchivoStatsLocal(ent->d_name);//, "./stats/*.voc");
                  //procesadoArchivoStatsGlobal(ent->d_name);//, "./stats/.ras");
                }
                printf ("Procesando LOCAL FIN\n");

                free(archivoBase);
            }
        }

        closedir (dir);
    }
    
     /**********************************************************
     *
     *  COPIA FICHEROS A LOCAL (Token, Stop, Stem, Voc y Rep)
     * 
    ***********************************************************/
    
    //Preparar los archivos locales en una carpeta Local
    
             //remove("aux2.voc");
             //rename("aux2.voc", "./resultado/aux2.voc");
    
    {
        //Variables de manejo de directorios y archivos
        char dirLocal[100]="./local/";
        char dirGlobal[100]="./global/";
        char directorio[100];
        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir (".");
        DIR * dir2 = opendir ("local");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Miramos que no haya error */
        if (dir2 == NULL) {
            printf ("Cannot open directory '%s'\n", "local");
            printf ("Making directory '%s'\n", "local");
            mkdir("local", S_IRWXU);//mode_t -> S_IRWXU S_IRUSR S_IWUSR S_IXUSR S_ISVTX
        }
        else {    
            closedir (dir2);
        }
        
        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {            
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(ent->d_name), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");

                printf ("Procesando LOCAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);
                if ( (extArchivo!=NULL) && ( (strcmp(extArchivo, "tok")==0) || (strcmp(extArchivo, "stp")==0) || (strcmp(extArchivo, "stm")==0) || (strcmp(extArchivo, "voc")==0) || (strcmp(extArchivo, "rep")==0) ) )
                {
                  printf ("Copiando ......... [%s %s]\n", nombreArchivo, extArchivo);

                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  strcpy(directorio,dirLocal);
                  strcat(directorio,ent->d_name);
                  rename(ent->d_name, directorio);
                }
                 printf ("Procesando LOCAL FIN\n");
                 free(archivoBase);
            }
        }
        closedir (dir);
    }
  
    /**********************************************************
     *
     *  PROCESADO GLOBAL (Terminos, Rasgos y dtf de documento)
     * 
    ***********************************************************/
    {
        //Variables de manejo de directorios y archivos
        char dirLocal[100]="./local/";
        char dirGlobal[100]="./global/";
        char directorio[100];
        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir ("local");
        DIR * dir2 = opendir ("global");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Miramos que no haya error */
        if (dir2 == NULL) {
            printf ("Cannot open directory '%s'\n", "global");
            printf ("Making directory '%s'\n", "global");
            mkdir("global", S_IRWXU);//mode_t -> S_IRWXU S_IRUSR S_IWUSR S_IXUSR S_ISVTX
        }
        else {    
            closedir (dir2);
        }
        
        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {          
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                
                //Directorio de intercambio               
                strcpy(directorio,dirLocal);
                strcat(directorio,ent->d_name);
                
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(directorio), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");
                                
                printf ("Procesando GLOBAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);

                if ( (extArchivo!=NULL) && ( (strcmp(extArchivo, "voc")==0) ) ) //|| (strcmp(extArchivo, "rep")==0) ) )
                {
                  printf ("Procesando ......... [%s %hhu]\n", ent->d_name, ent->d_type);
                 
                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  //for (i = 1; i < 5; i++) procesadoArchivo(ent->d_name,i);
                  
                  //Para todos los archivos del directorio *.voc
                  //append(archivo.ter);
                  //mezclar(archivo.ter);
                  //procesadoArchivoToken(ent->d_name);//, "./local/*.voc");
                  procesadoArchivo(directorio, 5, 0);             
                  strcpy(directorio,dirLocal);
                  strcat(directorio,"global");
                  strcat(directorio,".ter");
                  //procesadoArchivoStop(ent->d_name);//, "./token/*.ter");
                  //pasar(arvhivo.ter) -> (archivo.ras, arvhivo.dft);
                  procesadoArchivo(directorio, 6, 0);
                }
                printf ("Procesando GLOBAL FIN\n");

                free(archivoBase);

            }
        }
        closedir (dir);
        
    }
       
     /**********************************************************
     *
     *  COPIA FICHEROS A GLOBAL (Ter, Ras y Dtf)
     * 
    ***********************************************************/
    
    //Preparar los archivos locales en una carpeta Local
    
             //remove("aux2.voc");
             //rename("aux2.voc", "./resultado/aux2.voc");
    
    {
        //Variables de manejo de directorios y archivos
        char dirLocal[100]="./local/";
        char dirGlobal[100]="./global/";
        char directorio[100];
        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir ("local");
        DIR * dir2 = opendir ("global");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Miramos que no haya error */
        if (dir2 == NULL) {
            printf ("Cannot open directory '%s'\n", "global");
            printf ("Making directory '%s'\n", "global");
            mkdir("global", S_IRWXU);//mode_t -> S_IRWXU S_IRUSR S_IWUSR S_IXUSR S_ISVTX
        }
        else {    
            closedir (dir2);
        }
        
        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {            
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                    
                //Directorio de intercambio               
                strcpy(directorio,dirLocal);
                strcat(directorio,ent->d_name);
                
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(directorio), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");

                printf ("Procesando GLOBAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);
                if ( (extArchivo!=NULL) && ( (strcmp(extArchivo, "ter")==0) || (strcmp(extArchivo, "ras")==0) || (strcmp(extArchivo, "dft")==0) || (strcmp(extArchivo, "fin")==0) ) )
                {
                  printf ("Copiando ......... [%s %s]\n", nombreArchivo, extArchivo);

                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  strcpy(archivoBase,dirGlobal);
                  strcat(archivoBase,ent->d_name);
                  rename(directorio,archivoBase);
                }
                 printf ("Procesando GLOBAL FIN\n");
                 
                 free(archivoBase);
            }
        }
        closedir (dir);
    }
    
    /**********************************************************
     *
     *  PROCESADO GLOBAL (Vocabulario, Pesos y Fichero Inverso)
     * 
    ***********************************************************/
    {
        //Variables de manejo de directorios y archivos
        char dirLocal[100]="./local/";
        char dirGlobal[100]="./global/";
        char directorio[100];
        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir ("local");
        DIR * dir2 = opendir ("global");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Miramos que no haya error */
        if (dir2 == NULL) {
            printf ("Cannot open directory '%s'\n", "global");
            printf ("Making directory '%s'\n", "global");
            mkdir("global", S_IRWXU);//mode_t -> S_IRWXU S_IRUSR S_IWUSR S_IXUSR S_ISVTX
        }
        else {    
            closedir (dir2);
        }
        
        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {          
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                
                //Directorio de intercambio               
                strcpy(directorio,dirLocal);
                strcat(directorio,ent->d_name);
                
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(directorio), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");
                                
                printf ("Procesando GLOBAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);

                if ( (extArchivo!=NULL) && ( (strcmp(extArchivo, "voc")==0) ) ) //|| (strcmp(extArchivo, "rep")==0) ) )
                {
                  printf ("Procesando ......... [%s %hhu]\n", ent->d_name, ent->d_type);
                 
                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  //for (i = 1; i < 5; i++) procesadoArchivo(ent->d_name,i);
                  
                  //procesadoArchivoStatsLocal(ent->d_name);//, "./stats/*.voc");
                  //procesadoArchivoStatsGlobal(ent->d_name);//, "./stats/.ras");
                  //Contruir Voc y Rep de Pesado Global
                  //pesar(archivo.ras, arvhivo.dft);
                  //construirGlobal()
                  procesadoArchivo(directorio, 7, 0);
                                  
                }
                printf ("Procesando GLOBAL FIN\n");

                free(archivoBase);

            }
        }
        closedir (dir);
        
    }
    
    /**********************************************************
     *
     *  PROCESADO GLOBAL (Fichero Inverso)
     * 
    ***********************************************************/
    {
        int i=0;
        //Variables de manejo de directorios y archivos
        char dirLocal[100]="./local/";
        char dirGlobal[100]="./global/";
        char directorio[100];
        
        char * archivoBase=0;
        char * nombreArchivo=0;
        char * extArchivo=0;
        
        /* Ent contendrá información sobre el archivo que se está procesando en cada momento */
        struct dirent * ent;

        /* Con un puntero a DIR abriremos el directorio */
        DIR * dir = opendir ("local");
        DIR * dir2 = opendir ("global");

        /* Miramos que no haya error */
        if (dir == NULL) {
            printf ("Cannot open directory '%s'\n", ".");
            return 1;
        }

        /* Miramos que no haya error */
        if (dir2 == NULL) {
            printf ("Cannot open directory '%s'\n", "global");
            printf ("Making directory '%s'\n", "global");
            mkdir("global", S_IRWXU);//mode_t -> S_IRWXU S_IRUSR S_IWUSR S_IXUSR S_ISVTX
        }
        else {    
            closedir (dir2);
        }
        
        i=0;
        /* Leyendo uno a uno todos los archivos que hay */
        while ((ent = readdir(dir)) != NULL) 
        {          
            if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
                
                //Directorio de intercambio               
                strcpy(directorio,dirLocal);
                strcat(directorio,ent->d_name);
                
                //Archivo de intercambio
                archivoBase = (char *)calloc(strlen(directorio), sizeof(char));

                strcpy(archivoBase, ent->d_name);
                nombreArchivo = strtok(archivoBase, ".");
                extArchivo = strtok(NULL, "");
                                
                printf ("Procesando GLOBAL INICIO\n");
                printf ("Archivo    [%s]\n", ent->d_name);

                if ( (extArchivo!=NULL) && ( (strcmp(extArchivo, "voc")==0) ) ) //|| (strcmp(extArchivo, "rep")==0) ) )
                {
                  printf ("Procesando ......... [%s %hhu]\n", ent->d_name, ent->d_type);
                 
                  /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
                  //for (i = 1; i < 5; i++) procesadoArchivo(ent->d_name,i);
           
                  //Construir Fichero Inverso         
                  //strcpy(directorio,dirLocal);
                  //strcat(directorio,nombreArchivo);
                  //strcat(directorio,".fin");
                  
                  // Incrementa la Cuenta de los Archivos .Voc procesados
                  //para incorporar sus frecuencias en el lugar correspondiente
                  procesadoArchivo(directorio, 8, ++i);
                  
                }
                printf ("Procesando GLOBAL FIN\n");

                free(archivoBase);

            }
        }
        closedir (dir);
        
    }
    
return (EXIT_SUCCESS);
}

