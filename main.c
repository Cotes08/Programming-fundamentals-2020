#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "funciones.h"
#include <dirent.h>   /* dirent: directory entries (entradas de directorio) */
#include <sys/stat.h> /* stat: status (estado de los ficheros/directorios)  */

int main(int argc, char *argv[])
{
    Lista *lista = listaIncial();//Se inicializa la variable lista
    ListaRow *listaRow = listaIncialR();//Se inicializa la variable listaRow
    Nodo *nodoTab = NULL;
    FILE *file;
    char *comando[300], prompt[300] = "*", *dato[300], *tipos[300], *campos[300], nombredb[300] = "", nombretab[300] = "", archivoTab[300], entrada[300], entradaAux[300], operador, *tablas;
    char promptaux[100];
    char separadores[10] = " \n\t\r";
    char *dir = _getcwd(NULL, 0);
    int numCampos, i, j, numTablas, numCamposTAB = 0, control = 0, operadorENETRO;

    printf("Alumno: Manuel Garcia Cotes\nGMAIL: manuel.garcia32@goumh.umh.es\n");
    printf("- Ejercicio 1: HECHO\n");
    printf("- Ejercicio 2: HECHO\n");
    printf("- Ejercicio 3: HECHO\n");
    printf("- Ejercicio 4: SIN HACER\n\n\n");
    while (1)
    {
        int num = 0, cont = -1, aux = 0;
        numTablas = lista->longitud;//Actualizamos numTablas en cada ejecucion para que no haya problemas
        fflush(stdin);
        printf("\n[%s]:>", prompt);

        // Pillamos la entrada
        gets(entrada);
        // Creamos una entrada auxiliar que luego se explicara porque
        strcpy(entradaAux, entrada);
        if (strcmp(entrada, "") == 0)
        {
            printf("Error: No se ha introducido ningun parametro\n");
            continue;
        }

        // Como aqui desde un principio tenemos que trocear la cadena entrada, si posteriormente la queremos
        // usar quedara inservible de ahi que hagamos una entradaAux para tener una cadena que sirva.
        comando[num] = MiStrTok(entrada, separadores, 0);
        // Comprobamos si es insert o no
        if (strcmp(comando[0], "insert") == 0)
        {
            // Si lo es descomponemos la cadena con comillas
            comando[num] = MiStrTok(entradaAux, separadores, 1);
            while (comando[num] != NULL)
            {
                num++;
                comando[num] = MiStrTok(NULL, separadores, 1);
            }
        }
        else
        {
            // Si no lo es lo hacemos normal
            comando[num] = MiStrTok(entradaAux, separadores, 0);
            while (comando[num] != NULL)
            {
                num++;
                comando[num] = MiStrTok(NULL, separadores, 0); // En los strtok se pone NULL para que no se sobreescriba el contenido de entradaAux
            }
        }

        if (strcmp(comando[0], "exit") == 0) // Si lo que le pasmos es exit sale
        {
            if (strcmp(comando[1], "") == 0)
            {
                return 0;
            }
            else
            {
                printf("Error: Cantidad de parametros incorrecta\n");
            }
        }
        else if (strcmp(comando[0], "new") == 0) // Entramos dentro de new
        {
            if (comando[1] != NULL)
            {
                if (strcmp(comando[1], "db") == 0)
                {
                    if (num == 3)//new db (lo que sea)
                    {
                        if (dirExists(comando[2]) == 0) // Comprobamos si el directorio existe, si existe da error y sino lo crea
                        {
                            chdir(dir);                 // Nos vamos al directorio raiz para crearlo
                            if (mkdir(comando[2]) == 0) // Para comprobar que se crea bien la bd
                            {
                                chdir(comando[2]);             // Nos vamos al nuevo directorio de la bd
                                strcpy(nombredb, comando[2]);  // guardamos el nombre de la base de datos que acabamos de crear
                                file = fopen("meta.db", "w+"); // creamos el fichero meta.db
                                fprintf(file, "Tablas: 0\n");
                                fprintf(file, "----------------------\n");
                                fclose(file);
                                chdir(dir); // Volvemos al directorio raiz
                            }
                            else
                            {
                                printf("Error: El directorio no se ha podido crear\n");
                            }
                        }
                        else
                        {
                            printf("Error: El directorio ya existe\n");
                        }
                    }
                    else
                    {
                        printf("Error: Cantidad de parametros incorrectos\n");
                    }
                }
                else if (strcmp(comando[1], "table") == 0)
                {
                    if (num < 5 || num % 2 == 0) // Comprobamos que el numero de parametros son mas de 5 es decir (new table (tabla, campo, tipo)) y que sean impares
                    {
                        printf("Error: El numero de parametros es incorrecto \n");
                        continue; // Si no es asi se hace continue para salir otra vez al while
                    }
                    if (strcmp(nombredb, "") == 0) // Comprobamos que estamos en una db, es decir si nombredb es nulo es que no estamos en ninguna db
                    {
                        printf("Error: No estas en ninguna base de datos \n");
                        continue;
                    }
                    numCampos = (num - 3) / 2; /*Esto es para saber cuantos campos son, ya que le quitamos 3 para poder no contar con el new table (nombre de la tabla)
                                                 luego el campo entendido como parametro y tipo cuenta como 1 por eso el dividido entre 2.*/

                    // Ahora comprobamos si la tabla ya existe
                    if (buscarTabla(lista, comando[2]) != NULL)
                    {
                        printf("Error: La tabla ya existe y no se puede crear otra con el mismo nombre\n");
                        continue;
                    }

                    // De esta forma vamos a descomponer los campos y los tipos y los guardaremos en sus variables adecuadas.
                    for (i = 0; i < numCampos; i++)
                    {
                        campos[i] = comando[i * 2 + 3];
                        tipos[i] = comando[i * 2 + 4];
                    }
                    // Con este if comprobamos que los campos no estan duplicados
                    if (buscarDuplicados(campos, numCampos) == 1)
                    {
                        printf("Error: No pueden haber campos duplicados \n");
                        continue;
                    }
                    // Aqui detectamos que el tipo es correcto
                    for (i = 0; i < numCampos; i++)
                    {
                        if (detectarTipo(tipos[i]) == -1)
                        {
                            printf("Error: El tipo introducido no es correcto \n");
                            break; // Aqui hacemos brake y no continue porque el continue en el for no pararia la ejecucion
                        }
                    }
                    if (i < numCampos) // Si la i es menor que num campos es por que el bucle anterior salio antes de tiempo por lo tanto ha dado algun error y salimos del new table con esta funcion
                    {
                        printf("\n el numero de campos es: %i", numCampos);
                        continue;
                    }

                    sprintf(archivoTab, "%s.tab", comando[2]); // Creamos el archivo .tab
                    file = fopen(archivoTab, "w");
                    fprintf(file, "");
                    fclose(file);

                    InsertarFinal(lista, comando[2], numCampos, campos, tipos); // Introducimos los campos introducidos en el lista

                    if (lista->cabeza == NULL) // Comprobamos que se han insertado bien
                    {
                        printf("\n Error de memoria");
                        continue;
                    }
                    EscribirMETA(lista); // Ahora escribimos el meta db
                }
                else
                {
                    printf("Error: Cantidad de parametros incorrecta\n");
                }
            }
            else
            {
                printf("Error: Cantidad de parametros incorrecta\n");
            }
        }
        else if (strcmp(comando[0], "use") == 0)
        {
            if (comando[1] != NULL)
            {
                if (strcmp(comando[1], "db") == 0)
                {
                    if (num == 3)//use db (lo que sea)
                    {
                        chdir(dir);                 // Nos aseguramos de estar en la raiz
                        if (chdir(comando[2]) == 0) // Comprobamos que la base de datos existe y nos movemos a ella
                        {
                            strcpy(prompt, comando[2]);   // Copiamos la base de datos al prompt
                            strcpy(nombredb, comando[2]); // guardamos el nombre de la base de datos que acabamos de crear
                        }
                        else
                        {
                            printf("La base de datos no existe \n");
                            continue;
                        }
                        if (lista != NULL)
                        {
                            EliminarTab(lista);//Esto es para eliminar lo que hay en memoria del meta.db
                            LeerMeta(lista, &numTablas);//Leemos el nuevo meta.db

                            if (numTablas == 0)
                            {
                                printf("No hay tablas \n");
                            }
                            else
                            {
                                printf("Lista de las tablas: ");
                                nodoTab = lista->cabeza;
                                while (nodoTab != NULL)
                                {
                                    printf("\n -%s ", nodoTab->nombre);
                                    nodoTab = nodoTab->sig;
                                }
                                printf("\n");
                                nodoTab = NULL;
                            }
                        }
                    }
                    else
                    {
                        printf("Cantidad de parametros incorrecta\n");
                    }
                }
                else if (strcmp(comando[1], "table") == 0)
                {
                    if (num == 3)// use table (lo que sea)
                    {
                        if (strcmp(nombredb, "") != 0)//comprobamos que estamos en una base de datos
                        {
                            sprintf(archivoTab, "%s.tab", comando[2]);
                            if (seleccionarTAB(archivoTab) == 0) // De esta forma me aseguro de estar en la tabla adecuada
                            {
                                // Para poder utilizar una tabla primero la debemos encontrar si la tabla no existe no hacemos nada ya que no existe
                                if ((nodoTab = buscarTabla(lista, comando[2])) != NULL)//Esto me devuelve un puntero tipo nodo al lugar donde se encuentre la tabla que se busca
                                {
                                    strcpy(nombretab, comando[2]);
                                    sprintf(prompt, "%s/%s", nombredb, nombretab);

                                    numCamposTAB = nodoTab->numCampos;

                                    if (listaRow != NULL)
                                    {
                                        EliminarRow(listaRow, numCamposTAB);//Eliminamos de memoria las rows(valores de los.tab)
                                    }
                                    LeerTAB(listaRow, numCamposTAB, archivoTab);//leemos el nuevo .tab
                                }
                                else
                                {
                                    printf("Error la tabla no existe \n");
                                }
                            }
                            else
                            {
                                printf("Error fichero .tab no existe \n");
                            }
                        }
                        else
                        {
                            printf("Error: No estas en ninguna base de datos \n");
                            continue;
                        }
                    }
                    else
                    {
                        printf("Cantidad de parametros incorrecta\n");
                    }
                }
                else
                {
                    printf("Parametros incorrectos\n");
                }
            }
            else
            {
                printf("Cantidad de parametros incorrecta\n");
            }
        }
        else if (strcmp(comando[0], "insert") == 0)
        {
            if (num > 1)
            {
                if (strcmp(nombredb, "") != 0)
                {
                    if (strcmp(nombretab, "") != 0)
                    {
                        numCamposTAB = nodoTab->numCampos;//El nodo tab que pillamos anteriormente en el use table

                        if ((num - 1) == numCamposTAB)//Si los parametros introducidos (excluyendo el insert) son iguales se puede continuar 
                        {
                            for (i = 0; i < numCamposTAB; i++)
                            {
                                if (strcmp(tipoAChar(nodoTab->tipos[i]), "TEXT") == 0)//Controlamos si es del tipo correcto
                                {
                                    control = controlarEspacios(comando[i + 1]);
                                    if (control == 0) // Esto significa que hay una cadena sin espacios por lo tanto no hay comillas
                                    {
                                        if (isText(comando[i + 1]) == 1)
                                        {
                                            printf("ERROR: El parametro introducido no es del tipo correcto\n");
                                            break;
                                        }
                                    }
                                }
                                else if (strcmp(tipoAChar(nodoTab->tipos[i]), "NUM") == 0)//Controlamos si es del tipo correcto
                                {
                                    if (isNumber(comando[i + 1]) == 1)
                                    {
                                        printf("ERROR: El parametro introducido no es del tipo correcto\n");
                                        break;
                                    }
                                }
                                else if (strcmp(tipoAChar(nodoTab->tipos[i]), "DATE") == 0)//Controlamos si es del tipo correcto
                                {
                                    if (valFecha(comando[i + 1]) == 1)
                                    {
                                        printf("ERROR: El parametro introducido no es del tipo correcto\n");
                                        break;
                                    }
                                }
                                else if (strcmp(tipoAChar(nodoTab->tipos[i]), "EMAIL") == 0)//Controlamos si es del tipo correcto
                                {
                                    if (isEmail(comando[i + 1]) == 1)
                                    {
                                        printf("ERROR: El parametro introducido no es del tipo correcto\n");
                                        break;
                                    }
                                }
                            }
                            if (i < numCamposTAB)
                            {
                                continue;
                            }
                            InsertarFinalRow(listaRow, comando, numCamposTAB, 1);//Si se cumple todo se inserta en memoria 
                            EscribirTAB(listaRow, numCamposTAB, archivoTab);//y se inserta en el tab
                        }
                        else
                        {
                            printf("Cantidad de parametros incorrecta\n");
                        }
                    }
                    else
                    {
                        printf("Error: No hay ninguna tabla seleccionada \n");
                    }
                }
                else
                {
                    printf("Error: No estas en ninguna base de datos \n");
                    continue;
                }
            }
            else
            {
                printf("Cantidad de parametros incorrecta\n");
            }
        }
        else if (strcmp(comando[0], "select") == 0)
        {
            if (strcmp(nombredb, "") != 0)//Comprobamos que estamos en una db 
            {
                if (strcmp(nombretab, "") != 0)//Comprobamos que estamos en un tab
                {
                    sprintf(archivoTab, "%s.tab", nombretab);
                    if (seleccionarTAB(archivoTab) == 0)//Seleccionamos el archivo tab
                    {
                        if ((nodoTab = buscarTabla(lista, nombretab)) != NULL)//buscamos el nodo en cuestion 
                        {
                            if (num == 1)//si esta solo el select se imprime y ya
                            {
                                camposMetaSelect(nodoTab);
                                ImprimirValoresSelect(nodoTab, listaRow);
                            }
                            else if (num == 4)//si hay mas parametros hay que comprobarlos
                            {
                                operador = *comando[2];//guardar el <, > o =
                                operadorENETRO = (int)operador;//lo pasamos a int (ASCII)
                                if (operadorENETRO == 60 || operadorENETRO == 61 || operadorENETRO == 62)//Si es > < o = entra
                                {

                                    for (int j = 0; j < nodoTab->numCampos; j++)//Recorremos todos los campos 1 a 1
                                    {
                                        if (strcmp(nodoTab->campos[j], comando[1]) == 0)//si el campos que encontramos es igual al que le hemos pasado entra
                                        {
                                            //se comprueba de que tipo es y si se valida correcto el resto lo hace la funcion 
                                            /**
                                             * Es decir lo que se hara es sacar un valor con el =
                                             * o sacar los valores > a x
                                             * o sacar los valores < a x
                                             * mas imprimir la cabecera con los nombres de las tablas de los campos
                                             */
                                            if (nodoTab->tipos[j] == DATE && valFecha(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == NUM && isNumber(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == EMAIL && isEmail(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == TEXT)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else
                                            {
                                                printf("Error: Tipo no correcto\n");
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            cont++;
                                        }
                                        aux = j;
                                    }
                                    if (cont == aux)
                                    {
                                        printf("Error: campo no encontrado\n");
                                        continue;
                                    }
                                }
                                else
                                {
                                    printf("Error: el operador relacional no es correcto\n");
                                    continue;
                                }
                            }
                            else if (num == 5)//Esto es lo mismo que lo anterior pero controlando si es ASC o DESC 
                            {                 //Estos valores se le pasaran a la funcion que se encargara de todo
                                operador = *comando[2];
                                operadorENETRO = (int)operador;
                                if (operadorENETRO == 60 || operadorENETRO == 61 || operadorENETRO == 62)
                                {

                                    for (int j = 0; j < nodoTab->numCampos; j++)
                                    {
                                        if (strcmp(nodoTab->campos[j], comando[1]) == 0)
                                        {

                                            if (nodoTab->tipos[j] == DATE && valFecha(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == NUM && isNumber(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == EMAIL && isEmail(comando[3]) == 0)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else if (nodoTab->tipos[j] == TEXT)
                                            {
                                                camposMetaSelect(nodoTab);
                                                selectOperadorOrden(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, comando[4]);
                                            }
                                            else
                                            {
                                                printf("Error: Tipo no correcto\n");
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            cont++;
                                        }
                                        aux = j;
                                    }
                                    if (cont == aux)
                                    {
                                        printf("Error: campo no encontrado\n");
                                        continue;
                                    }
                                }
                                else
                                {
                                    printf("Error: el operador relacional no es correcto\n");
                                    continue;
                                }
                            }

                            else
                            {
                                printf("Error: Cantidad de paraemtros incorrecta\n");
                            }
                        }
                    }
                    else
                    {
                        printf("Error: error al abrir la tabla\n");
                    }
                }
                else
                {
                    printf("Error: No hay ninguna tabla seleccionada\n");
                }
            }
            else
            {
                printf("Error: No estas en ninguna base de datos\n");
            }
        }
        else if (strcmp(comando[0], "delete") == 0)//Lo mismo que la funcion select sin asc o desc
        {
            if (strcmp(nombredb, "") != 0)
            {
                if (strcmp(nombretab, "") != 0)
                {
                    sprintf(archivoTab, "%s.tab", nombretab);
                    if (seleccionarTAB(archivoTab) == 0)
                    {
                        if ((nodoTab = buscarTabla(lista, nombretab)) != NULL)
                        {
                            if (num == 4)
                            {
                                operador = *comando[2];
                                operadorENETRO = (int)operador;
                                if (operadorENETRO == 60 || operadorENETRO == 61 || operadorENETRO == 62)
                                {

                                    for (int j = 0; j < nodoTab->numCampos; j++)
                                    {
                                        if (strcmp(nodoTab->campos[j], comando[1]) == 0)
                                        {
                                            /**
                                             * La principal diferencia que hay con esta funcion es que tiene la funcion delete 
                                             * que eliminara los campos segun lo que se le ha indicado
                                             * cuando los campos se eliminien se actualizara tanto el archivo como lo que hay en memoria
                                             * lo adicional es que he reusado las funciones de sacar el nombre de las columnas de los campos
                                             * y tambien volver a imprimir los campos que queden para ver los cambios
                                             */
                                            if (nodoTab->tipos[j] == DATE && valFecha(comando[3]) == 0)
                                            {
                                                deleteOperador(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, archivoTab);
                                                camposMetaSelect(nodoTab);
                                                ImprimirValoresSelect(nodoTab, listaRow);
                                            }
                                            else if (nodoTab->tipos[j] == NUM && isNumber(comando[3]) == 0)
                                            {
                                                deleteOperador(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, archivoTab);
                                                camposMetaSelect(nodoTab);
                                                ImprimirValoresSelect(nodoTab, listaRow);
                                            }
                                            else if (nodoTab->tipos[j] == EMAIL && isEmail(comando[3]) == 0)
                                            {
                                                deleteOperador(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, archivoTab);
                                                camposMetaSelect(nodoTab);
                                                ImprimirValoresSelect(nodoTab, listaRow);
                                            }
                                            else if (nodoTab->tipos[j] == TEXT)
                                            {
                                                deleteOperador(listaRow, nodoTab->tipos[j], operadorENETRO, comando[3], comando[1], nodoTab, archivoTab);
                                                camposMetaSelect(nodoTab);
                                                ImprimirValoresSelect(nodoTab, listaRow);
                                            }
                                            else
                                            {
                                                printf("Error: Tipo no correcto\n");
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            cont++;
                                        }
                                        aux = j;
                                    }
                                    if (cont == aux)
                                    {
                                        printf("Error: campo no encontrado\n");
                                        continue;
                                    }
                                }
                                else
                                {
                                    printf("Error: el operador relacional no es correcto\n");
                                    continue;
                                }
                            }
                            else
                            {
                                printf("Error: Cantidad de paraemtros incorrecta\n");
                            }
                        }
                    }
                    else
                    {
                        printf("Error: error al abrir la tabla\n");
                    }
                }
                else
                {
                    printf("Error: No hay ninguna tabla seleccionada\n");
                }
            }
            else
            {
                printf("Error: No estas en ninguna base de datos\n");
            }
        }
        else if (strcmp(comando[0], "list") == 0)
        {
            if (num==2)
            {
                if (strcmp(comando[1], "db")==0)
                {
                   chdir(dir);
                    /* Con un puntero a DIR abriremos el directorio */
                    DIR *dir;
                    struct dirent *myfile;
                    FILE *f;
                    cont = 0;
                    char fullName[200];

                    /* Empezaremos a leer en el directorio actual */
                    dir = opendir(".");
                    printf("Bases de datos disponibles: ");
                    /* Leyendo uno a uno todos los archivos que hay */
                    while ((myfile = readdir(dir)) != NULL)
                    {
                        /* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
                        if ((strcmp(myfile->d_name, ".") != 0) && (strcmp(myfile->d_name, "..") != 0))
                        {
                            // Cuando encuentre el archivo lo abirara y comprobara si tiene el meta.db si lo tiene lo imprime
                            sprintf(fullName, "%s/meta.db", myfile->d_name);

                            if ((f = fopen(fullName, "r")) != NULL)
                            {
                                printf("\n -%s", myfile->d_name);
                                cont++;
                                fclose(f);
                            }
                        }
                    }
                    closedir(dir);

                    if (cont == 0)
                    {
                        printf("\n No hay bases de datos");
                    }
                    printf("\n ");

                    if (strcmp(nombredb, "") != 0) // Comprobamos que estamos en una db, es decir si nombredb es nulo es que no estamos en ninguna db
                    {
                        chdir(nombredb);
                    }
                }
                else if (strcmp(comando[1], "table")==0)
                {
                    if (strcmp(nombredb, "") != 0)
                    {
                        if (numTablas == 0)
                        {
                            printf("No hay tablas \n");
                        }
                        else
                        {
                            printf("Lista de las tablas: ");
                            nodoTab = lista->cabeza;
                            while (nodoTab != NULL)
                            {
                                printf("\n -%s ", nodoTab->nombre);
                                nodoTab = nodoTab->sig;
                            }
                            printf("\n");
                            nodoTab = NULL;
                        }    
                            
                    }
                    else
                    {
                        printf("Error: No estas en ninguna base de datos\n");
                    }
                }
                else
                {
                    printf("\n Error: parametro incorrecto");
                }        
            }
            else
            {
                printf("\n Error: Cantidad de parametros incorrecta");
            }
        }
        else if(strcmp(comando[0], "order") == 0)
        {
            if (num==3)
            {
                if (strcmp(nombredb, "") != 0)//Comprobamos que estamos en una db 
                {
                    if (numTablas!=0)
                    {
                        if (strcmp(nombretab, "") != 0)
                        {
                            sprintf(archivoTab, "%s.tab", nombretab);
                            if (seleccionarTAB(archivoTab) == 0)//Seleccionamos el archivo tab
                            {
                                if ((nodoTab = buscarTabla(lista, nombretab)) != NULL)//buscamos el nodo en cuestion 
                                { aux=0;
                                    for (int i = 0; i < nodoTab->numCampos; i++)
                                    {
                                        if (strcmp(comando[1], nodoTab->campos[i])==0)
                                        {
                                            funcOrder(listaRow, nodoTab, comando[1], comando[2], archivoTab);
                                            break;
                                        } 
                                        else
                                        {
                                            aux++;
                                        }   
                                    }
                                    if (aux==(nodoTab->numCampos))
                                    {
                                        printf("Error: el campo introducido no existe\n");
                                    } 
                                }
                            }
                            else
                            {
                                printf("Error: La tabla no es correcta\n");
                            }
                        }
                        else
                        {
                            printf("Error: No hay ninguna tabla seleccionada\n");
                        } 
                    }
                    else
                    {
                        printf("La base de datos no tiene tablas\n");
                    }      
                }
                else
                {
                    printf("Error: No estas en ninguna base de datos\n");
                }
            }
            else
            {
                printf("Error: Cantidad de parametros incorrecta \n ");
            }                   
        }
        else if (strcmp(comando[0], "remove") == 0)
        {
            if (num==2)
            {
                if (strcmp(nombredb, "") != 0)//Comprobamos que estamos en una db 
                {
                    strcpy(nombretab, comando[1]);
                    sprintf(archivoTab, "%s.tab", nombretab);
                    if (seleccionarTAB(archivoTab) == 0)//Seleccionamos el archivo tab
                    {
                        sprintf(promptaux, "%s/%s", nombredb, nombretab);
                        if (strcmp(promptaux, prompt)==0)
                        {
                            strcpy(prompt, nombredb);
                            remove(archivoTab);
                            actualizarMeta(lista, nombretab);
                        }
                        else
                        {
                            remove(archivoTab);
                            actualizarMeta(lista, nombretab);
                        }  
                    }
                    else
                    {
                        printf("Error: la tabla indicada no es correcta\n");
                    }
                }
                else
                {
                    printf("Error: No estas en ninguna base de datos\n");
                }
            }
            else
            {
                printf("\n Error: Cantidad de parametros incorrecta");
            }
            
            
        }
        else
        {
            printf("\n Error: Parametro incorrecto");
        }
    }

    return 0;
}


