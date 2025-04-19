#include "ej1.h"

string_proc_list* string_proc_list_create(void){
	string_proc_list* list = (string_proc_list*)malloc(sizeof(string_proc_list));
	check_malloc(list, "Error: No se pudo crear la lista");
	list->first = NULL;
	list->last  = NULL;
	return list;
}

string_proc_node* string_proc_node_create(uint8_t type, char* hash){
	string_proc_node* node = (string_proc_node*)malloc(sizeof(string_proc_node));
	check_malloc(node, "Error: No se pudo crear el nodo");
	node->next      = NULL;
	node->previous  = NULL;
	node->type      = type;
	node->hash      = hash;
	return node;
}

void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
	if (list == NULL){ return; }

	string_proc_node* node = string_proc_node_create(type, hash);
	if (list->first == NULL){ // Si la lista está vacía
		list->first = node;
		list->last  = node;
	} else {
		node->previous = list->last;
		list->last->next = node;
		list->last = node;
	}
}

char* string_proc_list_concat(string_proc_list* list, uint8_t type , char* hash){
// 	Genera un nuevo hash concatenando el pasado por parámetro con todos los hashes
// de los nodos de la lista cuyos tipos coinciden con el pasado por parámetro
	if (list == NULL || hash == NULL) { return NULL; }

	// Si la lista está vacía, retorno el hash original
	if(list->first == NULL){
		char* result = (char*)malloc(strlen(hash) + 1);
		check_malloc(result, "Error: No se pudo crear el nuevo hash");
		strcpy(result, hash);
		return result;
	}

	char* new_hash = (char*)malloc(1);
	check_malloc(new_hash, "Error: No se pudo crear el nuevo hash");
	new_hash[0] = '\0';

	// Inicializo el nuevo hash con una cadena vacía
	string_proc_node* current_node = list->first;
	while(current_node != NULL){
		if(current_node->type == type){
			if (current_node->hash == NULL) { // Si un hash es NULL, retorno NULL
				free(new_hash);
				return NULL;
			}
			char* temp = str_concat(new_hash, current_node->hash);
			free(new_hash);
			new_hash = temp;

		}
		current_node = current_node->next;
	}
	char* result = str_concat(hash, new_hash);
	free(new_hash);
	return result;
}


/** AUX FUNCTIONS **/

void string_proc_list_destroy(string_proc_list* list){
	if (list == NULL){ return; }

	/* borro los nodos: */
	string_proc_node* current_node	= list->first;
	string_proc_node* next_node		= NULL;
	while(current_node != NULL){
		next_node = current_node->next;
		string_proc_node_destroy(current_node);
		current_node	= next_node;
	}
	/*borro la lista:*/
	list->first = NULL;
	list->last  = NULL;
	free(list);
}

void string_proc_node_destroy(string_proc_node* node){
	if (node == NULL){ return; }
	node->next      = NULL;
	node->previous	= NULL;
	node->hash		= NULL;
	node->type      = 0;
	free(node);
}


char* str_concat(char* a, char* b) {
	int len1 = strlen(a);
    int len2 = strlen(b);

	int totalLength = len1 + len2;
    char *result = (char *)malloc(totalLength + 1);
	check_malloc(result, "Error: No se pudo crear la cadena concatenada");
    strcpy(result, a);
    strcat(result, b);
    return result;
}

void string_proc_list_print(string_proc_list* list, FILE* file){
        uint32_t length = 0;
        string_proc_node* current_node = list->first;
        while(current_node != NULL){
			length++;
			current_node = current_node->next;
        }
        fprintf( file, "List length: %d\n", length );
		current_node    = list->first;
        while(current_node != NULL){
			fprintf(file, "\tnode hash: %s | type: %d\n", current_node->hash, current_node->type);
			current_node = current_node->next;
        }
}

void check_malloc(void* ptr, char* error_msg){
	if (ptr == NULL) {
		fprintf(stderr, "%s\n", error_msg);
		exit(1);
	}
}