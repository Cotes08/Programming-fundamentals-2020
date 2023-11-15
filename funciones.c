
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

int contador = 0;

// Funcion de strtok
/**
 * Esta funcion lo que nos permite es activar la lectura de comillas o no 
 * Es decir si el int comillas es a 1 va a leer desde el principio de las comilas hasta el final 
 */
char *MiStrTok(char *cad1, char *sep, int comillas)
{
    static char *pt1 = NULL;
    static char *pt2 = NULL;
    static char *ptFinal;

    if (cad1 != NULL)
    {
        pt1 = cad1;
        ptFinal = pt1 + strlen(cad1);
    }
    else
    {
        pt1 = pt2 + 1;
    }

    if (comillas)
    {
        while (pt1[0] != '\0' && pt1 < ptFinal && strchr(sep, pt1[0]) != NULL && pt1[0] != '"')
            pt1++;
        if (pt1[0] == '"')
        {
            pt1++;
            pt2 = pt1 + 1;
            while (pt2[0] != '\0' && pt2[0] != '"')
                pt2++;
            if (pt2[0] == '\0')
            {
                pt1 = pt2 = ptFinal = NULL;
                return NULL;
            }
            else
            {
                pt2[0] = '\0';
                return pt1;
            }
        }
    }
    else
    {
        while (pt1[0] != '\0' && pt1 < ptFinal && strchr(sep, pt1[0]) != NULL)
            pt1++;
    }

    if (pt1 >= ptFinal)
    {
        pt1 = pt2 = ptFinal = NULL;
        return NULL;
    }

    pt2 = pt1 + 1;
    while (pt2[0] != '\0' && strchr(sep, pt2[0]) == NULL)
        pt2++;
    pt2[0] = '\0';

    return pt1;
}

// Esta funcion controla si el tab existe 
int seleccionarTAB(char *archivoTAB)
{
    FILE *file;
    if (file = fopen(archivoTAB, "r"))
    {
        fclose(file);
        return 0;
    }
    else
    {
        return 1;
    }
}

// Para comprobar si hay espacios, es decir, si hay espacio es una cadena con comillas, y si es asi todo lo de dentro se considera tipo texto,
// en el caso de que no hayan comillas el espacio no sera valido
int controlarEspacios(char *tmp)
{
    int aux = 0;

    for (int i = 0; tmp[i] != '\0'; i++)
    {
        if ((tmp[i]) == ' ')
        {
            // Hay espacios y por lo tanto comillas
            aux = 1;
            break;
        }
        else
        {
            // no hay espacios
            aux = 0;
        }
    }
    return aux;
}

// Reservamos memoria para la lista para inicializarla a nulo
Lista *listaIncial()
{
    Lista *l = malloc(sizeof(Lista));
    if (l == NULL)
        return NULL;
    l->cabeza = NULL;
    l->longitud = 0;

    return l;
}

// Reservamos memoria para la lista para inicializarla a nulo
ListaRow *listaIncialR()
{
    ListaRow *l = malloc(sizeof(ListaRow));
    if (l == NULL)
        return NULL;
    l->cabeza = NULL;
    l->filas = 0;

    return l;
}

