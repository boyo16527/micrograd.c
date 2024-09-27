#include <stdio.h>

// Our scalar node
struct Value {
  float data;
};

void repr(struct Value* self) {
  printf("Value(data=%.4f)\n", self->data);
}

struct Value initialize_value(float data) {
  struct Value v = {data};
  return v;
}

int main() {
  struct Value a = initialize_value(4.0);
  repr(&a);

  return 0;
}

