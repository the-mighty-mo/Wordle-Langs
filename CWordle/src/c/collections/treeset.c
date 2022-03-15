/*
 * Author: Benjamin Hall
 */

#include "collections/treeset.h"

#include <stdint.h>
#include <string.h>

#define DEFAULT_CAP 16

treeset_t treeset_new(type_info_t type_info)
{
    treeset_t treeset = {
        0,
        NULL,
        type_info
    };
    return treeset;
}

treeset_t treeset_from(type_info_t type_info, void const *arr, size_t count)
{
    treeset_t treeset = {
        0,
        NULL,
        type_info
    };
    
    for (int i = 0; i < count; ++i) {
        treeset_insert(&treeset, arr + i * type_info.type_sz);
    }
    return treeset;
}

treeset_t treeset_clone(treeset_t const *treeset)
{
    if (treeset == NULL) {
        type_info_t info = {0, NULL, NULL, NULL};
        return treeset_new(info);
    }
    
    treeset_t set = treeset_new(treeset->type_info);

    void const *elem = NULL;
    for (int i = 0; i < treeset->len; ++i) {
        elem = treeset_get_next(treeset, elem);
        treeset_insert(&set, elem);
    }

    return set;
}

static void treenode_drop_rec(treenode_t *treenode, drop_fn drop)
{
    if (treenode == NULL) {
        return;
    }

    treenode_drop_rec(treenode->left, drop);
    treenode_drop_rec(treenode->right, drop);

    if (drop) {
        drop(treenode->data);
    }
    free(treenode->data);
    free(treenode);
}

void treeset_drop(treeset_t *treeset)
{
    if (treeset == NULL) {
        return;
    }

    treenode_drop_rec(treeset->root, treeset->type_info.drop);
    memset(treeset, 0, sizeof(*treeset));
}

int treeset_is_empty(treeset_t const *treeset)
{
    return treeset->len == 0;
}

static void insert_node_fixup(treeset_t *treeset, treenode_t *cur_node);

/**
 * Inserts a node at the given parent, using the given element
 * comparison to determine whether it should be a left or right child.
 */
static void insert_node(treeset_t *treeset, void const *elem, treenode_t *parent, int comparison)
{
    treenode_t *new_node = calloc(1, sizeof(*new_node));
    new_node->parent = parent;
    new_node->data = malloc(treeset->type_info.type_sz);
    memcpy(new_node->data, elem, treeset->type_info.type_sz);

    if (comparison < 0) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    insert_node_fixup(treeset, new_node);
}

void treeset_insert(treeset_t *treeset, void const *elem)
{
    if (treeset == NULL || elem == NULL) {
        return;
    }

    if (treeset->root == NULL) {
        /* empty tree, insert at root */
        treeset->root = calloc(1, sizeof(*treeset->root));
        treeset->root->data = malloc(treeset->type_info.type_sz);
        memcpy(treeset->root->data, elem, treeset->type_info.type_sz);
        /* root is always black */
        treeset->root->color = BLACK;
        ++treeset->len;
    }

    treenode_t *cur_node = treeset->root;
    treenode_t *prev_node = NULL;
    int comparison = 0;

    /* find the node at which to insert the new node */
    do {
        prev_node = cur_node;

        comparison = treeset->type_info.compare(elem, cur_node->data);
        if (comparison < 0) {
            cur_node = cur_node->left;
        } else if (comparison > 0) {
            cur_node = cur_node->right;
        } else {
            /* element already exists */
            cur_node = NULL;
        }
    } while (cur_node != NULL);

    if (comparison != 0) {
        /* only insert if the element doesn't already exist */
        insert_node(treeset, elem, prev_node, comparison);
        ++treeset->len;
    }
}

void treeset_clear(treeset_t *treeset)
{
    if (treeset == NULL) {
        return;
    }

    treenode_drop_rec(treeset->root, treeset->type_info.drop);
    treeset->root = NULL;
    treeset->len = 0;
}

static treenode_t *get_node(treeset_t const *treeset, void const *elem)
{
    if (treeset == NULL || elem == NULL) {
        return NULL;
    }

    treenode_t *node = treeset->root;
    while (node != NULL) {
        int comparison = treeset->type_info.compare(elem, node->data);
        if (comparison < 0) {
            node = node->left;
        } else if (comparison > 0) {
            node = node->right;
        } else {
            return node;
        }
    }
    return NULL;
}

int treeset_contains(treeset_t const *treeset, void const *elem)
{
    return get_node(treeset, elem) != NULL;
}

void const *treeset_get_next(treeset_t const *treeset, void const *elem)
{
    if (treeset == NULL) {
        return NULL;
    }

    if (elem == NULL) {
        treenode_t *node = treeset->root;
        while (node->left != NULL) {
            node = node->left;
        }
        return node->data;
    }

    treenode_t *node = get_node(treeset, elem);
    if (node->right) {
        node = node->right;
        while (node->left) {
            node = node->left;
        }
    } else {
        treenode_t *parent = node->parent;
        if (parent->right == node) {
            while (node == parent->right) {
                node = parent;
                parent = parent->parent;
            }
        }
        if (node->right != parent) {
            node = parent;
        }
    }

    return node->data;
}

static treenode_t *fix_red_uncle(treenode_t *cur_node, treenode_t *uncle);
static treenode_t *fix_zig_zag(treeset_t *treeset, treenode_t *cur_node, uint8_t isParentLeft);
static treenode_t *fix_rotate_chain(treeset_t *treeset, treenode_t *cur_node, uint8_t isParentLeft);

/**
 * Fixes the red-black tree after a red node has been inserted.
 * 
 * @param treeset
 * @param cur_node
 *        The red node inserted to the tree
 */
