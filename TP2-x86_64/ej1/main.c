#include "ej1.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>


/**
 * Wrappers para las funciones de la implementación en C o en ensamblador
*/

string_proc_list *create_list(){
	return USE_ASM_IMPL
		? string_proc_list_create_asm()
		: string_proc_list_create();
}

string_proc_node *create_node(uint8_t type, char* hash){
	return USE_ASM_IMPL
		? string_proc_node_create_asm(type, hash)
		: string_proc_node_create(type, hash);
}

void add_node(string_proc_list * list, uint8_t type, char* hash){
	USE_ASM_IMPL
		? string_proc_list_add_node_asm(list, type, hash)
		: string_proc_list_add_node(list, type, hash);
}

char* concat_hash(string_proc_list * list, uint8_t type, char* hash){
	return USE_ASM_IMPL
		? string_proc_list_concat_asm(list, type, hash)
		: string_proc_list_concat(list, type, hash);
}


/**
*	crea y destruye a una lista vacía
*/
bool test_create_destroy_list(){
	string_proc_list * list = create_list();
	bool ok = (list->first == NULL && list->last == NULL);
	printf("Emptylist test: %s\n", ok ? "Passed" : "Failed");
	string_proc_list_destroy(list);
	return ok;
}

/**
*	crea y destruye un nodo
*/

bool test_create_destroy_node(){
	string_proc_node* node = create_node(0, "hash");
	bool ok = (node->next == NULL && node->previous == NULL && strcmp(node->hash, "hash") == 0 && node->type == 0);
	printf("New node test: %s\n", ok ? "Passed" : "Failed");
	string_proc_node_destroy(node);
	return ok;
}

/**
 * 	crea una lista y le agrega nodos
*/
bool test_create_list_add_nodes(){
    string_proc_list * list = create_list();
    add_node(list, 0, "hola");
    add_node(list, 0, "a");
    add_node(list, 0, "todos!");

    // Ver si existen los nodos
    string_proc_node* n1 = list->first;
    string_proc_node* n2 = n1 ? n1->next : NULL;
    string_proc_node* n3 = n2 ? n2->next : NULL;

    bool ok = 1;
    ok = ok && n1 && n2 && n3;
	// Verificar que los nodos tengan los valores correctos
	ok = ok && (n1->type == 0 && strcmp(n1->hash, "hola") == 0);
    ok = ok && (n2->type == 0 && strcmp(n2->hash, "a") == 0);
    ok = ok && (n3->type == 0 && strcmp(n3->hash, "todos!") == 0);
    ok = ok && (n3->next == NULL);
    ok = ok && (list->last == n3);

    printf("Add nodes test: %s\n", ok ? "Passed" : "Failed");

    string_proc_list_destroy(list);

	return ok;
}

/**
 * 	crea una lista y le agrega nodos. Luego aplica la lista a un hash.
*/

bool test_list_concat_basic(){
	string_proc_list * list	= create_list();
	add_node(list, 0, "hola");
	add_node(list, 0, " a");
	add_node(list, 0, " todos!");

	char* new_hash = concat_hash(list, 0, "hash:");
	bool ok = strcmp(new_hash, "hash:hola a todos!") == 0;
	printf("Concat basic test: %s\n", ok ? "Passed" : "Failed");

	string_proc_list_destroy(list);
	free(new_hash);
	return ok;
}

/**
 * Tests propios
 */

