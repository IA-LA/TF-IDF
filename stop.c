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

    
    extern int stop_words(char * palabra)
    {   
        int procesado;
        char * stop_word[] = { (char *)"a", (char *)"al", (char *)"algo", (char *)"algunas", (char *)"algunos", (char *)"ante", (char *)"antes", (char *)"como", (char *)"con", (char *)"contra", (char *)"cual", (char *)"cuando", (char *)"de", (char *)"del", (char *)"desde", (char *)"donde", (char *)"durante", (char *)"e", (char *)"el", (char *)"él", (char *)"ella", (char *)"ellas", (char *)"ellos", (char *)"en", (char *)"entre", (char *)"era", (char *)"erais", (char *)"éramos", (char *)"eran", (char *)"eras", (char *)"eres", (char *)"es", (char *)"esa", (char *)"esas", (char *)"ese", (char *)"eso", (char *)"esos", (char *)"esta", (char *)"está", (char *)"estaba", (char *)"estabais", (char *)"estábamos", (char *)"estaban", (char *)"estabas", (char *)"estad", (char *)"estada", (char *)"estadas", (char *)"estado", (char *)"estados", (char *)"estáis", (char *)"estamos", (char *)"están", (char *)"estando", (char *)"estar", (char *)"estará", (char *)"estarán", (char *)"estarás", (char *)"estaré", (char *)"estaréis", (char *)"estaremos", (char *)"estaría", (char *)"estaríais", (char *)"estaríamos", (char *)"estarían", (char *)"estarías", (char *)"estas", (char *)"estás", (char *)"este", (char *)"esté", (char *)"estéis", (char *)"estemos", (char *)"estén", (char *)"estés", (char *)"esto", (char *)"estos", (char *)"estoy", (char *)"estuve", (char *)"estuviera", (char *)"estuvierais", (char *)"estuviéramos", (char *)"estuvieran", (char *)"estuvieras", (char *)"estuvieron", (char *)"estuviese", (char *)"estuvieseis", (char *)"estuviésemos", (char *)"estuviesen", (char *)"estuvieses", (char *)"estuvimos", (char *)"estuviste", (char *)"estuvisteis", (char *)"estuvo", (char *)"fue", (char *)"fuera", (char *)"fuerais", (char *)"fuéramos", (char *)"fueran", (char *)"fueras", (char *)"fueron", (char *)"fuese", (char *)"fueseis", (char *)"fuésemos", (char *)"fuesen", (char *)"fueses", (char *)"fui", (char *)"fuimos", (char *)"fuiste", (char *)"fuisteis", (char *)"ha", (char *)"habéis", (char *)"había", (char *)"habíais", (char *)"habíamos", (char *)"habían", (char *)"habías", (char *)"habida", (char *)"habidas", (char *)"habido", (char *)"habidos", (char *)"habiendo", (char *)"habrá", (char *)"habrán", (char *)"habrás", (char *)"habré", (char *)"habréis", (char *)"habremos", (char *)"habría", (char *)"habríais", (char *)"habríamos", (char *)"habrían", (char *)"habrías", (char *)"han", (char *)"has", (char *)"hasta", (char *)"hay", (char *)"haya", (char *)"hayáis", (char *)"hayamos", (char *)"hayan", (char *)"hayas", (char *)"he", (char *)"hemos", (char *)"hube", (char *)"hubiera", (char *)"hubierais", (char *)"hubiéramos", (char *)"hubieran", (char *)"hubieras", (char *)"hubieron", (char *)"hubiese", (char *)"hubieseis", (char *)"hubiésemos", (char *)"hubiesen", (char *)"hubieses", (char *)"hubimos", (char *)"hubiste", (char *)"hubisteis", (char *)"hubo", (char *)"la", (char *)"las", (char *)"le", (char *)"les", (char *)"lo", (char *)"los", (char *)"más", (char *)"me", (char *)"mi", (char *)"mí", (char *)"mía", (char *)"mías", (char *)"mío", (char *)"míos", (char *)"mis", (char *)"mucho", (char *)"muchos", (char *)"muy", (char *)"nada", (char *)"ni", (char *)"no", (char *)"nos", (char *)"nosotras", (char *)"nosotros", (char *)"nuestra", (char *)"nuestras", (char *)"nuestro", (char *)"nuestros", (char *)"o", (char *)"os", (char *)"otra", (char *)"otras", (char *)"otro", (char *)"otros", (char *)"para", (char *)"pero", (char *)"poco", (char *)"por", (char *)"porque", (char *)"que", (char *)"qué", (char *)"quien", (char *)"quienes", (char *)"se", (char *)"sea", (char *)"seáis", (char *)"seamos", (char *)"sean", (char *)"seas", (char *)"será", (char *)"serán", (char *)"serás", (char *)"seré", (char *)"seréis", (char *)"seremos", (char *)"sería", (char *)"seríais", (char *)"seríamos", (char *)"serían", (char *)"serías", (char *)"sí", (char *)"sido", (char *)"siendo", (char *)"sin", (char *)"sobre", (char *)"sois", (char *)"somos", (char *)"son", (char *)"soy", (char *)"su", (char *)"sus", (char *)"suya", (char *)"suyas", (char *)"suyo", (char *)"suyos", (char *)"también", (char *)"tanto", (char *)"te", (char *)"tendrá", (char *)"tendrán", (char *)"tendrás", (char *)"tendré", (char *)"tendréis", (char *)"tendremos", (char *)"tendría", (char *)"tendríais", (char *)"tendríamos", (char *)"tendrían", (char *)"tendrías", (char *)"tened", (char *)"tenéis", (char *)"tenemos", (char *)"tenga", (char *)"tengáis", (char *)"tengamos", (char *)"tengan", (char *)"tengas", (char *)"tengo", (char *)"tenía", (char *)"teníais", (char *)"teníamos", (char *)"tenían", (char *)"tenías", (char *)"tenida", (char *)"tenidas", (char *)"tenido", (char *)"tenidos", (char *)"teniendo", (char *)"ti", (char *)"tiene", (char *)"tienen", (char *)"tienes", (char *)"todo", (char *)"todos", (char *)"tu", (char *)"tú", (char *)"tus", (char *)"tuve", (char *)"tuviera", (char *)"tuvierais", (char *)"tuviéramos", (char *)"tuvieran", (char *)"tuvieras", (char *)"tuvieron", (char *)"tuviese", (char *)"tuvieseis", (char *)"tuviésemos", (char *)"tuviesen", (char *)"tuvieses", (char *)"tuvimos", (char *)"tuviste", (char *)"tuvisteis", (char *)"tuvo", (char *)"tuya", (char *)"tuyas", (char *)"tuyo", (char *)"tuyos", (char *)"un", (char *)"una", (char *)"uno", (char *)"unos", (char *)"vosotras", (char *)"vosotros", (char *)"vuestra", (char *)"vuestras", (char *)"vuestro", (char *)"vuestros", (char *)"y", (char *)"ya", (char *)"yo" };
        
        //printf("Plabara stop: %s", palabra);
        
        for (procesado = 0; procesado < 308; procesado++) {
            if(strcmp(palabra, stop_word[procesado])==0){
                procesado = 666;
                break;
            }
        }
        
        /* Initialise the process: */
        switch (procesado) {
            case 666:
                return 1;
                break;
            default:
                return 0;
                break;
        }
        /* Finishise the process: */
        return 1;
    }

    extern void stop_file(struct SN_env * z, FILE * f_in, FILE * f_out) {
    #define INC 10
        int lim = INC;
        symbol * b = (symbol *) malloc(lim * sizeof(symbol));

        //for (int j = 0; j < lim; j++) b[j]=' ';
        
        //Lee Token caracter a caracter
        while(1) {
            int ch = getc(f_in);
            if (ch == EOF) {
                free(b); return;
            }
            {
                int i = 0;
                while(1) {
                    //Termina Token
                    if (ch == '\n'  || ch == '\t' || ch == '\r' || ch == ' ' || ch == ',' || ch == ';' || ch == '.' || ch == ':' || ch == '-' || ch == '_' || ch == '\\' || ch == '!' || ch == '|' || ch == '"' || ch == '\'' || ch == '(' || ch == ')' ||  ch == '+' ||  ch == '-' || ch == EOF) break;
                    if (i == lim) {  /* make b bigger */
                        symbol * q = (symbol *) malloc((lim + INC) * sizeof(symbol));
                        memmove(q, b, lim * sizeof(symbol));
                        free(b); b = q;
                        //for (int j = lim; j < lim + INC; j++) b[j]='\0';
                        lim = lim + INC;
                    }
                    b[i] = ch; i++;
                    //b[i] = '\0';//Arregla un problema de corrupcion de datos
                    ch = getc(f_in);
                }
                
                /* INICIO Tratamiento de cada token */
                SN_set_current(z, i, b);                
                for(int j=i;j<lim;j++) b[j]='\0';
                /* FIN Tratamiento de cada token */
                
                {
                    int j;
                    //Copia Token en fichero salida
                    //eliminando los caracteres de puntuacion
                    // y las stop-words
                    if((z->l>0)&&(stop_words((char*)b)==0)){
                        for (j = 0; j < z->l; j++) {
                            fprintf(f_out, "%c", z->p[j]);
                        }
                        fprintf(f_out, "\n");
                    }
                }
                //free(b);
            }
        }
    }

    extern int stopping(char * in, char * out)
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
            stop_file(z, f_in, f_out);
            close_env(z);
            fclose(f_in);
            fclose(f_out);
        }

        return 0;
    }