static void insert_node_fixup(treeset_t *treeset, treenode_t *cur_node)
{
    /*
     * once the parent is black, or we've reached the root node,
     * we no longer run the risk of having two reds in a row
     */
    while (cur_node != treeset->root && cur_node->parent->color == RED) {
        treenode_t *grandparent = cur_node->parent->parent;

        if (cur_node->parent == grandparent->left) {
            /* left subtree of grandparent, uncle is right subtree */
            treenode_t *uncle = grandparent->right;

            /* NULL = BLACK */
            if (uncle != NULL && uncle->color == RED) {
                cur_node = fix_red_uncle(cur_node, uncle);
            } else {
                if (cur_node == cur_node->parent->right) {
                    cur_node = fix_zig_zag(treeset, cur_node, 1);
                }
                cur_node = fix_rotate_chain(treeset, cur_node, 1);
            }
        } else {
            /* mirrored from left subtree implementation (right <-> left) */

            /* right subtree of grandparent, uncle is left subtree */
            treenode_t *uncle = grandparent->left;

            /* NULL = BLACK */
            if (uncle != NULL && uncle->color == RED) {
                cur_node = fix_red_uncle(cur_node, uncle);
            } else {
                if (cur_node == cur_node->parent->left) {
                    cur_node = fix_zig_zag(treeset, cur_node, 0);
                }
                cur_node = fix_rotate_chain(treeset, cur_node, 0);
            }
        }
    }

    /* the root is always black */
    treeset->root->color = BLACK;
}

static void left_rotate(treeset_t *treeset, treenode_t *node);
static void right_rotate(treeset_t *treeset, treenode_t *node);

/* 
 * Legend for documentation below:
 * 
 * G - grandparent
 * P - parent
 * U - uncle
 * C - current node
 * -{R/B} - red/black
 */

/**
 * Fixes a node with a red uncle.
 * 
 * Situation (or mirrored):
 * 
 *     G-B              G-R
 * P-R     U-R  =>  P-B     U-B
 * C-R              C-R
 * 
 * @return
 *        The next node to fix
 */
static treenode_t *fix_red_uncle(treenode_t *cur_node, treenode_t *uncle)
{
    cur_node->parent->parent->color = RED;
    cur_node->parent->color = BLACK;
    uncle->color = BLACK;

    /* repeat on grandparent since we made it red */
    cur_node = cur_node->parent->parent;
    return cur_node;
}

/**
 * Fixes a node that makes a zig-zag shape
 * with its parent and grandparent.
 * 
 * Situation (or mirrored):
 * 
 *     G-B              G-B
 * P-R      =>      C-R
 *     C-R      P-R
 * 
 * - rotate left at P
 * - P can now be treated like an inserted
 *   red node, so continue from P
 * 
 * @return
 *        The next node to fix
 */
static treenode_t *fix_zig_zag(treeset_t *treeset, treenode_t *cur_node, uint8_t is_parent_left)
{
    /* switch to operating on the parent */
    cur_node = cur_node->parent;

    if (is_parent_left) {
        left_rotate(treeset, cur_node);
    } else {
        right_rotate(treeset, cur_node);
    }

    /* continue from the parent */
    return cur_node;
}

/**
 * Fixes a node by rotating the Node-Parent-Grandparent
 * chain so the parent is the new root of the subtree.
 * 
 * Situation (or mirrored):
 * 
 *         G-B          P-B
 *     P-R      =>  C-R     G-R
 * C-R
 * 
 * - make P black and G red
 * - rotate right at G
 * - continue from the same node
 * 
 * @return
 *        The next node to fix
 */
static treenode_t *fix_rotate_chain(treeset_t *treeset, treenode_t *cur_node, uint8_t is_parent_left)
{
    /* fix the colors of the parent and grandparent */
    cur_node->parent->color = BLACK;
    cur_node->parent->parent->color = RED;

    if (is_parent_left) {
        right_rotate(treeset, cur_node->parent->parent);
    } else {
        left_rotate(treeset, cur_node->parent->parent);
    }

    /* continue from the same node */
    return cur_node;
}

/**
 * Rotates left at a given node.
 * 
 *     N                   R
 * L       R     =>    N       RR
 *      RL   RR      L   RL
 */
static void left_rotate(treeset_t *treeset, treenode_t *node)
{
    treenode_t *right_child = node->right;

    right_child->parent = node->parent;
    if (node->parent == NULL) {
        treeset->root = right_child;
    } else if (node ==  node->parent->left) {
        node->parent->left = right_child;
    } else {
        node->parent->right = right_child;
    }

    node->right = right_child->left;
    if (node->right != NULL) {
        node->right->parent = node;
    }

    right_child->left = node;
    node->parent = right_child;
}

/**
 * Rotates right at a given node.
 * 
 *        N               L
 *    L       R  =>  LL       N
 * LL   LR                 LR   R
 */
static void right_rotate(treeset_t *treeset, treenode_t *node)
{
    treenode_t *left_child = node->left;

    left_child->parent = node->parent;
    if (node->parent == NULL) {
        treeset->root = left_child;
    } else if (node ==  node->parent->left) {
        node->parent->left = left_child;
    } else {
        node->parent->right = left_child;
    }

    node->left = left_child->right;
    if (node->left != NULL) {
        node->left->parent = node;
    }

    left_child->right = node;
    node->parent = left_child;
}

static type_info_t const type_info = {
    sizeof(treeset_t),
    treeset_drop,
    NULL,
    NULL
};

type_info_t treeset_type_info(void)
{
    return type_info;
}
