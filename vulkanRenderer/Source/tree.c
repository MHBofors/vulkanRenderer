//
//  tree.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-21.
//

#include "tree.h"

int node_alloc(tree_node **node, uint32_t value) {
    tree_node *node_new = malloc(sizeof(tree_node));
    if(node_new == NULL) {
        return -1;
    } else {
        *node_new = (tree_node){
            .left = NULL,
            .right = NULL,
            .value = value
        };
        return 0;
    }
}
void node_free(tree_node *node) {
    if(node != NULL) {
        node_free(node->left);
        node_free(node->right);
        free(node);
    } 
}

int node_add(tree_node **node, uint32_t value) {
    tree_node *root = *node;
    if(root == NULL) {
        if(node_alloc(&root, value) != -1) {
            return 1;
        } else {
            return 0;
        }
    } else if(value < root->value) {
        return node_add(&root->left, value);
    } else if(root->value < value) {
        return node_add(&root->right, value);
    }
    return 0;
}

int tree_alloc(binary_tree **tree) {
    binary_tree *tree_new = malloc(sizeof(binary_tree));
    if(tree_new == NULL) {
        return -1;
    } else {
        *tree_new = (binary_tree){
            .root = NULL,
            .size = 0
        };
        return 0;
    }
}

void tree_free(binary_tree *tree) {
    node_free(tree->root);
    free(tree);
}

int tree_add(binary_tree *tree, uint32_t value) {
    int val_added = (&tree->root, value);
    tree->size += val_added;
    return val_added;
}

tree_node *tree_search(binary_tree *tree, uint32_t value);
