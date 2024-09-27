#include <stdio.h>
#include <stdlib.h>

// Our scalar node
struct Value {
  float data;
  struct Value** children; // Point to an array of Value pointers
  int num_children;
};

void repr(struct Value* self) {
  printf("Value(data=%.4f, children=[", self->data);
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
  v->num_children = 0;
  return v;
}

struct Value* add(struct Value* left, struct Value* right) {
  struct Value* sum = initialize_value(left->data + right->data);
  add_child(sum, left);
  add_child(sum, right);
  return sum;
}


int main() {
  struct Value* a = initialize_value(4.0);
  struct Value* b = initialize_value(2.0);

  repr(a);
  repr(b);

  struct Value* c = add(a, b);
  repr(c);

  return 0;
}

