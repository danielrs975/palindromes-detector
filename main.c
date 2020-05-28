// Daniel Rodriguez 14-10955
// Jaqueline Farrach 14-10334

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define FOR(i, j, n); for(i = n; i < j; i++)
#define FORR(i, j, n); for(i = n, i < j, i--)


// Estructura que mantiene la informacion de un archivo: i_node
typedef struct stat i_node;

char candidato_palindromo[1024] = ".";

// Palindromos encontrado
char palindromos[1024];

// La siguiente variable almacena la carpeta en donde se quiere 
// formar el arbol


char *caracter_separacion = "/";
char carpeta_inicio[256] = "./";
int altura_maxima = 20;
int incluir_files = 0;
int pipe1[2];
int numero_palindromos = 0;


// Argumentos opcionales del programa
char *flagd = "-d";
char *flagm = "-m";
char *flagf = "-f";


// Funciones para leer e escribir en el pipe

void leer_del_pipe(int buf){
    FILE* stream;
    palindromos[0] = '\0';
    int iterador = 0;
    char caracter;
    stream = fdopen(buf, "r");
    while((caracter = fgetc(stream)) != EOF){
        if (caracter == '\n'){
            palindromos[iterador] = ',';
            iterador++;
            palindromos[iterador] = ' ';
            iterador++;
            numero_palindromos++;
            continue;
        }
        palindromos[iterador] = caracter;
        iterador++;
    }
    if (strlen(palindromos) > 0){
        printf("Los palindromos encontrados son los siguientes: %s\n", palindromos);
    }
    else{
        printf("No existen\n");
    }
}

void escribir_en_el_pipe(int file, char *palabra){
    FILE *stream;
    stream = fdopen(file, "w");
    fprintf(stream, "%s", palabra);
    fclose(stream);
}

// Funcion para detectar si un nodo es un archivo o un directorio.