bool test_list_concat_selective(){
	string_proc_list * list = create_list();

	add_node(list, 0, "Hello");
	add_node(list, 1, "This");
	add_node(list, 0, " World");
	add_node(list, 2, "Should");
	add_node(list, 1, " is");
	add_node(list, 0, "!");
	add_node(list, 1, " great");
	add_node(list, 2, " not");
	add_node(list, 4, "");

	char* type0_result = concat_hash(list, 0, "Type0: ");
	bool ok1 = strcmp(type0_result, "Type0: Hello World!") == 0;

	char* type1_result = concat_hash(list, 1, "Type1: ");
	bool ok2 = strcmp(type1_result, "Type1: This is great") == 0;

	char* type2_result = concat_hash(list, 2, "Type2: ");
	bool ok3 = strcmp(type2_result, "Type2: Should not") == 0;

	char* type3_result = concat_hash(list, 3, "Type3: "); // No hay nodos de tipo 3
	bool ok4 = strcmp(type3_result, "Type3: ") == 0;

	char* type4_result = concat_hash(list, 4, "Type4: "); // Nodo de tipo 4 vacío
	bool ok5 = strcmp(type4_result, "Type4: ") == 0;

	char* type0_result_empty = concat_hash(list, 0, ""); // Concatenar con un hash vacío
	bool ok6 = strcmp(type0_result_empty, "Hello World!") == 0;

	char* empty_result = concat_hash(list, 4, ""); // Concatenar con un hash vacío y un nodo con hash vacío
	bool ok7 = strcmp(empty_result, "") == 0; // Debería devolver una cadena vacía

	// Resultados
	bool ok = ok1 && ok2 && ok3 && ok4 && ok5 && ok6 && ok7;
	printf("Selective concat test: %s\n", ok ? "Passed" : "Failed");
	if (!ok1) { printf("Failed, Type0 result: %s\n", type0_result); }
	if (!ok2) { printf("Failed, Type1 result: %s\n", type1_result); }
	if (!ok3) { printf("Failed, Type2 result: %s\n", type2_result); }
	if (!ok4) { printf("Failed, Type3 result: %s\n", type3_result); }
	if (!ok5) { printf("Failed, Type4 result: %s\n", type4_result); }
	if (!ok6) { printf("Failed, Type0 empty result: %s\n", type0_result_empty); }
	if (!ok7) { printf("Failed, empty result: %s\n", empty_result); }

	string_proc_list_destroy(list);
	free(type0_result);
	free(type1_result);
	free(type2_result);
	free(type3_result);
	free(type4_result);
	free(type0_result_empty);
	free(empty_result);
	return ok;
}

bool test_null_inputs(){
	string_proc_list * empty_list = create_list();
	string_proc_list * list = create_list();

	// Tests add_node
	add_node(NULL, 0, "hola"); // No deberia explotar ni pasar nada

	add_node(list, 0, NULL); // Deberia agregar el nodo
	add_node(list, 0, "hola");
	add_node(list, 1, "hola");

	bool ok1 = (list->first != NULL);
	bool ok2 = (list->first->hash == NULL); // El hash deberia ser NULL
	bool ok3 = (list->first->type == 0);

	// Test concat
	char* result_hash = concat_hash(NULL, 0, "hash:");
	bool ok4 = (result_hash == NULL); // Concatenar con lista NULL deberia devolver NULL

	result_hash = concat_hash(list, 1, NULL);
	bool ok5 = (result_hash == NULL); // Concatenar con hash NULL (input) deberia devolver NULL

	result_hash = concat_hash(list, 0, "hash:");
	bool ok6 = (result_hash == NULL); // Concatenar con hash NULL (en algun nodo) deberia devolver NULL

	result_hash = concat_hash(empty_list, 0, "hash:");
		bool ok7 = strcmp(result_hash, "hash:") == 0; // Concatenar con lista vacia deberia devolver una copia del hash original

	// Resultados
	bool ok = ok1 && ok2 && ok3 && ok4 && ok5 && ok6 && ok7;
	if (!ok1) { printf("Failed add_node NULL hash, list should have node\n"); }
	if (!ok2) { printf("Failed add_node NULL hash, node should have NULL hash\n"); }
	if (!ok3) { printf("Failed add_node NULL hash, node should have type 0\n"); }
	if (!ok4) { printf("Failed concat with NULL list, should return NULL\n"); }
	if (!ok5) { printf("Failed concat with NULL hash (at input), should return NULL\n"); }
	if (!ok6) { printf("Failed concat with NULL hash (in node), should return NULL\n"); }
	if (!ok7) { printf("Failed concat with empty list, should return original hash\n"); }

	string_proc_list_destroy(list);
	string_proc_list_destroy(empty_list);
	free(result_hash);

	printf("Null inputs test: %s\n", ok ? "Passed" : "Failed");
	return ok;
}

/**
* Corre los test a se escritos por los alumnos
*/
void run_tests(){
	bool ok = 1;
	ok = ok && test_create_destroy_list();
	ok = ok && test_create_destroy_node();
	ok = ok && test_create_list_add_nodes();
	ok = ok && test_list_concat_basic();
	ok = ok && test_list_concat_selective();
	ok = ok && test_null_inputs();

	printf("All tests: %s\n", ok ? "Passed✅" : "Failed❌");
}

int main (void){
	run_tests();
	return 0;
}

