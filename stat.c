  #include <stdio.h>
    #include <stdlib.h> /* for malloc, free */
    #include <string.h> /* for memmove */

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

    /* Función local de pesado TF */
    /* Devuelve la frecuencia del Rasgo contado en un total de documentos */
    extern float stat_weighting(int contador, int total)
    {         
        /* Initialise the process: */
        if((contador>0)&&(total>0))
            return (float)((float)contador/(float)total);
        else
            return 0.0;
        /* Finishise the process: */
    }
        
    extern int stat_matching(char * palabra, char * palabraPrevia)
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

    extern void statLocal_file(struct SN_env * z, FILE * f_in, FILE * f_outRasgo, FILE * f_outPeso, int total) {
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
                peso = stat_weighting(contRasgo, total);
                for (int j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                //fprintf(f_outRasgo, "\n");

                fprintf(f_outPeso, "%.8f", peso);
                //fprintf(f_outPeso, "\n");
                
                free(b0);
                free(b); return;
            }
            {
                int i = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n' || ch == '\t' || ch == '\r' || ch == ' ' || ch == ',' || ch == ';' || ch == '.' || ch == ':' || ch == '-' || ch == '_' || ch == '\\' || ch == '!' || ch == '|' || ch == '"' || ch == '\'' || ch == '(' || ch == ')' ||  ch == '+' ||  ch == '-' || ch == EOF) break;
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    b[i] = ch; i++;
                    b[i] = '\0';//Arregla un problema de corrupcion de datos
                    ch = getc(f_in);
                }

                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                for(int j=i;j<lim;j++) b[j]='\0';
                /* FIN Tratamiento de cada token */
                
                {
                    int j;
                    
                    //Copia Rasgo y Peso en cada fichero salida
                    if((strlen(b0)>0)&&(stat_matching((char *)b, (char *)b0)!=0)){
                        peso = stat_weighting(contRasgo, total);
                        for (j = 0; j < strlen(b0); j++) fprintf(f_outRasgo, "%c", b0[j]);
                        fprintf(f_outRasgo, "\n");
                        
                        fprintf(f_outPeso, "%.8f", peso);
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

    extern int statting(char * in, char * outRasgo, char * outPeso, int total)
    {   //char * in = 0;
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
            
            statLocal_file(z, f_in, f_outRasgo, f_outPeso, total);
            
            close_env(z);
            fclose(f_in);
            fclose(f_outRasgo);
            fclose(f_outPeso);
        }

        return 0;
    }

