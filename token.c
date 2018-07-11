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

    extern void token_file(struct SN_env * z, FILE * f_in, FILE * f_out) {
    #define INC 10
        int lim = INC;
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));

        //for (int j = 0; j < lim; j++) b[j]='\0';
        
        //Lee Token caracter a caracter
        while(1) {
            int ch = getc(f_in);
            if (ch == EOF) {
                free(b); return;
            }
            {
                int i = 0, saltaCaracter=0;
                while(1) {
                    //Caracter fin de Token
                    if (ch == '\n' || ch == '\t' || ch == '\r' || ch == ' ' || ch == ',' || ch == ';' || ch == '.' || ch == ':' || ch == '-' || ch == '_' || ch == '/' || ch == '\\' || ch == '!' || ch == '?' || ch == '|' || ch == '"' || ch == '\'' || ch == '(' || ch == ')' || ch == '%' || ch == '&' || ch == '$' || ch == '+' || ch == '-' || ch == '=' || ch == EOF || (('0' <= ch) && (ch <= '9')) ) break;
                    //Pasa caracter mayusculo con tilde (191+) a minusculas con tilde
                    switch(ch){
                        case 145:
                            ch=177;
                            break;
                        case 129:
                            ch=161;
                            break;
                        case 137:
                            ch=169;
                            break;
                        case 141:
                            ch=173;
                            break;
                        case 147:
                            ch=179;
                            break;
                        case 154:
                            ch=186;
                            break;
                        case 194:
                            saltaCaracter=1;
                            break;
                        case 195:
                            saltaCaracter=3;
                            break;
                        case 191:
                            saltaCaracter=2;
                            break;
                        default:
                            ch=((ch>0x40)&&(ch<0x5b))?(ch|0x60):(ch);
                            break;
                    }
                    //Reserva mas memoria si se supera el limite
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        //for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                   if(saltaCaracter==0){//Normal
                        b[i] = ch; i++;
                        //b[i] = '\0';//Arregla un problema de corrupcion de datos
                        ch = getc(f_in);
                   }
                   else{
                        if(saltaCaracter==1){//Salta apertura de interrogante o exclamacion 
                         ch = getc(f_in);
                         ch = 191;
                         saltaCaracter=0;
                        }
                        else{
                            if(saltaCaracter==2){//Salta vocal mayuscula con tilde
                                ch = getc(f_in);
                                saltaCaracter=0;
                            }
                            else{//Salta vocal Uu con dieresis
                                ch = getc(f_in);
                                if( (ch != 156) && (ch != 188) ){
                                    b[i] = 195; i++;
                                    //b[i] = '\0';//Arregla un problema de corrupcion de datos                             
                                }
                                else{
                                    ch=117;
                                }
                                saltaCaracter=0;
                            }
                        }
                   }
                }

                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);
                /* FIN Tratamiento de cada token */
                
                {
                    //Copia Token en fichero salida
                    //eliminando los caracteres de puntuacion
                    if(z->l>0){
                        int j;
                        for (j = 0; j < z->l; j++) fprintf(f_out, "%c", z->p[j]);
                        fprintf(f_out, "\n");
                    }
                }
                //free(b);
            }
        }
    }

    extern int tokening(char * in, char * out)
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
            token_file(z, f_in, f_out);
            close_env(z);
            fclose(f_in);
            fclose(f_out);
        }

        return 0;
    }