// Funcion para comprobar si un directorio existe
int dirExists(const char *path)
{
    struct stat info;

    if (stat(path, &info) != 0)
        return 0;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

// Funcion para detectar el tipo
//Es decir el parametro que reciba como texto lo transformara a type 
TYPE detectarTipo(char *token)
{
    if (strcmp(token, "NUM") == 0)
    {
        return NUM;
    }
    else if (strcmp(token, "TEXT") == 0)
    {
        return TEXT;
    }
    else if (strcmp(token, "DATE") == 0)
    {
        return DATE;
    }
    else if (strcmp(token, "EMAIL") == 0)
    {
        return EMAIL;
    }
    else
    {
        return -1;
    }
}
// Pasar de type a char
//Es decir el parametro que reciba como type lo transformara a texto 
char *tipoAChar(TYPE tipo)
{
    if (tipo == TEXT)
    {
        return "TEXT";
    }
    else if (tipo == NUM)
    {
        return "NUM";
    }
    else if (tipo == DATE)
    {
        return "DATE";
    }
    else if (tipo == EMAIL)
    {
        return "EMAIL";
    }
}
// Para buscar para ver si hay algun campo duplicado
//Para comprobar que en un mismo insert el campo no se duplica 
int buscarDuplicados(char **campos, int numCampos)
{
    for (int i = 0; i < numCampos - 1; i++)
    {
        for (int j = i + 1; j < numCampos; j++)
        {
            if (strcmp(campos[i], campos[j]) == 0)
                return 1;
        }
    }
    return 0;
}
// Crear los nodos de las tablas
Nodo *CrearNodo(char *nombre, int numCampos, char **campos, char **tipos)
{
    Nodo *nodo = (Nodo *)malloc(sizeof(Nodo));         // Reservamos memoria para el nodo
    strcpy(nodo->nombre, nombre);                      // Copiamos la variable que le pasamos a la nueva estructura
    nodo->numCampos = numCampos;                       // Copiamos la variable campos a la variable de la estructura
    nodo->campos = malloc(numCampos * sizeof(char *)); // Reservamos memoria para los campos
    nodo->tipos = malloc(numCampos * sizeof(TYPE));    // Reservamos memoria para los tipos
    for (int i = 0; i < numCampos; i++)
    {
        nodo->campos[i] = strdup(campos[i]);     // Recorremos los campos para reservar memoria 1 a 1
        nodo->tipos[i] = detectarTipo(tipos[i]); // Recorremos los tipos para insertarlos 1 a 1
    }
    nodo->sig = NULL;
    nodo->ant = NULL;

    return nodo;
}
// Crear el nodo row
ROW *CrearNodoRow(char **valores, int numCampos, int sum)
{
    ROW *row = (ROW *)malloc(sizeof(ROW));             // Reservamos memoria para el nodoRow
    row->valores = malloc(numCampos * sizeof(char *)); // Reservamos memoria para las rows
    for (int i = 0; i < numCampos; i++)
    {
        row->valores[i] = strdup(valores[i + sum]);
    }
    row->sig = NULL;
    row->ant = NULL;

    return row;
}

// Funcion para buscar en la tabla
//Se le pasa el campo por parametro y se comprueba si esta en la tabla
//Si lo encuentra nos devuelve el puntero apuntando a ese nodo
Nodo *buscarTabla(Lista *lista, char *nombreTAB)
{
    Nodo *puntero = lista->cabeza; // Creamos una variable llamada puntero
    while (puntero != NULL)
    {
        if (strcmp(puntero->nombre, nombreTAB) == 0) // Vamos comprobando campo a campo si son iguales
        {
            return puntero; // Si lo son devuelve el puntero y si no devuelve nullo
        }
        puntero = puntero->sig;
    }
    return NULL;
}
// Para insertar los nodos en la lista
//Para añadir los nodos y organizar las conexiones de siguiente y anterior
void InsertarFinal(Lista *lista, char *nombre, int numCampos, char **campos, char **tipos)
{
    // Creamos un nodo con los datos
    Nodo *nodo = CrearNodo(nombre, numCampos, campos, tipos);
    // Aqui comprobamos si es el primer nodo, en el caso de que lo sea sera cabecera
    if (lista->cabeza == NULL)
    {
        lista->cabeza = nodo;
    }
    else
    {
        // Aqui declaramos un puntero para recorrer los elementos
        Nodo *puntero = lista->cabeza;
        while (puntero->sig != NULL)
        {
            puntero = puntero->sig;
        }
        // Una vez tengamos el elemento ponemos que el anterior del nodo es a lo que apunta ahora nuestra variale punter
        nodo->ant = puntero;
        // El siguiente de lo que apunta nuestro puntero es el nodo.
        puntero->sig = nodo;
        // Para garantizar que toda la lista queda "cerrada" lo siguiente del nodo es null.
        nodo->sig = NULL;
    }
    // Para llevar la cuenta de cuantos elementos se insertan.
    lista->longitud++;
}

// Para insertar los nodos en la de tipo ROW
void InsertarFinalRow(ListaRow *listaRow, char **valores, int numCampos, int sum)
{
    // Creamos un nodo con los datos
    ROW *nodoRow = CrearNodoRow(valores, numCampos, sum);
    // Aqui comprobamos si es el primer nodo, en el caso de que lo sea sera cabecera
    if (listaRow->cabeza == NULL)
    {
        listaRow->cabeza = nodoRow;
    }
    else
    {
        // Aqui declaramos un puntero para recorrer los elementos
        ROW *puntero = listaRow->cabeza;

        while (puntero->sig != NULL)
        {
            puntero = puntero->sig;
        }
        // Una vez tengamos el elemento ponemos que el anterior del nodo es a lo que apunta ahora nuestra variale punter
        nodoRow->ant = puntero;
        // El siguiente de lo que apunta nuestro puntero es el nodo.
        puntero->sig = nodoRow;
        // Para garantizar que toda la lista queda "cerrada" lo siguiente del nodo es null.
        nodoRow->sig = NULL;
    }
    // Para llevar la cuenta de cuantos elementos se insertan.
    listaRow->filas++;
}

// Funcion para imprimir la lista
void ImprmirLista(Lista *lista)
{
    Nodo *puntero = lista->cabeza;
    while (puntero != NULL)
    {
        printf("Nombre: %s \n", puntero->nombre);
        printf("Numcampos: %i \n", puntero->numCampos);
        for (int i = 0; i < puntero->numCampos; i++)
        {
            printf("Campo numero %i: %s \n", i, puntero->campos[i]);
            printf("Tipo numero %i: %s \n", i, tipoAChar(puntero->tipos[i]));
        }

        puntero = puntero->sig;
        printf("\n");
    }
}

void ImprmirROWS(ListaRow *listaROW, int numCamposTAB)
{
    ROW *puntero = listaROW->cabeza;
    while (puntero != NULL)
    {
        for (int i = 0; i < numCamposTAB; i++)
        {
            printf("\n Valor numero %i: %s ", i, puntero->valores[i]);
        }
        puntero = puntero->sig;
        printf("\n");
    }
}

// Escribir el fichero metaDB
void EscribirMETA(Lista *lista)
{
    FILE *f;
    Nodo *puntero = lista->cabeza;
    int i;

    f = fopen("meta.db", "w+");
    fprintf(f, "tablas: %d\n", lista->longitud); // Imprimimos la primera linea
    while (puntero != NULL)
    {
        fprintf(f, "-------------------------\n");
        fprintf(f, "%s: %d\n", puntero->nombre, puntero->numCampos);
        for (i = 0; i < puntero->numCampos; i++)
        {
            fprintf(f, "%s: ", puntero->campos[i]);
            fprintf(f, "%s\n", tipoAChar(puntero->tipos[i]));
        }
        puntero = puntero->sig;
    }
    fclose(f);
}

void EscribirTAB(ListaRow *listaROW, int numCamposTAB, char *archivoTAB)
{
    FILE *f;
    ROW *puntero = listaROW->cabeza;
    int i;

    if (listaROW->cabeza != NULL)
    {
        while (puntero->sig != NULL)
        {
            puntero = puntero->sig;
        }
    }
    f = fopen(archivoTAB, "a+"); // En este caso es a ya que queremos añadir al final, no sobreescribir

    for (i = 0; i < numCamposTAB; i++)
    {
        if (controlarEspacios(puntero->valores[i]) == 1 && i == numCamposTAB - 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
        {
            fprintf(f, "\"%s\" \n", puntero->valores[i]);
        }
        else if (controlarEspacios(puntero->valores[i]) == 1 && i != numCamposTAB - 1) // cadena con comillas pero no esta al final por lo tanto se pone coma
        {
            fprintf(f, "\"%s\", ", puntero->valores[i]);
        }
        else if (i == numCamposTAB - 1) // De esta forma compruebo si es el ultimo parametro para hacer \n y no poner coma
        {
            fprintf(f, "%s \n", puntero->valores[i]);
        }
        else
        {
            fprintf(f, "%s, ", puntero->valores[i]); // Cadena sin comillas y con coma
        }
    }

    fclose(f);
}

void LeerMeta(Lista *lista, int *numTablas)
{
    FILE *f;
    Nodo *puntero = lista->cabeza;
    int numCampos, i, j;
    char cadena[100], nombre[100], *campos[100], *tipos[100];
    char separadores[10] = " :\n\t\r";

    f = fopen("meta.db", "r");

    fgets(cadena, 100, f);
    *numTablas = atoi(cadena + 8);

    if (*numTablas != 0)
    {
        for (i = 0; i < *numTablas; i++)
        {
            fgets(cadena, 100, f); // De esta forma lo que hago es coger la cadena de ------ y guardarla
            // de forma que si hago otro fgets esa cadena se la saltara
            fgets(cadena, 100, f);

            // Ahora ya tengo la primera linea de datos que es el nombre y el numero y haremos lo siguiente
            strcpy(nombre, strtok(cadena, separadores));
            numCampos = atoi(strtok(NULL, separadores));

            for (j = 0; j < numCampos; j++)
            {
                fgets(cadena, 100, f); // Pillamos la siguiente linea
                campos[j] = strdup(strtok(cadena, separadores));
                tipos[j] = strdup(strtok(NULL, separadores));
            }
            InsertarFinal(lista, nombre, numCampos, campos, tipos);
            for (j = 0; j < numCampos; j++)
            {
                free(campos[j]);
                free(tipos[j]);
            }
        }
    }
    fclose(f);
}

void LeerTAB(ListaRow *listaRow, int numCamposTAB, char *archivoTAB)
{
    FILE *f;
    ROW *puntero = listaRow->cabeza;
    char cadena[500], *valores[100], separadores[10] = " ,\n\r\0";
    int i, j;
    f = fopen(archivoTAB, "r");

    while (fgets(cadena, 500, f) != NULL)
    {
        valores[0] = strdup(MiStrTok(cadena, separadores, 1));
        for (i = 1; i < numCamposTAB; i++)
        {
            valores[i] = strdup(MiStrTok(NULL, separadores, 1));
        }
        InsertarFinalRow(listaRow, valores, numCamposTAB, 0);
        for (j = 0; j < numCamposTAB; j++)
        {
            free(valores[j]);
        }
    }
    fclose(f);
}

// Funcion que elimina de memoria lo que haya en el meta.db
void EliminarTab(Lista *lista)
{
    Nodo *puntero = lista->cabeza;

    while (puntero != NULL)
    {
        lista->cabeza = puntero->sig;

        for (int i = 0; i < puntero->numCampos; i++)
        {
            free(puntero->campos[i]);
        }
        free(puntero->campos);
        free(puntero->tipos);
        //free(puntero->nombre);
        //free(puntero->numCampos);
        free(puntero);
        lista->longitud--;
        puntero = lista->cabeza;
    }
}

// Falta por implementar
void EliminarRow(ListaRow *listaRow, int numCampos)
{
    ROW *puntero = listaRow->cabeza;

    while (puntero != NULL)
    {
        listaRow->cabeza = puntero->sig;
        for (int i = 0; i < numCampos; i++)
        {
            free(puntero->valores[i]);
        }
        free(puntero->valores);
        listaRow->filas--;
        puntero = listaRow->cabeza;
    }
}

// Para comprobar si una cadena es texto
int isText(char tmp[])
{
    int j = 0, isDigit;

    while (j < strlen(tmp))
    {
        if ((tmp[j] >= 'a' && tmp[j] <= 'z') || (tmp[j] >= 'A' && tmp[j] <= 'Z'))
        {
            j++;
            continue;
        }
        else
            return 1;
        j++;
    }
    return 0;
}

// Para comprobar si una cadena es texto
int isEmail(char tmp[])
{
    int j = 0, isDigit;
    int arroba=0, punto=0;

    while (j < strlen(tmp))
    {
        if ((tmp[j] >= 'a' && tmp[j] <= 'z') || (tmp[j] >= 'A' && tmp[j] <= 'Z'))
        {
            j++;
            continue;
        }
        else if (tmp[j] >= '0' && tmp[j] <= '9')
        {
            j++;
            continue;
        }
        else if (tmp[j] == '@')
        {
            arroba++;
            j++;
            continue;
        }
        else if (tmp[j] == '.')
        {
            punto++;
            j++;
            continue;
        }
        else
            return 1;
        j++;
    }
    if (arroba==0 || punto==0)
    {
        return 1;
    }
    
    return 0;
}

// Para comprobar si una cadena es numero
int isNumber(char tmp[])
{
    int j = 0;

    while (j < strlen(tmp))
    {
        if (tmp[j] >= '0' && tmp[j] <= '9')
        {
            j++;
            continue;
        }
        else if (tmp[j] == '.')
        {
            j++;
            continue;
        }
        else
            return 1;
    }
    return 0;
}

/**
 * Esta funcion se encarga de validar que una fecha es correcta.
 *
 * param cadena es la fecha que se le pasa por parametro.
 * return  0 si la fecha es correcta.
 * return 1 si la fecha es incorrecta.
 */
int valFecha(char *cadena)
{
    char *token, fecha[200];
    int bol = 1, dia, mes, anio;
    strcpy(fecha, cadena);
    dia = mes = anio = 0;

    token = strtok(fecha, "/");
    dia = atoi(token);
    token = strtok(NULL, "/");
    mes = atoi(token);
    token = strtok(NULL, "/");
    anio = atoi(token);

    if (dia == 0 || mes == 0 || anio == 0)
    {
        return 1;
    }

    if (mes >= 1 && mes <= 12)
    {

        switch (mes)
        {

        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (dia >= 1 && dia <= 31)
            {
                bol = 0;
            };
            break;

        case 4:
        case 6:
        case 9:
        case 11:
            if (dia >= 1 && dia <= 30)
            {
                bol = 0;
            };
            break;

        case 2:
            // Si el año es bisiesto +1 día.
            if ((anio % 4 == 0) && (anio % 100 != 0) || (anio % 400 == 0))
            {
                if (dia >= 1 && dia <= 29)
                {
                    bol = 0;
                };
            }
            else
            {
                if (dia >= 1 && dia <= 28)
                {
                    bol = 0;
                };
            };
            break;
        };
    };

    return bol;
}

//Funcion que imprime el nombre de los campos para cuando se haga un select 
//por ver los tipos de campos que son 
void camposMetaSelect(Nodo *nodo)
{
    printf("\n");
    for (int i = 0; i < nodo->numCampos; i++)
    {
        printf("%s   ", nodo->campos[i]);
    }
    printf("\n----------------------------------------------------");
}

//Funcion que imprime los valores de la tabla al escrbir select 
void ImprimirValoresSelect(Nodo *nodo, ListaRow *listaRow)
{

    ROW *punteroROW = listaRow->cabeza;

    while (punteroROW != NULL)
    {
        printf("\n");
        for (int j = 0; j < nodo->numCampos; j++)
        {
            if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
            {
                printf("\"%s\"   ", punteroROW->valores[j]);
            }
            else
            {
                printf("%s   ", punteroROW->valores[j]);
            }
        }
        printf("\n");
        punteroROW = punteroROW->sig;
    }
}

/**
 * El devolver pos es muy importante para poder trabajar con select, update, delete.....
 * ya que lo que hace es buscar el campo en concreto que se le pase 
 * y todas y cada una de las comparaciones que haga con el tab la hara con los valores
 * de ese campo en concreto, ya que si quiero comparar numeros de id tienen que ser los 
 * campos que esten en la poscion de id y no en el resto 
 */
int devolverPos(Nodo *nodoTab, char *campo)
{
    int pos;

    for (int i = 0; i < nodoTab->numCampos; i++)
    {
        if (strcmp(campo, nodoTab->campos[i]) == 0)
        {
            pos = i;
            break;
        }
    }
    return pos;
}

/**
 * Esta funcion comprueba si la fecha2 es mayor, menor o igual.
 * @return 0 si las fechas 2 y 1 son iguales.
 * @return 1 si la fecha2 es mayor que la 1.
 * @return 2 si la fecha2 es menor que la 1.
 */
int sortDates(char *date1, char *date2)
{
    char temp;
    int anio1, mes1, dia1, anio2, mes2, dia2;

    char fecha[200], fecha2[200], *token;
    strcpy(fecha, date1);
    strcpy(fecha2, date2);

    token = strtok(fecha, "/");
    dia1 = atoi(token);
    token = strtok(NULL, "/");
    mes1 = atoi(token);
    token = strtok(NULL, "/");
    anio1 = atoi(token);
    token = strtok(fecha2, "/");
    dia2 = atoi(token);
    token = strtok(NULL, "/");
    mes2 = atoi(token);
    token = strtok(NULL, "/");
    anio2 = atoi(token);

    if (anio2 < anio1)
    {
        return 2;
    }
    else if (anio2 == anio1)
    {
        if (mes2 < mes1)
        {
            return 2;
        }
        else if (mes2 == mes1)
        {
            if (dia2 < dia1)
            {
                return 2;
            }
            else if (dia2 == dia1)
            {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * Esta funcion lo que hace es ordenar la lista de los tab de forma ascendetnte o descendente 
 * 
 */
void ordenarAscDesc(ListaRow *listaRow, Nodo *nodoTab, char *orden, int numCampos, int pos)
{
    ROW *punteroi;
    ROW *punteroj;
    char **aux;
    if (strcmp(orden, "DES") == 0)
    {
        if (nodoTab->tipos[pos] == NUM)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (atof(punteroi->valores[pos]) > atof(punteroj->valores[pos])) // Si el numero 1 es mayor al numero2 se cambia
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == DATE)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (sortDates(punteroi->valores[pos], punteroj->valores[pos]) == 2) // Si la fecha1 es mayor que la fecha 2 se cambia
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == TEXT)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (strcmp(punteroi->valores[pos], punteroj->valores[pos]) > 0)
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == EMAIL)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (strcmp(punteroi->valores[pos], punteroj->valores[pos]) > 0)
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
    }
    else if (strcmp(orden, "ASC") == 0)
    {
        if (nodoTab->tipos[pos] == NUM)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (atof(punteroi->valores[pos]) < atof(punteroj->valores[pos])) // Si el numero 1 es mayor al numero2 se cambia
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == DATE)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (sortDates(punteroi->valores[pos], punteroj->valores[pos]) == 1) // Si la fecha1 es mayor que la fecha 2 se cambia
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == TEXT)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (strcmp(punteroi->valores[pos], punteroj->valores[pos]) < 0)
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
        else if (nodoTab->tipos[pos] == EMAIL)
        {
            for (punteroi = listaRow->cabeza; punteroi != NULL; punteroi = punteroi = punteroi->sig)
            {
                for (punteroj = listaRow->cabeza; punteroj != NULL; punteroj = punteroj = punteroj->sig)
                {
                    if (strcmp(punteroi->valores[pos], punteroj->valores[pos]) < 0)
                    {
                        aux = punteroi->valores;
                        punteroi->valores = punteroj->valores;
                        punteroj->valores = aux;
                    }
                }
            }
        }
    }
}


/**
 * Esta funcion lo que hace es imprimir con el select =, > o < 
 * En el caso de que se indique ASC o DESC se llama a la funcion anterior para ordenarlo 
 * 
 */
void selectOperadorOrden(ListaRow *listaRow, TYPE tipo, int operador, char *campoAbuscar, char *campo, Nodo *nodoTab, char *orden)
{
    ROW *punteroROW = listaRow->cabeza;
    int pos = 0;

    pos = devolverPos(nodoTab, campo);

    if (orden!=NULL)
    {
        ordenarAscDesc(listaRow, nodoTab, orden, nodoTab->numCampos, pos);
    }
    if (operador == 61) // =
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) == 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                // devolver pos
                if (num == atof(punteroROW->valores[pos]))
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }
                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                if (sortDates(campoAbuscar, punteroROW->valores[pos]) == 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) == 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
    }
    else if (operador == 62) // >
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                // devolver pos
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) > 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                // devolver pos
                if (num < atof(punteroROW->valores[pos]))
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                if (sortDates(campoAbuscar, punteroROW->valores[pos]) == 1)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                // devolver pos
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) > 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
    }
    else if (operador == 60) // <
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                // devolver pos
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) < 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }
                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                // devolver pos
                if (num > atof(punteroROW->valores[pos]))
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                if (sortDates(campoAbuscar, punteroROW->valores[pos]) == 2)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }

                punteroROW = punteroROW->sig;
            }
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                // devolver pos
                if (strcmp(campoAbuscar, punteroROW->valores[pos]) < 0)
                {
                    printf("\n");
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        if (controlarEspacios(punteroROW->valores[j]) == 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
                        {
                            printf("\"%s\"   ", punteroROW->valores[j]);
                        }
                        else
                        {
                            printf("%s   ", punteroROW->valores[j]);
                        }
                    }
                    printf("\n");
                }
                punteroROW = punteroROW->sig;
            }
        }
    }
}

