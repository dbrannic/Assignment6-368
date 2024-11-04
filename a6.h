#ifndef a6_H
#define a6_H
#include <stdio.h>

typedef enum { LEAF, CUT } NodeType;
typedef enum { HORIZONTAL, VERTICAL } CutType;

typedef struct TreeNode {
    NodeType type;
    int width, height;
    int x, y;
    CutType cutType;
    struct TreeNode *left, *right;
} TreeNode;

TreeNode* createLeafNode(int width, int height);
TreeNode* createCutNode(CutType cutType, TreeNode* left, TreeNode* right);
void preOrderTraversal(TreeNode *root, FILE *output);
void postOrderTraversal(TreeNode *root, FILE *output);
void calculateDimensions(TreeNode *root);
void calculateCoordinates(TreeNode *root, int x, int y);

#endif
