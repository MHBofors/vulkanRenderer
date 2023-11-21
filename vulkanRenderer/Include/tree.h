//
//  tree.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-21.
//


#include <stdlib.h>

typedef struct tree_node {
    struct tree_node *left;
    struct tree_node *right;
    uint32_t value;
} tree_node;

typedef struct {
    tree_node *root;
    uint32_t size;
} binary_tree;

int node_alloc(tree_node **node, uint32_t value);
void node_free(tree_node *node);

int node_add(tree_node **node, uint32_t value);

int tree_alloc(binary_tree **tree);
void tree_free(binary_tree *tree);

int tree_add(binary_tree *tree, uint32_t value);
tree_node *tree_search(binary_tree *tree, uint32_t value);