/**
 * Esta funcion sirve para actualizar el tab despues de haber borrado los campos que se especificaban en el delete 
 * 
 */
void EscribirTABdelete(ListaRow *listaROW, int numCamposTAB, char *archivoTAB)
{
    FILE *f;
    ROW *puntero = listaROW->cabeza;
    int i;

    f = fopen(archivoTAB, "w+"); // En este caso lo que queremos es crear uno desde 0

    while (puntero != NULL)
    {
        for (i = 0; i < numCamposTAB; i++)
        {
            if (controlarEspacios(puntero->valores[i]) == 1 && i == numCamposTAB - 1) // Este caso es igual que el anterior pero en el caso de que sea cadena con comillas
            {
                fprintf(f, "\"%s\" \n", puntero->valores[i]);
            }
            else if (controlarEspacios(puntero->valores[i]) == 1 && i != numCamposTAB - 1) // cadena con comillas pero no esta al final por lo tanto se pone coma
            {
                fprintf(f, "\"%s\",  ", puntero->valores[i]);
            }
            else if (i == numCamposTAB - 1) // De esta forma compruebo si es el ultimo parametro para hacer \n y no poner coma
            {
                fprintf(f, "%s \n", puntero->valores[i]);
            }
            else
            {
                fprintf(f, "%s, ", puntero->valores[i]); // Cadena sin comillas y con coma
            }
        }
        puntero = puntero->sig;
    }

    fclose(f);
}

