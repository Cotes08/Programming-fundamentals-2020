

typedef enum { NUM, DATE, TEXT, EMAIL} TYPE;

typedef struct Nodo
{
    char nombre[100];
    int numCampos;
    char **campos;
    TYPE *tipos;
    struct Nodo *sig, *ant;
}Nodo;

typedef struct ROW
{
    char **valores;
    struct ROW *sig, *ant;
}ROW;

typedef struct Lista
{
    Nodo* cabeza;
    int longitud;
} Lista;

typedef struct ListaRow
{
    ROW* cabeza;
    int filas;
} ListaRow;



//Gestion de parametros
char* MiStrTok(char *cad1, char *sep, int comillas);
//Validar tipo TEXT DATE NUM y EMAIL
int isText(char tmp[]);
int isNumber(char tmp[]);
int valFecha(char *cadena);
int controlarEspacios(char *tmp);
int isEmail(char tmp[]);
int sortDates(char *date1, char *date2);
//////////////////////////////////////////////////
/*       Gestion de ficheros y archivos         */
//////////////////////////////////////////////////
int dirExists(const char *path);
int buscarDuplicados(char** campos, int numCampos);
void EscribirMETA(Lista* lista);
void LeerMeta(Lista *lista, int *numTablas);
void EscribirTAB(ListaRow* listaROW, int numCamposTAB, char* archivoTAB);
void LeerTAB(ListaRow *listaRow, int numCamposTAB, char *archivoTAB);
void funcOrder(ListaRow *listaRow, Nodo *nodoTab, char *campoAbuscar, char *orden, char *archivoTAB);
void actualizarMeta(Lista *lista, char* nombreCampo);

//////////////////////////////////////////////////
/*       Gestion de la lista y los nodos        */
//////////////////////////////////////////////////
Lista* listaIncial();
ListaRow* listaIncialR();
Nodo* CrearNodo(char *nombre, int numCampos, char **campos, char **tipos);
ROW* CrearNodoRow(char **valores, int numCampos, int sum);
void InsertarFinal(Lista* lista, char *nombre, int numCampos, char **campos, char **tipos);
void InsertarFinalRow(ListaRow* listaRow, char** valores, int numCampos, int sum);
Nodo *buscarTabla(Lista *lista, char *nombreTAB);
char *tipoAChar(TYPE tipo);
TYPE detectarTipo(char *token);
void ImprmirLista(Lista* lista);
void ImprmirROWS(ListaRow* listaROW, int numCamposTAB);
void EliminarTab(Lista* lista);
void EliminarRow(ListaRow* listaRow, int numCampos);
void camposMetaSelect(Nodo* nodo);
void ImprimirValoresSelect(Nodo* nodo, ListaRow* listaRow);
int seleccionarTAB(char*archivoTAB);
int devolverPos(Nodo*nodoTab, char* campoAbuscar);
void ordenarAscDesc(ListaRow* listaRow, Nodo* nodoTab, char* orden, int numCampos, int pos);
void selectOperadorOrden(ListaRow* listaRow, TYPE tipo, int operador, char* campoAbuscar, char* campo ,Nodo* nodoTab, char* orden);
void deleteOperador(ListaRow* listaRow, TYPE tipo, int operador, char* campoAbuscar, char* campo ,Nodo* nodoTab, char* archivoTAB);
void EscribirTABdelete(ListaRow* listaROW, int numCamposTAB, char* archivoTAB);



