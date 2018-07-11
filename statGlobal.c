  #include <stdio.h>
    #include <stdlib.h> /* for malloc, free */
    #include <string.h> /* for memmove */
    #include <math.h> /* for log */

    #include "api.h"
    #include "spanish.h"


    /* This derives from the source file driver.template */

    /* A simple driver for a single ANSI C generated Spanish stemmer.

       Following compilation with

           g++ -o prog.cpp q\*.c
           c++ -o prog.cpp q\*.c
     
        Q folder must contain:
     
        To avoid this error message with gcc compiler:
        
            "Undefined symbols for architecture x86_64:"
     
       The command line syntax is

           ./prog file [-o[utput] file] -h[elp]]

       The first argument gives the input file, which consists of a list of words
       to be stemmed, one per line. (Words must be in lower case.) If omitted, stdin
       is used.

       The output is sent to stdout by default, otherwise to the -output file.

    */

    /* Función de copiado de ficheros */
    /* Copia dos documentos */
    extern int copiaGlobal(char * archivo, char * archivoSalida){
        FILE *pFile;
        FILE *pFile2;
        char buffer[128];

        pFile=fopen(archivo, "r");
        pFile2=fopen(archivoSalida, "w");
        rewind(archivo);
        rewind(archivoSalida);
        fflush(archivo);
        fflush(archivoSalida);
        
        if(pFile==NULL) {
            perror("Error opening file.");
        }
        else {
            while(fgets(buffer, sizeof(buffer), pFile)) {
                fprintf(pFile2, "%s", buffer);
            }
            fprintf(pFile2, "%c", '\n');
        }
        fclose(pFile);
        fclose(pFile2);
    }

    /* Función de adjuntado de ficheros */
    /* Une dos documento en uno solo */
    extern int appendGlobal(char * archivo, char * archivoSalida){
        FILE *pFile;
        FILE *pFile2;
        char buffer[128];

        pFile=fopen(archivo, "r");
        pFile2=fopen(archivoSalida, "a");
        if(pFile==NULL) {
            perror("Error opening file.");
        }
        else {
            while(fgets(buffer, sizeof(buffer), pFile)) {
                fprintf(pFile2, "%s", buffer);
            }
            fprintf(pFile2, "%c", '\n');
        }
        fclose(pFile);
        fclose(pFile2);
    }

    /* Función global de pesado TF-IDF */
    /* Devuelve la frecuencia del Rasgo contado en un total de documentos */
    extern float statGlobal_weighting(float frecuencia, int apariciones, int total)
    {         
        /* Initialise the process: */
        if(apariciones>0)
            return (float)((float)frecuencia * ( (float)total/(float)apariciones) );
        else
            return 0.0;
        /* Finishise the process: */
    }
        
    extern int statGlobal_matching(char * palabra, char * palabraPrevia)
    {   
        int procesado;
        
        for (procesado = 0; procesado < 1; procesado++) {
            if(strcmp(palabra, palabraPrevia)==0){
                procesado = 666;
                break;
            }
        }
        /* Initialise the process: */
        switch (procesado) {
            case 666:
                return 0;
                break;
            default:
                return 1;
                break;
        }
        /* Finishise the process: */
        return 1;
    }

    extern void rasgosGlobal_file(struct SN_env * z, FILE * f_in, FILE * f_outRasgo, FILE * f_outPeso, int total) {
        #define INC 10
        int lim = INC;
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(1 * sizeof(symbol));//Memoria del token anterior
        
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        for (int j = 0; j < lim; j++) b0[j]='\0';
        
        //Ordenar fichero
        //mezclar(f_in);
        
        //Lee Token caracter a caracter
        while(1) {
            int ch = getc(f_in);
            if (ch == EOF) {

                for (int j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                //fprintf(f_outRasgo, "\n");

                fprintf(f_outPeso, "%d", contRasgo);
                //fprintf(f_outPeso, "\n");
                
                free(b0);
                free(b); return;
            }
            {
                int i = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == ' ' || ch == ',' || ch == ';' || ch == '.' || ch == ':' || ch == '-' || ch == '_' || ch == '\\' || ch == '!' || ch == '|' || ch == '"' || ch == '\'' || ch == '(' || ch == ')' ||  ch == '+' ||  ch == '-' || ch == EOF) break;
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        //for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    b[i] = ch; i++;
                    ch = getc(f_in);
                }

                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int j=i;j<lim;j++) b[j]='\0';
                /* FIN Tratamiento de cada token */
                
                {
                    int j;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)!=0)){

                        for (j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                        fprintf(f_outRasgo, "\n");
                        
                        fprintf(f_outPeso, "%d", contRasgo);
                        fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        contRasgo=1;
                    }
                    else
                        //if(peso==0)
                            contRasgo++;
                        
                    //Actualiza rasgo previo
                    free(b0);
                    b0 = (symbol *) malloc(lim* sizeof(symbol));
                    for(int j=0;j<lim;j++) b0[j]=b[j];
                    
                }
            }
        }
        free(b);
        free(b0);
        close_env(z0);
    }
    
    extern void statGlobal_file(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_inGlobalRasgo, FILE * f_inGlobalPeso, FILE * f_outRasgo, FILE * f_outPeso, int total)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;

        for (int j = 0; j < lim; j++) b0[j]='\0';
        for (int j = 0; j < lim; j++) b01[j]='\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*****************************      
        //Lee Token caracter a caracter
        //Ficheros locales Voc y Rep
        //
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {
                        b[i] = ch; 
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {
                        b1[j] = ch1; 
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */
                
                // *************************
                //Examina Rasgo y Apariciones
                //Ficheros globales Ras y Dtf
                //
                {                  
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_inGlobalRasgo);
                        ch1 = getc(f_inGlobalPeso);

                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;

                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }
                            if(fin2==0) {
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_inGlobalRasgo);
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_inGlobalPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                    }
                }                
                
                //****************************              
                //Copia Rasgo y Peso a fichero
                //Ficheros globales Voc y Rep
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    j=0;                        
                    //Inicializa contador al Rasgo actual
                    contRasgo=1;
                    
                    //Problema con el redondeo !!!!!!!!!!!!!!!!
                    //a partir del 8 decimal..........
                    sscanf(b1, "%f", &frecuencia);
                    //sscanf(apariciones, "%d", (char *)&b01);
                    for(int k=j;k<10;k++) buffer[k]=b1[k];
                    frecuencia=strtof(buffer,NULL);
                    frecuencia=strtof(b1,NULL);
                    frecuencia=atof(b1);                    
                    apariciones=atoll((char *)b01);
                    frecuencia=atoff(b1);
                    //apariciones=atoi((const char*)b01);
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Escribe el Rasgo
                        for (int j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                        fprintf(f_outRasgo, "\n");
                                     
                        //Escribe el peso Global del Rasgo
                        peso = statGlobal_weighting(frecuencia, apariciones, total);
                        fprintf(f_outPeso, "%.8f", peso);
                        fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        contRasgo=1;
                        
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }

    extern void inversoGlobal_fileCrear(FILE * f_inGlobalRasgo, FILE * f_inGlobalPeso, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        
        for (int j = 0; j < lim; j++) b0[j]='\0';
        for (int j = 0; j < lim; j++) b01[j]='\0';

        //*********************************  
        //Lee Token caracter a caracter

                // *************************
                //Examina Rasgo y Apariciones
                //Ficheros globales Ras y Dtf
                //
                {                  
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(1) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_inGlobalRasgo);
                        ch1 = getc(f_inGlobalPeso);

                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;

                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                            
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_inGlobalRasgo);
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_inGlobalPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                          
                        //*********************************************
                        //Fichero local invertido Fin
                        //Adjunta Num_Documento y Frecuencia a Rasgo =>
                        //
                        {                     
                            //Escribe el Rasgo
                            for (int j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                            fprintf(f_outRasgo, "%s", " -> ");

                            for ( int j = 0 ; j < num_doc ; j++ ){
                                fprintf(f_outRasgo, "%s", "\tdoc");
                                fprintf(f_outRasgo, "%d", j);
                                fprintf(f_outRasgo, "%c", ':');
                                //for (int j = 0; j < strlen(b1); j++) fprintf(f_outRasgo, "%c", b1[j]);
                                fprintf(f_outRasgo, "%s", "0.00000000");
                            }
                            fprintf(f_outRasgo, "%c", '\n');

                            /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                            for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                            for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                            for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                        }
                    }
                }

        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
    
    extern void inversoGlobal_file(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        
        int write_at=0; int read_at=0;
        
        for (int k = 0; k < INC; k++) b[k] = '\0';
        for (int k = 0; k < INC; k++) b1[k] = '\0';
        for (int j = 0; j < INC; j++) b0[j] = '\0';
        for (int j = 0; j < INC; j++) b01[j] = '\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*********************************  
        //Lee Token caracter a caracter
        //Ficheros origen Voc y Rep locales
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {//Rasgo a buscar
                        b[i] = ch;
                        b[i+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {//Frecuencia del Rasgo
                        b1[j] = ch1;
                        b1[j+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                // Rasgo y Frecuencia del vocabulario
                //obtenido del fichero num_doc
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */

                // ********************************************
                //Busca Rasgo en F inverso 
                //Ficheros globales finverso.voc y finverso.rep
                //
                {                 
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_outRasgo);
                        ch1 = getc(f_outPeso);
                        
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
    
                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;
    
                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '-' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch == '-' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                                       
                                //Toma posicion lectura/escritura fichero de rasgos
                                //fgetpos (pFile,&pos);
                                //fgetpos(f_outRasgo,posR);
                                //fgetpos(f_outRasgo,posW);
                                //read_at = ftell(f_outRasgo);
                                
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_outRasgo);
                                                                                   
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
                                
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_outPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                                                               
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                                                
    //read_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, write_at, SEEK_SET);
    
        
                    }
                }
                
                //*********************************************
                //Tratamiento de linea de Rasgo correspondiente
                //Sobre el fichero local invertido Finvertido.voc
                //Adjunta Num_Documento y Frecuencia a Rasgo =>
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    int dosPuntos=0;
                    
                    ch=ch1=0;
                    j=0;            
                    
                    //Inicializa contador al Rasgo actual
                    //contRasgo=1;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Problema con el redondeo !!!!!!!!!!!!!!!!
                        //a partir del 8 decimal..........
                        sscanf(b1, "%f", &frecuencia);
                        //sscanf(apariciones, "%d", (char *)&b01);
                        for(int k=j;k<10;k++) buffer[k]=b1[k];
                        frecuencia=strtof(buffer,NULL);
                        frecuencia=strtof(b1,NULL);
                        frecuencia=atof(b1);                    
                        apariciones=atoll((char *)b01);
                        frecuencia=atoff(b1);
                        //apariciones=atoi((const char*)b01);

                        //Coloca el puntero de escritura del Fichero
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posW);
                        //fseek(f_outRasgo, write_at, SEEK_SET);                   
                        
while ((ch = fgetc(f_outRasgo)) != '\n') {
//if(ch!='\0') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    // ASCII 97-122
    if (('a' <= ch) && (ch <= 'z')) {
      fputc(ch - 32, f_outRasgo);
    } else {// ASCII 58
      if ((':' == ch) && (dosPuntos < 11)) { // < 5)) {// < 3)) {//Max numero de documentos
          apariciones=1;
          if(dosPuntos<10) {//4) { //3) {//Numero de documentos
              dosPuntos++;
          }
          else {
              dosPuntos=1;
          }
          fputc(ch, f_outRasgo);    
      } else {// ASCII 48-57
        if (((('0' <= ch) && (ch <= '9'))||(ch=='.')) && (apariciones == 1)) {//Documento(num_doc)
            if(dosPuntos == num_doc) {// Documento a trabajar (int num_doc)
                fputc(b1[j], f_outRasgo);//Introduce la frecuencia
                j++;
            }
            else {
                fputc(ch, f_outRasgo);
                apariciones=0; j=0;
            }
        } else {
            apariciones=0; j=0;
            fputc(ch, f_outRasgo);
        }
      }
    }
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
//}
if(ch='\n') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    fputc(ch, f_outRasgo);
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
                        
                        //Busca el sitio de escritura en el fichero inverso
                        //for (int j = 0; j < num_doc ; j++) while(ch!=':') ch=getc(f_outRasgo);
                        //Escribe el Rasgo
                        //for (int j = 0; j < strlen(b); j++) fprintf(f_outRasgo, "%c", b[j]);
                        //if(num_doc==1) fprintf(f_outRasgo, "%s", " -> ");
                        //fprintf(f_outRasgo, "%s", "\tdoc");
                        //fprintf(f_outRasgo, "%d", num_doc);
                        //fprintf(f_outRasgo, "%c", ':');
                        //for (int j = 0; j < 10 ; j++) fprintf(f_outRasgo, "%c", b1[j]);
                        //fprintf(f_outRasgo, "%8f", frecuencia);
                        //fprintf(f_outRasgo, "%c", '\t');
                        //fprintf(f_outRasgo, "%c", '\n');
                                                
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                        //while(ch!='\n') ch=getc(f_outRasgo);
                        
                        //write_at = ftell(f_outRasgo);
                        //fseek(f_outRasgo, read_at, SEEK_SET);
                        
                        //Escribe el peso Global del Rasgo
                        //peso = statGlobal_weighting(frecuencia, apariciones, num_doc);
                        //fprintf(f_outPeso, "%.8f", peso);
                        //fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        //contRasgo=1;
                        
                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posR);
                        
    //write_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, read_at, SEEK_SET);
    
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
    
    extern void inversoGlobal_file_OK(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        
        int write_at=0; int read_at=0;
        
        for (int k = 0; k < INC; k++) b[k] = '\0';
        for (int k = 0; k < INC; k++) b1[k] = '\0';
        for (int j = 0; j < INC; j++) b0[j] = '\0';
        for (int j = 0; j < INC; j++) b01[j] = '\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*********************************  
        //Lee Token caracter a caracter
        //Ficheros origen Voc y Rep locales
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {//Rasgo a buscar
                        b[i] = ch;
                        b[i+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {//Frecuencia del Rasgo
                        b1[j] = ch1;
                        b1[j+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                // Rasgo y Frecuencia del vocabulario
                //obtenido del fichero num_doc
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */

                // ********************************************
                //Busca Rasgo en F inverso 
                //Ficheros globales finverso.voc y finverso.rep
                //
                {                 
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_outRasgo);
                        ch1 = getc(f_outPeso);
                        
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
    
                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;
    
                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '-' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch == '-' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                                       
                                //Toma posicion lectura/escritura fichero de rasgos
                                //fgetpos (pFile,&pos);
                                //fgetpos(f_outRasgo,posR);
                                //fgetpos(f_outRasgo,posW);
                                //read_at = ftell(f_outRasgo);
                                
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_outRasgo);
                                                                                   
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
                                
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_outPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                                                               
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                                                
    //read_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, write_at, SEEK_SET);
    
        
                    }
                }
                
                //*********************************************
                //Tratamiento de linea de Rasgo correspondiente
                //Sobre el fichero local invertido Finvertido.voc
                //Adjunta Num_Documento y Frecuencia a Rasgo =>
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    int dosPuntos=0;
                    
                    ch=ch1=0;
                    j=0;            
                    
                    //Inicializa contador al Rasgo actual
                    //contRasgo=1;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Problema con el redondeo !!!!!!!!!!!!!!!!
                        //a partir del 8 decimal..........
                        sscanf(b1, "%f", &frecuencia);
                        //sscanf(apariciones, "%d", (char *)&b01);
                        for(int k=j;k<10;k++) buffer[k]=b1[k];
                        frecuencia=strtof(buffer,NULL);
                        frecuencia=strtof(b1,NULL);
                        frecuencia=atof(b1);                    
                        apariciones=atoll((char *)b01);
                        frecuencia=atoff(b1);
                        //apariciones=atoi((const char*)b01);

                        //Coloca el puntero de escritura del Fichero
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posW);
                        //fseek(f_outRasgo, write_at, SEEK_SET);                   
                        
while ((ch = fgetc(f_outRasgo)) != '\n') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    // ASCII 97-122
    if (('a' <= ch) && (ch <= 'z')) {
      fputc(ch - 32, f_outRasgo);
    } else {// ASCII 58
      if ((':' == ch) && (dosPuntos < 4)) {//Max numero de documentos
          if(dosPuntos<3) {//Numero de documentos
              dosPuntos++;
          }
          else {
              dosPuntos=1;
          }
          fputc(ch, f_outRasgo);    
      } else {// ASCII 48-57
        if (('0' <= ch) && (ch <= '9') && (dosPuntos > 0)) {//Documento(num_doc)
            if(dosPuntos == num_doc) {// Documento a trabajar (int num_doc)
                fputc('1', f_outRasgo);//Introduce la frecuencia
            }
            else {
                fputc(ch, f_outRasgo);
            }
        } else {
            fputc(ch, f_outRasgo);
        }
      }
    }
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
if(ch='\n') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    fputc(ch, f_outRasgo);
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
                        
                        //Busca el sitio de escritura en el fichero inverso
                        //for (int j = 0; j < num_doc ; j++) while(ch!=':') ch=getc(f_outRasgo);
                        //Escribe el Rasgo
                        //for (int j = 0; j < strlen(b); j++) fprintf(f_outRasgo, "%c", b[j]);
                        //if(num_doc==1) fprintf(f_outRasgo, "%s", " -> ");
                        //fprintf(f_outRasgo, "%s", "\tdoc");
                        //fprintf(f_outRasgo, "%d", num_doc);
                        //fprintf(f_outRasgo, "%c", ':');
                        //for (int j = 0; j < 10 ; j++) fprintf(f_outRasgo, "%c", b1[j]);
                        //fprintf(f_outRasgo, "%8f", frecuencia);
                        //fprintf(f_outRasgo, "%c", '\t');
                        //fprintf(f_outRasgo, "%c", '\n');
                                                
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                        //while(ch!='\n') ch=getc(f_outRasgo);
                        
                        //write_at = ftell(f_outRasgo);
                        //fseek(f_outRasgo, read_at, SEEK_SET);
                        
                        //Escribe el peso Global del Rasgo
                        //peso = statGlobal_weighting(frecuencia, apariciones, num_doc);
                        //fprintf(f_outPeso, "%.8f", peso);
                        //fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        //contRasgo=1;
                        
                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posR);
                        
    //write_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, read_at, SEEK_SET);
    
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
    
    extern void inversoGlobal_fileOK_1_11111111(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        
        int write_at=0; int read_at=0;
        
        for (int k = 0; k < INC; k++) b[k] = '\0';
        for (int k = 0; k < INC; k++) b1[k] = '\0';
        for (int j = 0; j < INC; j++) b0[j] = '\0';
        for (int j = 0; j < INC; j++) b01[j] = '\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*********************************  
        //Lee Token caracter a caracter
        //Ficheros origen Voc y Rep locales
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {//Rasgo a buscar
                        b[i] = ch;
                        b[i+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {//Frecuencia del Rasgo
                        b1[j] = ch1;
                        b1[j+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                // Rasgo y Frecuencia del vocabulario
                //obtenido del fichero num_doc
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */

                // ********************************************
                //Busca Rasgo en F inverso 
                //Ficheros globales finverso.voc y finverso.rep
                //
                {                 
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_outRasgo);
                        ch1 = getc(f_outPeso);
                        
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
    
                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;
    
                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '-' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch == '-' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                                       
                                //Toma posicion lectura/escritura fichero de rasgos
                                //fgetpos (pFile,&pos);
                                //fgetpos(f_outRasgo,posR);
                                //fgetpos(f_outRasgo,posW);
                                //read_at = ftell(f_outRasgo);
                                
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_outRasgo);
                                                                                   
//Control de lectura/escritura
//INICIO                            
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    
            fputc(ch, f_outRasgo);
            
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
//FIN
//Control de lectura/escritura
                                
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_outPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                                                               
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                                                
    //read_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, write_at, SEEK_SET);
    
        
                    }
                }
                
                //*********************************************
                //Tratamiento de linea de Rasgo correspondiente
                //Sobre el fichero local invertido Finvertido.voc
                //Adjunta Num_Documento y Frecuencia a Rasgo =>
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    int dosPuntos=0;
                    
                    ch=ch1=0;
                    j=0;            
                    
                    //Inicializa contador al Rasgo actual
                    //contRasgo=1;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Problema con el redondeo !!!!!!!!!!!!!!!!
                        //a partir del 8 decimal..........
                        sscanf(b1, "%f", &frecuencia);
                        //sscanf(apariciones, "%d", (char *)&b01);
                        for(int k=j;k<10;k++) buffer[k]=b1[k];
                        frecuencia=strtof(buffer,NULL);
                        frecuencia=strtof(b1,NULL);
                        frecuencia=atof(b1);                    
                        apariciones=atoll((char *)b01);
                        frecuencia=atoff(b1);
                        //apariciones=atoi((const char*)b01);

                        //Coloca el puntero de escritura del Fichero
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posW);
                        //fseek(f_outRasgo, write_at, SEEK_SET);                   
                        
while ((ch = fgetc(f_outRasgo)) != '\n') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    // ASCII 97-122
    if (('a' <= ch) && (ch <= 'z')) {
      fputc(ch - 32, f_outRasgo);
    } else {// ASCII 58
      if ((':' == ch) && (dosPuntos < 4)) {//Max numero de documentos
          if(dosPuntos<3) {
              dosPuntos++;
          }
          else {
              dosPuntos=1;
          }
          fputc(ch, f_outRasgo);    
      } else {// ASCII 48-57
        if (('0' <= ch) && (ch <= '9') && (dosPuntos > 0)) {//Documento(num_doc)
            if(dosPuntos == 2) {// Documento a trabajar (int num_doc)
                fputc('1', f_outRasgo);//Introduce la frecuencia
            }
            else {
                fputc(ch, f_outRasgo);
            }
        } else {
            fputc(ch, f_outRasgo);
        }
      }
    }
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
if(ch='\n') {
    read_at = ftell(f_outRasgo);
    fseek(f_outRasgo, write_at, SEEK_SET);
    fputc(ch, f_outRasgo);
    write_at = ftell(f_outRasgo);
    fseek(f_outRasgo, read_at, SEEK_SET);
}
                        
                        //Busca el sitio de escritura en el fichero inverso
                        //for (int j = 0; j < num_doc ; j++) while(ch!=':') ch=getc(f_outRasgo);
                        //Escribe el Rasgo
                        //for (int j = 0; j < strlen(b); j++) fprintf(f_outRasgo, "%c", b[j]);
                        //if(num_doc==1) fprintf(f_outRasgo, "%s", " -> ");
                        //fprintf(f_outRasgo, "%s", "\tdoc");
                        //fprintf(f_outRasgo, "%d", num_doc);
                        //fprintf(f_outRasgo, "%c", ':');
                        //for (int j = 0; j < 10 ; j++) fprintf(f_outRasgo, "%c", b1[j]);
                        //fprintf(f_outRasgo, "%8f", frecuencia);
                        //fprintf(f_outRasgo, "%c", '\t');
                        //fprintf(f_outRasgo, "%c", '\n');
                                                
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                        //while(ch!='\n') ch=getc(f_outRasgo);
                        
                        //write_at = ftell(f_outRasgo);
                        //fseek(f_outRasgo, read_at, SEEK_SET);
                        
                        //Escribe el peso Global del Rasgo
                        //peso = statGlobal_weighting(frecuencia, apariciones, num_doc);
                        //fprintf(f_outPeso, "%.8f", peso);
                        //fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        //contRasgo=1;
                        
                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posR);
                        
    //write_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, read_at, SEEK_SET);
    
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
    
    extern void inversoGlobal_file1(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        int write_at=0, read_at=0;
        
        for (int k = 0; k < INC; k++) b[k] = '\0';
        for (int k = 0; k < INC; k++) b1[k] = '\0';
        for (int j = 0; j < INC; j++) b0[j] = '\0';
        for (int j = 0; j < INC; j++) b01[j] = '\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*********************************  
        //Lee Token caracter a caracter
        //Ficheros origen Voc y Rep locales
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {
                        b[i] = ch;
                        b[i+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {
                        b1[j] = ch1;
                        b1[j+1] = '\0';//Arregla un problema de corrupcion de datos
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */

                // *************************
                //Examina Rasgo y Apariciones
                //Ficheros globales Ras y Dtf
                //
                {                 
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_outRasgo);
                        ch1 = getc(f_outPeso);

                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;

                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '-' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch == '-' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                                       
                                //Toma posicion lectura/escritura fichero de rasgos
                                //fgetpos (pFile,&pos);
                                //fgetpos(f_outRasgo,posR);
                                //fgetpos(f_outRasgo,posW);
                                //read_at = ftell(f_outRasgo);
                                
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_outRasgo);
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_outPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                                                               
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                                                
    //read_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, write_at, SEEK_SET);
    
        
                    }
                }                
                
                
                //*********************************************
                //Fichero local invertido Fin
                //Adjunta Num_Documento y Frecuencia a Rasgo =>
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    j=0;                        
                    //Inicializa contador al Rasgo actual
                    //contRasgo=1;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Problema con el redondeo !!!!!!!!!!!!!!!!
                        //a partir del 8 decimal..........
                        sscanf(b1, "%f", &frecuencia);
                        //sscanf(apariciones, "%d", (char *)&b01);
                        for(int k=j;k<10;k++) buffer[k]=b1[k];
                        frecuencia=strtof(buffer,NULL);
                        frecuencia=strtof(b1,NULL);
                        frecuencia=atof(b1);                    
                        apariciones=atoll((char *)b01);
                        frecuencia=atoff(b1);
                        //apariciones=atoi((const char*)b01);

                        //Coloca el puntero de escritura del Fichero
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posW);
                        //fseek(f_outRasgo, write_at, SEEK_SET);

                        //Busca el sitio de escritura en el fichero inverso
                        for (int j = 0; j < num_doc ; j++) while(ch!=':') ch=getc(f_outRasgo);
                        //Escribe el Rasgo
                        //for (int j = 0; j < strlen(b); j++) fprintf(f_outRasgo, "%c", b[j]);
                        //if(num_doc==1) fprintf(f_outRasgo, "%s", " -> ");
                        //fprintf(f_outRasgo, "%s", "\tdoc");
                        //fprintf(f_outRasgo, "%d", num_doc);
                        //fprintf(f_outRasgo, "%c", ':');
                        for (int j = 0; j < 10 ; j++) fprintf(f_outRasgo, "%c", b1[j]);
                        //fprintf(f_outRasgo, "%8f", frecuencia);
                        //fprintf(f_outRasgo, "%c", '\t');
                        //fprintf(f_outRasgo, "%c", '\n');
                                                
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        //fgetpos(f_outRasgo,posR);
                        while(ch!='\n') ch=getc(f_outRasgo);
                        
                        //write_at = ftell(f_outRasgo);
                        //fseek(f_outRasgo, read_at, SEEK_SET);
                        
                        //Escribe el peso Global del Rasgo
                        //peso = statGlobal_weighting(frecuencia, apariciones, num_doc);
                        //fprintf(f_outPeso, "%.8f", peso);
                        //fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        //contRasgo=1;
                        
                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        //fsetpos (f_outRasgo,posR);
                        
    //write_at = ftell(f_outRasgo);
    //fseek(f_outRasgo, read_at, SEEK_SET);
    
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
        
    extern void inversoGlobal_file2(struct SN_env * z, FILE * f_inVoc, FILE * f_inRep, FILE * f_outRasgo, FILE * f_outPeso, int num_doc)
    {
        #define INC 10
        int lim = INC;
        
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));
        symbol * b1 = (symbol *) malloc(lim * sizeof(symbol));
        
        symbol * b0= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
        symbol * b01= (symbol *) malloc(lim * sizeof(symbol));//Memoria del token anterior
                
        struct SN_env * z0 = create_env();
        int contRasgo=0;
        float peso=0.0;
        
        int fin=0, fin1=0;
        int ch = 0;
        int ch1 = 0;
                
        int fin2=0, fin3=0;

        int i = 0;
        int j = 0;        
        
        fpos_t * posR= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        fpos_t * posW= (fpos_t *)malloc(1 * sizeof(fpos_t));//Indice de lectura/escritura del fichero inverso
        
        for (int j = 0; j < lim; j++) b0[j]='\0';
        for (int j = 0; j < lim; j++) b01[j]='\0';
        
        //Ordenar fichero
        //mezclar(f_in);

        //*********************************  
        //Lee Token caracter a caracter
        //Ficheros origen Voc y Rep locales
        while(1) {
            //Testigos
            fin=0; fin1=0;
            
            for (int k = 0; k < lim; k++) b[k] = '\0';
            for (int k = 0; k < INC; k++) b1[k] = '\0';
            
            lim=INC;

            ch = getc(f_inVoc);
            ch1 = getc(f_inRep);
            
            //Comprueba fin de fichero
            if ( (ch == EOF) && (ch1 == EOF)) {

                fin=-1; fin1=-1;

                return;
            }
            else
                if (ch == EOF) {
                    
                    fin=-1; 
                }
                else
                    if(ch1==EOF){
                        
                        fin1=-1;
                    }
            {
                i = 0;
                j = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin=-1; if(fin1==-1) break; }
                    if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin1=-1; if(fin==-1) break; }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    if(fin==0) {
                        b[i] = ch; 
                        ch = getc(f_inVoc);
                        i++;
                    }
                    if(fin1==0) {
                        b1[j] = ch1; 
                        ch1 = getc(f_inRep);
                        j++;
                    }
                }
                
                // ***********************************
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int k=i;k<lim;k++) b[k]='\0';
                for(int k=j;k<INC;k++) b1[k]='\0';
                /* FIN Tratamiento de cada token */

                // *************************
                //Examina Rasgo y Apariciones
                //Ficheros globales Ras y Dtf
                //
                {                 
                    ch = 0;
                    ch1 = 0;
                    lim = INC;
                    
                    //Lee Token caracter a caracter
                    //Ficheros globales
                    //Hasta que encuentar el Rasgo b en Rasgos globales b0
                    while(statGlobal_matching((char *)b, (char *)b0)!=0) {

                        for (int k = 0; k < lim; k++) b0[k] = '\0';
                        for (int k = 0; k < INC; k++) b01[k] = '\0';
                        
                        //Testigos
                        fin2=0; fin3=0;
                        ch = getc(f_outRasgo);
                        ch1 = getc(f_outPeso);

                        //Comprueba fin de fichero
                        if ( (ch == EOF) && (ch1 == EOF)) {

                            fin2=-1; fin3=-1;

                            //free(b0);
                            //free(b01);
                            //free(b1);
                            //free(b);
                            return;
                        }
                        else
                            if (ch == EOF) {

                                fin2=-1; 
                            }
                            else
                                if(ch1==EOF){
                                    
                                    fin3=-1;
                                }

                        i = 0;
                        j = 0;
                        //Lee Token
                        while(1) {
                            //Termina Token
                            if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) { fin2=-1; if(fin3==-1) break; }
                            if (ch1 == '\n' || ch1 == '\t' || ch1 == ' ' || ch1 == EOF) { fin3=-1; if(fin2==-1) break; }
                            if (i == lim) {  /* make b bigger */
                                symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                                memmove(q, b0, lim * sizeof(symbol));
                                free(b0); b0 = q;
                                for (int j = lim; j < lim + INC; j++) b0[j]='\0';
                                lim = lim + INC;
                            }                            
                            if(fin2==0) {                                       
                                //Toma posicion lectura/escritura fichero de rasgos
                                //fgetpos (pFile,&pos);
                                //fgetpos(f_outRasgo,posR);
                                fgetpos(f_outRasgo,posW);
                            
                                b0[i] = ch;
                                b0[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch = getc(f_outRasgo);
                                i++;
                            }
                            if(fin3==0) {
                                b01[j] = ch1;
                                b01[i+1] = '\0';//Arregla un problema de corrupcion de datos
                                ch1 = getc(f_outPeso);
                                j++;
                            }
                        }                        
                        for(int k=i;k<lim;k++) b0[k]='\0';
                        for(int k=j;k<INC;k++) b01[k]='\0';
                                                               
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        fgetpos(f_outRasgo,posR);
                                
                    }
                }                
                
                
                //*********************************************
                //Fichero local invertido Fin
                //Adjunta Num_Documento y Frecuencia a Rasgo =>
                //
                {
                    float frecuencia=0;
                    int apariciones=0;
                    char buffer[10];
                    j=0;                        
                    //Inicializa contador al Rasgo actual
                    contRasgo=1;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(statGlobal_matching((char *)b, (char *)b0)==0)){
                        
                        //Problema con el redondeo !!!!!!!!!!!!!!!!
                        //a partir del 8 decimal..........
                        sscanf(b1, "%f", &frecuencia);
                        //sscanf(apariciones, "%d", (char *)&b01);
                        for(int k=j;k<10;k++) buffer[k]=b1[k];
                        frecuencia=strtof(buffer,NULL);
                        frecuencia=strtof(b1,NULL);
                        frecuencia=atof(b1);                    
                        apariciones=atoll((char *)b01);
                        frecuencia=atoff(b1);
                        //apariciones=atoi((const char*)b01);

                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        fsetpos (f_outRasgo,posW);
                        
                        //Escribe el Rasgo
                        //for (int j = 0; j < strlen(b); j++) fprintf(f_outRasgo, "%c", b[j]);
                        if(num_doc==1) fprintf(f_outRasgo, "%s", " -> ");
                            
                        fprintf(f_outRasgo, "%s", "\tdoc");
                        fprintf(f_outRasgo, "%d", num_doc);
                        fprintf(f_outRasgo, "%c", ':');
                        for (int j = 0; j < strlen(b1); j++) fprintf(f_outRasgo, "%c", b1[j]);
                        //fprintf(f_outRasgo, "%c", '\t');
                        fprintf(f_outRasgo, "%c", '\n');
                                                
                        //Toma posicion lectura/escritura fichero de rasgos
                        //fgetpos (pFile,&pos);
                        //ftell (pFile)
                        fgetpos(f_outRasgo,posR);
                        
                        //Escribe el peso Global del Rasgo
                        //peso = statGlobal_weighting(frecuencia, apariciones, num_doc);
                        //fprintf(f_outPeso, "%.8f", peso);
                        //fprintf(f_outPeso, "\n");
                        
                        //Inicializa contador al Rasgo actual
                        contRasgo=1;
                        
                        //Coloca el puntero de escritura
                        //fseek ( f_outRasgo , 1 , SEEK_CUR );
                        //fsetpos (pFile,&pos);
                        fsetpos (f_outRasgo,posR);
                        
                    }
                    
                    /*for (int k = 0; k < strlen((char*)b); k++) b[k] = '\0';
                    for (int k = 0; k < strlen((char*)b1); k++) b1[k] = '\0';
                    for (int k = 0; k < strlen((char*)b0); k++) b0[k] = '\0';
                    for (int k = 0; k < strlen((char*)b01); k++) b01[k] = '\0';*/
                }
            }
        }
        
        free(b);
        free(b1);
        free(b0);
        free(b01);
        close_env(z0);
    }
        
    extern int stattingGlobalRasgos(char * in, char * outRasgo, char * outPeso, int total)
    {   
        //char * in = 0;
        //char * out = 0;

        /* initialise the stemming process: */

        {
            struct SN_env * z = create_env();
            FILE * f_in;
            FILE * f_outRasgo;
            FILE * f_outPeso;
            
            f_in = in == 0 ? stdin : fopen(in, "r");
            if (f_in == 0) {
                fprintf(stderr, "file %s not found\n", in); exit(1);
            }
            f_outRasgo = outRasgo == 0 ? stdout : fopen(outRasgo, "w+");
            if (f_outRasgo == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outRasgo); exit(1);
            }
            
            f_outPeso = outPeso == 0 ? stdout : fopen(outPeso, "w+");
            if (f_outPeso == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outPeso); exit(1);
            }
            
            rasgosGlobal_file(z, f_in, f_outRasgo, f_outPeso, total);
            
            close_env(z);
            fclose(f_in);
            fclose(f_outRasgo);
            fclose(f_outPeso);
        }

        return 0;
    }
        
    extern int stattingGlobal(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total)
    {   //char * in = 0;
        //char * out = 0;

        /* initialise the stemming process: */

        {
            struct SN_env * z = create_env();
            FILE * f_inVoc;
            FILE * f_inRep;
            FILE * f_inGlobalRasgo;
            FILE * f_inGlobalPeso;
            FILE * f_outRasgo;
            FILE * f_outPeso;
            
            f_inVoc = inVoc == 0 ? stdin : fopen(inVoc, "r");
            if (f_inVoc == 0) {
                fprintf(stderr, "file %s not found\n", inVoc); exit(1);
            }            
            f_inRep = inRep == 0 ? stdin : fopen(inRep, "r");
            if (f_inRep == 0) {
                fprintf(stderr, "file %s not found\n", inRep); exit(1);
            }   
            
            f_inGlobalRasgo = inGlobalRasgo == 0 ? stdin : fopen(inGlobalRasgo, "r+");
            if (f_inGlobalRasgo == 0) {
                fprintf(stderr, "file %s not found\n", inGlobalRasgo); exit(1);
            }
            rewind(f_inGlobalRasgo);
            
            f_inGlobalPeso = inGlobalPeso == 0 ? stdin : fopen(inGlobalPeso, "r+");
            if (f_inGlobalPeso == 0) {
                fprintf(stderr, "file %s not found\n", inGlobalPeso); exit(1);
            }
            rewind(f_inGlobalPeso);
            
            f_outRasgo = outRasgo == 0 ? stdout : fopen(outRasgo, "w+");
            if (f_outRasgo == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outRasgo); exit(1);
            }
            
            f_outPeso = outPeso == 0 ? stdout : fopen(outPeso, "w+");
            if (f_outPeso == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outPeso); exit(1);
            }
            
            statGlobal_file(z, f_inVoc, f_inRep, f_inGlobalRasgo, f_inGlobalPeso, f_outRasgo, f_outPeso, total);
            
            close_env(z);
            fclose(f_inVoc);
            fclose(f_inRep);
            fclose(f_inGlobalRasgo);
            fclose(f_inGlobalPeso);
            fclose(f_outRasgo);
            fclose(f_outPeso);
        }

        return 0;
    }

    extern int stattingGlobalInversoCrear(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total)
    {   //char * in = 0;
        //char * out = 0;

        /* initialise the stemming process: */

        {
            struct SN_env * z = create_env();
            FILE * f_inVoc;
            FILE * f_inRep;
            FILE * f_inGlobalRasgo;
            FILE * f_inGlobalPeso;
            FILE * f_outRasgo;
            FILE * f_outPeso;
            
            f_inVoc = inVoc == 0 ? stdin : fopen(inVoc, "r");
            if (f_inVoc == 0) {
                fprintf(stderr, "file %s not found\n", inVoc); exit(1);
            }            
            f_inRep = inRep == 0 ? stdin : fopen(inRep, "r");
            if (f_inRep == 0) {
                fprintf(stderr, "file %s not found\n", inRep); exit(1);
            }   
            
            f_inGlobalRasgo = inGlobalRasgo == 0 ? stdin : fopen(inGlobalRasgo, "r+");
            if (f_inGlobalRasgo == 0) {
                fprintf(stderr, "file %s not found\n", inGlobalRasgo); exit(1);
            }
            rewind(f_inGlobalRasgo);
            
            f_inGlobalPeso = inGlobalPeso == 0 ? stdin : fopen(inGlobalPeso, "r+");
            if (f_inGlobalPeso == 0) {
                fprintf(stderr, "file %s not found\n", inGlobalPeso); exit(1);
            }
            rewind(f_inGlobalPeso);
            
            f_outRasgo = outRasgo == 0 ? stdout : fopen(outRasgo, "w+");
            if (f_outRasgo == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outRasgo); exit(1);
            }
            
            f_outPeso = outPeso == 0 ? stdout : fopen(outPeso, "w+");
            if (f_outPeso == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outPeso); exit(1);
            }

            inversoGlobal_fileCrear(f_inGlobalRasgo, f_inGlobalPeso, f_outRasgo, f_outPeso, 10);
            
            close_env(z);
            fclose(f_inVoc);
            fclose(f_inRep);
            fclose(f_inGlobalRasgo);
            fclose(f_inGlobalPeso);
            fclose(f_outRasgo);
            fclose(f_outPeso);
        }

        return 0;
    }
    
        extern int stattingGlobalInverso(char * inVoc, char * inRep, char * inGlobalRasgo, char * inGlobalPeso, char * outRasgo, char * outPeso, int total)
    {   //char * in = 0;
        //char * out = 0;

        /* initialise the stemming process: */

        {
            struct SN_env * z = create_env();
            FILE * f_inVoc;
            FILE * f_inRep;
            FILE * f_inGlobalRasgo;
            FILE * f_inGlobalPeso;
            FILE * f_outRasgo;
            FILE * f_outPeso;
            
            if(total==1) {
                f_inGlobalRasgo = inGlobalRasgo == 0 ? stdin : fopen(inGlobalRasgo, "r+");
                if (f_inGlobalRasgo == 0) {
                    fprintf(stderr, "file %s not found\n", inGlobalRasgo); exit(1);
                }
                rewind(f_inGlobalRasgo);

                f_inGlobalPeso = inGlobalPeso == 0 ? stdin : fopen(inGlobalPeso, "r+");
                if (f_inGlobalPeso == 0) {
                    fprintf(stderr, "file %s not found\n", inGlobalPeso); exit(1);
                }
                rewind(f_inGlobalPeso);

                f_outRasgo = outRasgo == 0 ? stdout : fopen(outRasgo, "w+");
                if (f_outRasgo == 0) {
                    fprintf(stderr, "file %s cannot be opened\n", outRasgo); exit(1);
                }

                f_outPeso = outPeso == 0 ? stdout : fopen(outPeso, "w+");
                if (f_outPeso == 0) {
                    fprintf(stderr, "file %s cannot be opened\n", outPeso); exit(1);
                }

                inversoGlobal_fileCrear(f_inGlobalRasgo, f_inGlobalPeso, f_outRasgo, f_outPeso, 10);//10);// 5); //Para 3 Documentos -> 3);
            
                fclose(f_inGlobalRasgo);
                fclose(f_inGlobalPeso);
                fclose(f_outRasgo);
                fclose(f_outPeso);
            }
            
            f_inVoc = inVoc == 0 ? stdin : fopen(inVoc, "r");
            if (f_inVoc == 0) {
                fprintf(stderr, "file %s not found\n", inVoc); exit(1);
            }            
            f_inRep = inRep == 0 ? stdin : fopen(inRep, "r");
            if (f_inRep == 0) {
                fprintf(stderr, "file %s not found\n", inRep); exit(1);
            }
            
            f_outRasgo = outRasgo == 0 ? stdout : fopen(outRasgo, "r+");
            if (f_outRasgo == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outRasgo); exit(1);
            }
            
            f_outPeso = outPeso == 0 ? stdout : fopen(outPeso, "r+");
            if (f_outPeso == 0) {
                fprintf(stderr, "file %s cannot be opened\n", outPeso); exit(1);
            }
            
            inversoGlobal_file(z, f_inVoc, f_inRep, f_outRasgo, f_outPeso, total);
                        
            //fflush(f_outRasgo);
            
            close_env(z);
            fclose(f_inVoc);
            fclose(f_inRep);
            fclose(f_outRasgo);
            fclose(f_outPeso);
        }

        return 0;
    }