/**
 * Esta funcion lo que hace es recibir los mismos paramtros que el select =, >, < 
 * Pero en este caso se pone delete y elimina las filas que cumplen dicha condicion 
 * Lo que hace es reorganizar la lista, eliminar los datos de memoria y llamar a la funcion
 * anterior para actualizar el tab 
 * 
 */
void deleteOperador(ListaRow *listaRow, TYPE tipo, int operador, char *campoAbuscar, char *campo, Nodo *nodoTab, char *archivoTAB)
{
    ROW *punteroROW = listaRow->cabeza;
    ROW *ant;
    ROW *sig;
    ROW *puntero;
    int pos = 0;

    pos = devolverPos(nodoTab, campo);

    if (operador == 61) // =
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) == 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (num == atof(puntero->valores[pos]))
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (sortDates(campoAbuscar, puntero->valores[pos]) == 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) == 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
    }
    else if (operador == 62) // >
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) > 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (num < atof(puntero->valores[pos]))
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        sig = puntero->sig;
                        puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                } 
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (sortDates(campoAbuscar, puntero->valores[pos]) == 1)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) > 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
    }
    else if (operador == 60) // <
    {
        if (tipo == TEXT)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) < 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == NUM)
        {
            float num = atof(campoAbuscar);
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (num > atof(puntero->valores[pos]))
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == DATE)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (sortDates(campoAbuscar, puntero->valores[pos]) == 2)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }

                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
        else if (tipo == EMAIL)
        {
            while (punteroROW != NULL)
            {
                puntero = punteroROW;
                punteroROW = punteroROW->sig;
                if (strcmp(campoAbuscar, puntero->valores[pos]) < 0)
                {
                    if (puntero == listaRow->cabeza)
                    {
                        listaRow->cabeza = puntero->sig;
                        if (puntero->sig != NULL)
                        {
                            puntero->sig->ant = NULL;
                        }
                    }
                    else if (puntero->sig == NULL)
                    {
                        ant = puntero->ant;
                        puntero->ant = NULL;
                        ant->sig = NULL;
                    }
                    else
                    {
                        ant = puntero->ant;
                        // puntero->ant = NULL;
                        sig = puntero->sig;
                        // puntero->sig = NULL;
                        ant->sig = sig;
                        sig->ant = ant;
                    }
                    for (int j = 0; j < nodoTab->numCampos; j++)
                    {
                        free(puntero->valores[j]);
                    }
                    listaRow->filas--;
                }
            }
            EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
        }
    }
}



