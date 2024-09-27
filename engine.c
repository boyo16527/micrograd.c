#include <stdio.h>
#include <stdlib.h>

// Our scalar node
struct Value {
  float data;
  char op;
  struct Value** children; // Point to an array of Value pointers
  int num_children;
};

void repr(struct Value* self) {
  printf("Value(data=%.4f, op = %c, children=[", self->data, self->op);
  for (int i = 0; i < self->num_children; i++) {
    if (i > 0) { printf (", "); }

    repr(self->children[i]);
  }
  printf("])\n");
}

void add_child(struct Value* self, struct Value* child) {
  self->children[self->num_children] = child;
  self->num_children++;
}

struct Value* initialize_value(float data) {
  struct Value* v = malloc(sizeof(struct Value));
  v->data = data;
  v->children = malloc(sizeof(struct Value*) * 2); 
  v->op = '\0';
  v->num_children = 0;
  return v;
}

struct Value* add(struct Value* left, struct Value* right) {
  struct Value* sum = initialize_value(left->data + right->data);
  sum->op = '+';
  add_child(sum, left);
  add_child(sum, right);
  return sum;
}

struct Value* mul(struct Value* left, struct Value* right) {
  struct Value* product = initialize_value(left->data * right->data);
  product->op = '*';
  add_child(product, left);
  add_child(product, right);
  return product;
}


int main() {
  struct Value* a = initialize_value(4.0);
  struct Value* b = initialize_value(2.0);

  repr(a);
  repr(b);

  struct Value* c = add(a, b);
  repr(c);
  struct Value* d = initialize_value(-3.0);
  repr(d);
  struct Value* e = mul(c, d);
  repr(e);

  return 0;
}