int is_dir(const char* path) {
    i_node buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

// Programa hijo para reconocer palindromos. 
char *detectar_palindromo(char *palabra_directorio){
    int tamano_string = strlen(palabra_directorio);
    int iterador_principal;
    int iterador_izquierda, iterador_derecha;
    char respuesta[1024];
    char auxiliar[1024];
   
    char *separacion = ", ";
    
    respuesta[0] = '\0';

    FOR(iterador_principal, tamano_string, 0){
        iterador_izquierda = iterador_principal;
        iterador_derecha = iterador_principal;

        strncat(respuesta, palabra_directorio + iterador_principal, 1);
        // strcat(respuesta, palabra_directorio[iterador_principal]);
        if (iterador_derecha + 1 < tamano_string && palabra_directorio[iterador_izquierda] == palabra_directorio[iterador_derecha + 1]){
            strncat(respuesta, palabra_directorio + iterador_derecha, 1);
            // strncat(respuesta, palabra_directorio + iterador_derecha, 1);            
            iterador_derecha++;
        }
        while(--iterador_izquierda >= 0 && ++iterador_derecha < tamano_string){
            if (palabra_directorio[iterador_izquierda] == palabra_directorio[iterador_derecha]){
                strncat(respuesta, palabra_directorio + iterador_izquierda, 1);
                strncat(auxiliar, palabra_directorio + iterador_izquierda, 1);
                strcat(auxiliar, respuesta);
                respuesta[0] = '\0';
                strcpy(respuesta, auxiliar);
                auxiliar[0] = '\0';
            }
            else if (strlen(respuesta) >= 3){
                break;
            }
        }

        if (strlen(respuesta) >= 3){
            strcat(palindromos, respuesta);
            strcat(palindromos, separacion);
            // printf("Se detecto un palindromo: %s\n", respuesta);
        }
        respuesta[0] = '\0';
        auxiliar[0] = '\0';

    }


    return palindromos;
}

// Funcion para obtener el directorio en el cual se comienza la recursion
void obtener_carpeta_inicio(char *path){
    int iterador_string_1 = strlen(path) - 2;
    int iterador_string_2 = 0;
    char caracter = path[iterador_string_1];
    candidato_palindromo[0] = '\0';

    while(caracter != '/'){
        iterador_string_1--;
        caracter = path[iterador_string_1];
    }

    iterador_string_1++;
    caracter = path[iterador_string_1];
    while(caracter != '/'){
        candidato_palindromo[iterador_string_2] = caracter;
        iterador_string_2++;
        iterador_string_1++;
        caracter = path[iterador_string_1];
    }
}

// Recorrido del arbol de directorios 
void dfs(char *path_inicio, char *cadena, int altura){

    // Declaro las estructuras necesarias para poder recorrer
    // el arbol de directorios

    struct dirent *directorio;
    char copia_path[1024];
    char copia_cadena[1024];

    // Contador de numero de archivos dentro de un directorio
    int numero_archivos = 0;

    strcpy(copia_path, path_inicio);
    
    // Si el arbol alcanzo la altura maxima entonces corta el dfs en ese punto
    if (altura + 1 > altura_maxima){
        return;
    }

    // Abro el directorio
    DIR *directorio_actual = opendir(path_inicio);
    // Recorro el contenido de tal directorio.
    while ((directorio = readdir(directorio_actual)) != NULL){
        // Verifico si el archivo actual es un archivo o un directorio
        // Los directorios que no se van a leer son . y ..
        if (strcmp(directorio -> d_name, ".") == 0 || strcmp(directorio ->d_name, "..") == 0){
            continue;
        }
        // printf("%s\n", strcat(copia_path, directorio -> d_name));
        if (is_dir(strcat(copia_path, directorio -> d_name))){
            // Si es un directorio entonces vuelvo a hacer dfs sobre esa
            // direccion
            strcat(copia_path, caracter_separacion);
            strcpy(copia_cadena, cadena);
            strcat(copia_cadena, directorio -> d_name);
            
            int copia_altura = altura;
            
           

            pid_t proceso_hijo = fork();

            if (proceso_hijo == 0){

                palindromos[0] = '\0';
                char *palindromo = detectar_palindromo(copia_cadena);
                // Parte de comunicacion con el padre.....
                close(pipe1[0]);

                escribir_en_el_pipe(pipe1[1], palindromo);

                // printf("Palindromos encontrado en %s: %s\n", copia_cadena, palindromo);
                // Salida del proceso hijo 
                exit(EXIT_SUCCESS);
            }
            
    
            dfs(copia_path, copia_cadena, ++copia_altura);
        }
        else if (incluir_files == 1){
            strcpy(copia_cadena, cadena);
            strcat(copia_cadena, directorio -> d_name);
            // printf("Palindromo: %s\n", copia_cadena);
            pid_t proceso_hijo = fork();

            if (proceso_hijo == 0){

                palindromos[0] = '\0';
                char *palindromo = detectar_palindromo(copia_cadena);
                // Parte de comunicacion con el padre.....
                close(pipe1[0]);

                escribir_en_el_pipe(pipe1[1], palindromo);

                // printf("Palindromos encontrado en %s: %s\n", copia_cadena, palindromo);
                // Salida del proceso hijo 
                exit(EXIT_SUCCESS);
            }
        }
    
        copia_path[0] = '\0';
        strcpy(copia_path, path_inicio);
        numero_archivos++;
    }
    // if (numero_archivos == 0){
        // printf("Palindromo: %s\n", cadena);
    // }
}

int main(int argc, char* argv[]){
    
    // Crear el pipe
    pipe(pipe1);

    int iterador_argumentos;

    // Analisis de los argumentos opcionales
    char* argumento;
    // Recorro los argumentos opcionales que son puestos como entrada 
    FOR(iterador_argumentos, argc, 1){
        argumento = argv[iterador_argumentos];
        // printf("El string %s es igual al string -d: %d\n", argumento, strcmp(argumento, flagd));
        if (strcmp(argumento, flagd) == 0){
            // Se ve si se puso una carpeta de inicio
            if (iterador_argumentos + 1 >= argc){
                // Si no se puso entonces da un mensaje de error
                // y se sale del programa
                printf("No se coloco carpeta de inicio\nSaliendo del programa...\n");
                exit(EXIT_FAILURE);
            }
            else {
                // Si se puso una carpeta inicio entonces se guarda en una variable global
                // y se empieza a construir el arbol a partir de ahi
                strcpy(carpeta_inicio, argv[iterador_argumentos + 1]);
                // Aqui obtengo el directorio el cual comienza la recursion
                // obtener_carpeta_inicio(carpeta_inicio);
                iterador_argumentos++;
            }
        }
        else if(strcmp(argumento, flagm) == 0){
            // Ve si se puso un limite a la altura del arbol de directorios
            // a construir
            if (iterador_argumentos + 1 >= argc){
                // Si no se puso un entero que represente la altura
                // maxima del arbol entonces se reporta un error
                printf("No se coloco la altura maxima\nSaliendo del programa...\n");
                exit(EXIT_FAILURE);
            }
            else {
                // Si se puso la altura maxima entonces se procede a sobreescribir
                // el valor default de la variable que almacena esta opcion
                altura_maxima = atoi(argv[iterador_argumentos + 1]);
                iterador_argumentos++;
            }
        }
        else if (strcmp(argumento, flagf) == 0){
            // Si esta opcion se marca entonces se incluyen los nombres 
            // de los archivos en la busqueda del palindromo
            incluir_files = 1;
        }
    }


    // Comienzo del programa....

    // Prueba DFS #1
    dfs(carpeta_inicio, candidato_palindromo, 1);


    // Leer del buffer:
    close(pipe1[1]);
    
    leer_del_pipe(pipe1[0]);
    
    // Terminacion del programa.
    exit(EXIT_SUCCESS);
}