void funcOrder(ListaRow *listaRow, Nodo *nodoTab, char *campoAbuscar, char *orden, char *archivoTAB)
{
    ROW *punteroROW = listaRow->cabeza;
    int pos = 0;
    pos=devolverPos(nodoTab, campoAbuscar);
    if (strcmp(orden, "ASC")==0)
    {
        ordenarAscDesc(listaRow, nodoTab, orden, nodoTab->numCampos, pos);
        EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
    }
    else if (strcmp(orden, "DES")==0)
    {
        ordenarAscDesc(listaRow, nodoTab, orden, nodoTab->numCampos, pos);
        EscribirTABdelete(listaRow, nodoTab->numCampos, archivoTAB);
    }
    else
    {
        printf("ERROR: campos incorrectos \n");
    }
}


void actualizarMeta(Lista *lista, char* nombreCampo)
{
    Nodo *punteroNODO = lista->cabeza;
    Nodo *ant;
    Nodo *sig;
    Nodo *puntero;

    while (punteroNODO != NULL)
    {
        puntero = punteroNODO;
        punteroNODO = punteroNODO->sig;
        if (strcmp(nombreCampo, puntero->nombre)==0)
        {
            if (puntero == lista->cabeza)
            {
                lista->cabeza = puntero->sig;
                if (puntero->sig != NULL)
                {
                    puntero->sig->ant = NULL;
                }
            }
            else if (puntero->sig == NULL)
            {
                ant = puntero->ant;
                puntero->ant = NULL;
                ant->sig = NULL;
            }
            else
            {
                ant = puntero->ant;
                // puntero->ant = NULL;
                sig = puntero->sig;
                // puntero->sig = NULL;
                ant->sig = sig;
                sig->ant = ant;
            }
            for (int i = 0; i < puntero->numCampos; i++)
            {
                free(puntero->campos[i]);
            }
            free(puntero->campos);
            free(puntero->tipos);
            //free(puntero->nombre);
            //free(puntero->numCampos);
            lista->longitud--;       
        }    
    }
    EscribirMETA(lista);
}

