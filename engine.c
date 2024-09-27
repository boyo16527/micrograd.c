#include <stdio.h>
#include <stdlib.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

// Our scalar node
struct Value {
  float data;
  float grad;
  char label;
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
  v->label = '\0';
  v->children = malloc(sizeof(struct Value*) * 2); 
  v->op = '\0';
  v->num_children = 0;
  v->grad = 0.0;
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

void set_child_grad(struct Value* parent) {
  for (int i = 0; i < parent->num_children; i++) {
    struct Value* child = parent->children[i];

    float local_derivative = 1.0;
    if (parent->op == '*') {
      for (int j = 0; j < parent->num_children; j++) {
        if (j != i) {  
          local_derivative *= parent->children[j]->data;   
        } 
      }
    }
    if (parent->op == '+') {
      local_derivative = 1.0;
    }

    child->grad += local_derivative * parent->grad;
    set_child_grad(child);
  }
  return;
}

void backprop(struct Value* root) {
  root->grad = 1.0;
  set_child_grad(root);
}

void add_node_and_edges(struct Value* node, Agraph_t* g) {
  char node_name[40]; 
  snprintf(node_name, sizeof(node_name), "label: %c | data: %.2f | grad: %.2f", node->label, node->data, node->grad);

  Agnode_t* gv_node = agnode(g, node_name, 1);

  Agnode_t** gv_nodes = malloc(sizeof(Agnode_t*) * node->num_children);
  int num_gv_nodes = 0;

  for (int i = 0; i < node->num_children; i++) {
    add_node_and_edges(node->children[i], g);
    char child_name[40];
    snprintf(child_name, sizeof(child_name), "label: %c | data: %.2f | grad: %.2f", node->children[i]->label, node->children[i]->data, node->children[i]->grad);

    Agnode_t* child_gv_node = agnode(g, child_name, 0);
    gv_nodes[num_gv_nodes] = child_gv_node;
    num_gv_nodes++;
  }

  char op_name[20];
  // want op node to be unique
  snprintf(op_name, sizeof(op_name), "%c_%p", node->op, (void*)node);

  Agnode_t* op_node = agnode(g, op_name, 1);
  agedge(g, gv_node, op_node, NULL, 1);

  for (int i = 0; i < num_gv_nodes; i++) {
    agedge(g, op_node, gv_nodes[i], NULL, 1);
  }

  free(gv_nodes);
}

void visualize_graph(struct Value* root) {
  GVC_t* gvc = gvContext();
  Agraph_t* g = agopen("G", Agdirected, NULL);

  add_node_and_edges(root, g);

  gvLayout(gvc, g, "dot");
  gvRenderFilename(gvc, g, "png", "output.png");
  gvFreeLayout(gvc, g);

  agclose(g);
  gvFreeContext(gvc);
}


void free_value(struct Value* v) {
  if (v == NULL) return; 

  for (int i = 0; i < v->num_children; i++){
    free_value(v->children[i]);
  }

  free(v->children);
  free(v);
}

int main() {
  struct Value* a = initialize_value(2.0);
  a->label = 'a';
  struct Value* b = initialize_value(-3.0);
  b->label = 'b';
  struct Value* c = initialize_value(10.0);
  c->label = 'c';
  struct Value* e = mul(a, b);
  e->label = 'e';
  struct Value* d = add(e, c);
  d->label = 'd';
  struct Value* f = initialize_value(-2.0);
  f->label = 'f';

  struct Value* L = mul(d, f);
  L->label = 'L';

  backprop(L);

  visualize_graph(L);

  free_value(L);

  return 0;
}

