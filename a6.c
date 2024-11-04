#include <stdio.h>
#include <stdlib.h>
#include "a6.h"

TreeNode* createLeafNode(int width, int height) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    node->type = LEAF;
    node->width = width;
    node->height = height;
    node->x = node->y = 0;
    node->left = node->right = NULL;
    return node;
}

TreeNode* createCutNode(CutType cutType, TreeNode* left, TreeNode* right) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    node->type = CUT;
    node->cutType = cutType;
    node->left = left;
    node->right = right;
    node->width = node->height = 0;
    return node;
}

void preOrderTraversal(TreeNode *root, FILE *output) {
    if (!root) return;
    if (root->type == LEAF)
        fprintf(output, "Leaf: Width=%d, Height=%d\n", root->width, root->height);
    else
        fprintf(output, "Cut: %c\n", (root->cutType == HORIZONTAL) ? 'H' : 'V');
    preOrderTraversal(root->left, output);
    preOrderTraversal(root->right, output);
}

void calculateDimensions(TreeNode *root) {
    if (!root) return;
    if (root->type == LEAF) return;
    calculateDimensions(root->left);
    calculateDimensions(root->right);
    if (root->cutType == HORIZONTAL) {
        root->width = root->left->width > root->right->width ? root->left->width : root->right->width;
        root->height = root->left->height + root->right->height;
    } else {
        root->width = root->left->width + root->right->width;
        root->height = root->left->height > root->right->height ? root->left->height : root->right->height;
    }
}

void calculateCoordinates(TreeNode *root, int x, int y) {
    if (!root) return;
    if (root->type == LEAF) {
        root->x = x;
        root->y = y;
    } else {
        if (root->cutType == HORIZONTAL) {
            calculateCoordinates(root->left, x, y + root->right->height);
            calculateCoordinates(root->right, x, y);
        } else {
            calculateCoordinates(root->left, x, y);
            calculateCoordinates(root->right, x + root->left->width, y);
        }
    }
}

void postOrderTraversal(TreeNode *root, FILE *output) {
    if (!root) return;
    postOrderTraversal(root->left, output);
    postOrderTraversal(root->right, output);
    fprintf(output, "Node: Width=%d, Height=%d, X=%d, Y=%d\n", root->width, root->height, root->x, root->y);
}

TreeNode* buildExampleTree() {
    TreeNode *left = createLeafNode(3, 2);
    TreeNode *right = createLeafNode(2, 4);
    TreeNode *cut = createCutNode(HORIZONTAL, left, right);
    return cut;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <preorder_output> <dimensions_output> <coordinates_output>\n", argv[0]);
        return 1;
    }

    FILE *outFile1 = fopen(argv[1], "w");
    FILE *outFile2 = fopen(argv[2], "w");
    FILE *outFile3 = fopen(argv[3], "w");
    if (!outFile1 || !outFile2 || !outFile3) {
        perror("Error opening output files");
        return 1;
    }

    TreeNode *root = buildExampleTree();

    preOrderTraversal(root, outFile1);

    calculateDimensions(root);
    postOrderTraversal(root, outFile2);

    calculateCoordinates(root, 0, 0);
    postOrderTraversal(root, outFile3);

    fclose(outFile1);
    fclose(outFile2);
    fclose(outFile3);
    free(root);

    return 0;
}