#include "addressbook.h"

address_book_t* create_address_book() {
	address_book_t* book = (address_book_t *) malloc(sizeof(address_book_t));
	book->head = NULL;
	return book; 
}

contact_t* create_contact(char* first_name, char* last_name, int age, char* email) {
	contact_t* new_contact = (contact_t *) malloc(sizeof(contact_t));

    strcpy(new_contact->first_name, first_name);
    // new_contact->first_name[sizeof(new_contact->first_name) - 1] = '\0'; // Ensure null-termination
    strcpy(new_contact->last_name, last_name);
    // new_contact->last_name[sizeof(new_contact->last_name) - 1] = '\0'; // Ensure null-termination
    new_contact->age = age;
    strcpy(new_contact->email, email);
    // new_contact->email[sizeof(new_contact->email) - 1] = '\0'; // Ensure null-termination

	return new_contact;
}

void add_contact(address_book_t* address_book, contact_t* contact) {
	//allocating memory for the new node and initializing 
	node_t* new_node = (node_t *) malloc(sizeof(node_t));
	new_node->data = (void*) contact;
	new_node->next = NULL;

	//Initial case
	if (address_book->head == NULL){
		address_book->head = new_node;
		return;
	}

	node_t* tmp = address_book->head;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	} 
	tmp->next = new_node;
}

contact_t* find_contact(address_book_t* address_book, char* first_name, char* last_name) {
	return NULL;
}

void remove_contact(address_book_t* address_book, contact_t* contact) {
	//Invalid Parameters 
	if (contact == NULL || address_book == NULL){
		fprintf(stderr, "Error: NULL Pointer provided ");
	}
	//Conditions to exit early
	if (address_book->head == NULL){
		fprintf(stderr,"address book is empty. Removing contact not possible");
		return;
	}

	node_t* current = address_book->head->next;
	node_t* previous = NULL;
	if (current->data == (void*) contact){
		address_book->head = current->next;
		return;
	}
	while (current->data != (void *) contact){
		if(current->next == NULL){
			printf("contact not found in address book");
			return;
		}
		previous = current;
		current = current->next;
	}
	printf("contact found in address book");
	previous->next = current->next;
	free(current->data);
	free(current);
	printf("contact successfully deleted from address book");
}

void print_address_book(address_book_t* address_book) {
	node_t* current = address_book->head;
	while (current != NULL) {
		contact_t* contact = (contact_t*)current->data;
		printf("%s %s, %d, %s\n", contact->first_name, contact->last_name, contact->age,
		       contact->email);
		current = current->next;
	}
}

contact_t* duplicate_contact(contact_t* contact) {
	if (contact == NULL) return NULL;
	
	return create_contact(contact->first_name,contact->last_name,contact->email);
}

address_book_t* filter_address_book(address_book_t* address_book, bool (*filter)(contact_t*)) {
	return NULL;
}

void sort_address_book(address_book_t* address_book, bool (*compare)(contact_t*, contact_t*)) {}

bool compare_by_name(contact_t* contact1, contact_t* contact2) {
	if( strcmp(contact1->first_name, contact2->first_name) != 0) return false;

	if (strcmp(contact1->last_name, contact2->last_name ) != 0) return false;

	if (contact1->age != contact2->age) return false;

	if (strcmp(contact1->email, contact2->email) != 0) return false;

	return true;
}

bool is_adult(contact_t* contact) {
	return contact->age > 17;
}

size_t count_contacts(address_book_t* address_book) {
	if(address_book == NULL || address_book->head == NULL) return 0;	

	node_t* current = address_book->head;
	size_t count = 1;
	while(current->next != NULL) {
		count += 1;
		current = current->next;
	}
	return count;
}

void free_address_book(address_book_t* address_book) {
	if(address_book == NULL) return;

	
	node_t *current = address_book->head;
	while(current != NULL) {
		current = current->next;
	}

	free(address_book);
}