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

    extern void stem_file(struct SN_env * z, FILE * f_in, FILE * f_out) {
    #define INC 10
        int lim = INC;
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));

        //for (int j = 0; j < lim; j++) b[j]='\0';
        
        while(1) {
            int ch = getc(f_in);
            if (ch == EOF) {
                free(b); return;
            }
            {
                int i = 0, saltaCaracter=0;
                while(1) {
                    //Fin de linea o de fichero
                    if (ch == '\n' || ch == EOF) break;
                   //Pasa cada caracter tonico (195+) a atono
                   switch(ch){
                       case 195:
                           saltaCaracter=666;
                           break;
                       case 161:
                           ch=97;
                           break;
                       case 169:
                           ch=101;
                           break;
                       case 173:
                           ch=105;
                           break;
                       case 179:
                           ch=111;
                           break;
                       case 186:
                           ch=117;
                           break;
                       case 177:
                           if(saltaCaracter==0)
                           {                                                         
                            ch=195;
                            saltaCaracter=1;
                           }
                           else{                          
                            ch=177;
                            saltaCaracter=0;
                           }
                           break;
                       default:
                           break;
                   }
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        //for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                   if(saltaCaracter==0){
                    b[i] = ch; i++;
                    //b[i] = '\0';//Arregla un problema de corrupcion de datos
                    ch = getc(f_in);
                   }
                   else
                    if(saltaCaracter==1){
                        b[i] = ch; i++;
                        //b[i] = '\0';//Arregla un problema de corrupcion de datos
                        ch=177;
                        saltaCaracter=2;
                    }
                    else{
                        ch = getc(f_in);
                        saltaCaracter=0;
                   }
                }

                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                stem(z);
                /* FIN Tratamiento de cada token */
                
                {
                    int j;
                    for (j = 0; j < z->l; j++) fprintf(f_out, "%c", z->p[j]);
                    fprintf(f_out, "\n");
                }
            }
            //free(b);
        }
    }

    extern int stemming(char * in, char * out)
    {   //char * in = 0;
        //char * out = 0;

        /* initialise the stemming process: */

        {
            struct SN_env * z = create_env();
            FILE * f_in;
            FILE * f_out;
            f_in = in == 0 ? stdin : fopen(in, "r");
            if (f_in == 0) {
                fprintf(stderr, "file %s not found\n", in); exit(1);
            }
            f_out = out == 0 ? stdout : fopen(out, "w");
            if (f_out == 0) {
                fprintf(stderr, "file %s cannot be opened\n", out); exit(1);
            }
            stem_file(z, f_in, f_out);
            close_env(z);
            fclose(f_in);
            fclose(f_out);
        }

        return 0;
    }
