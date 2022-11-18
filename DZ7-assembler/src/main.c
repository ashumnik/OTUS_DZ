#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int64_t data[] = {4, 8, 15, 16, 23, 42};
int64_t data_length = sizeof(data) / sizeof(int64_t);

struct Node
{
	int64_t a;
	struct Node *next;
};

void print_int(int64_t *a)
{
	printf("%ld ", *a);
}

int64_t p(int64_t *a) 
{
    return *a & 1;
}

struct Node *add_element(int64_t a, struct Node *n)
{
	struct Node *new_Node = malloc(sizeof(struct Node));
	if (new_Node == NULL)
		abort();
	
	new_Node->a = a;
	new_Node->next = n;
	
	return new_Node;
}

void m(struct Node* n, void(*func)(int64_t*)) {
    if (n == NULL) {
        return;
    }
    (*func)(&n->a);
    m(n->next, func);
}

struct Node* f(struct Node* n, struct Node* odd_list, int64_t(*pred)(int64_t*)) {
    if (n == NULL) {
        return odd_list;
    }
    if ((*pred)(&n->a)) {
        odd_list = add_element(n->a, odd_list);
    }
    return f(n->next, odd_list, pred);
}

int main() {
    struct Node* ptr = NULL;
	struct Node* ptr2 = NULL;
    int64_t i = data_length;
	
    while (i--) {
        int64_t a = data[i];
        ptr = add_element(a, ptr);
    }

    m(ptr, print_int);
    putc('\n', stdout);

    ptr2 = f(ptr, ptr2, p);

    m(ptr2, print_int);
    putc('\n', stdout);

    return 0;
}
