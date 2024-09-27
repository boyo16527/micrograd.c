#include <stdio.h>
#include <stdlib.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

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

void add_node_and_edges(struct Value* node, Agraph_t* g) {
  char node_name[20]; 
  snprintf(node_name, sizeof(node_name), "%.4f", node->data);

  Agnode_t* gv_node = agnode(g, node_name, 1);

  Agnode_t** gv_nodes = malloc(sizeof(Agnode_t*) * node->num_children);
  int num_gv_nodes = 0;

  for (int i = 0; i < node->num_children; i++) {
    add_node_and_edges(node->children[i], g);
    char child_name[20];
    snprintf(child_name, sizeof(child_name), "%.4f", node->children[i]->data);

    Agnode_t* child_gv_node = agnode(g, child_name, 0);
    gv_nodes[num_gv_nodes] = child_gv_node;
    num_gv_nodes++;
  }

  char op_name[3];
  snprintf(op_name, sizeof(op_name), "%c", node->op);

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


int main() {
  struct Value* a = initialize_value(4.0);
  struct Value* b = initialize_value(2.0);

  repr(a);
  repr(b);

  struct Value* c = add(a, b);
  repr(c);
  struct Value* d = initialize_value(-3.0);
  repr(d);
  struct Value* L = mul(c, d);
  repr(L);

  visualize_graph(L);

  return 0;
}

