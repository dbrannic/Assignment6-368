#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TreeNode {
int block_label;
int block_width;
int block_height;
char cut_type;
int Xcoordinate, Ycoordinate;
struct TreeNode* leftChild;
struct TreeNode* rightChild;
} TreeNode;

TreeNode* buildTree(FILE* inputFile);
void freeTree(TreeNode* rootNode);
void compute_dimensions(TreeNode* currentNode, FILE* outputFile2);
void assign_coordinates(TreeNode* currentNode, int xOffset, int yOffset, FILE* outputFile3);
void pre_order_traversal(TreeNode* currentNode, FILE* outputFile1);
char* trim_whitespace(char* str);
TreeNode* createTreeNode(int label, int width, int height, char cut);

char* trim_whitespace(char* str) {
char* end;
while (isspace((unsigned char)*str)) str++;
if (*str == 0) return str;
end = str + strlen(str) - 1;
while (end > str && isspace((unsigned char)*end)) end--;
end[1] = '\0';
return str;
}

TreeNode* createTreeNode(int label, int width, int height, char cut) {
TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));

newNode->block_label = label;
newNode->block_width = width;
newNode->block_height = height;
newNode->cut_type = cut;
newNode->Xcoordinate = newNode->Ycoordinate = 0;
newNode->leftChild = newNode->rightChild = NULL;
return newNode;
}

void freeTree(TreeNode* rootNode) {
if (rootNode) {
freeTree(rootNode->leftChild);
freeTree(rootNode->rightChild);
free(rootNode);
}
}

TreeNode* buildTree(FILE* inputFile) {
char inputLine[256];
TreeNode* stack[100];
TreeNode* currentNode = NULL;
int top = -1;

while (fgets(inputLine, sizeof(inputLine), inputFile)) {
char* trimmedLine = trim_whitespace(inputLine);
if (strlen(trimmedLine) == 0) continue;

int label, width, height;
char cutChar = trimmedLine[0];

if (sscanf(trimmedLine, "%d(%d,%d)", &label, &width, &height) == 3) {
currentNode = createTreeNode(label, width, height, 'N');
}
else if (cutChar == 'V' || cutChar == 'H') {
if (top < 1) {
fprintf(stderr, "Error: Not enough nodes for cut %c\n", cutChar);
continue;
}
TreeNode* rightNode = stack[top--];
TreeNode* leftNode = stack[top--];
currentNode = createTreeNode(0, 0, 0, cutChar);
currentNode->leftChild = leftNode;
currentNode->rightChild = rightNode;

if (cutChar == 'V') {
currentNode->block_width = leftNode->block_width + rightNode->block_width;
currentNode->block_height = (leftNode->block_height > rightNode->block_height) ? leftNode->block_height : rightNode->block_height;
} else {
currentNode->block_width = (leftNode->block_width > rightNode->block_width) ? leftNode->block_width : rightNode->block_width;
currentNode->block_height = leftNode->block_height + rightNode->block_height;
}
}

stack[++top] = currentNode;
}

if (top != 0) {
fprintf(stderr, "Error: Incorrect number of nodes left in stack\n");
return NULL;
}

return stack[top];
}

void pre_order_traversal(TreeNode* currentNode, FILE* outputFile1) {
if (!currentNode) return;

if (currentNode->cut_type != 'N') {
fprintf(outputFile1, "%c\n", currentNode->cut_type);
}

if (currentNode->leftChild) {
if (currentNode->leftChild->cut_type == 'N') {
fprintf(outputFile1, "%d(%d,%d)\n", currentNode->leftChild->block_label, currentNode->leftChild->block_width, currentNode->leftChild->block_height);
} else {
pre_order_traversal(currentNode->leftChild, outputFile1);
}
}
if (currentNode->rightChild) {
if (currentNode->rightChild->cut_type == 'N') {
fprintf(outputFile1, "%d(%d,%d)\n", currentNode->rightChild->block_label, currentNode->rightChild->block_width, currentNode->rightChild->block_height);
} else {
pre_order_traversal(currentNode->rightChild, outputFile1);
}
}
}

void compute_dimensions(TreeNode* currentNode, FILE* outputFile2) {
if (!currentNode) return;

compute_dimensions(currentNode->leftChild, outputFile2);
compute_dimensions(currentNode->rightChild, outputFile2);

if (currentNode->cut_type == 'V') {
currentNode->block_width = currentNode->leftChild->block_width + currentNode->rightChild->block_width;
currentNode->block_height = (currentNode->leftChild->block_height > currentNode->rightChild->block_height) ? currentNode->leftChild->block_height : currentNode->rightChild->block_height;
} else if (currentNode->cut_type == 'H') {
currentNode->block_width = (currentNode->leftChild->block_width > currentNode->rightChild->block_width) ? currentNode->leftChild->block_width : currentNode->rightChild->block_width;
currentNode->block_height = currentNode->leftChild->block_height + currentNode->rightChild->block_height;
}

if (currentNode->cut_type == 'N') {
fprintf(outputFile2, "%d(%d,%d)\n", currentNode->block_label, currentNode->block_width, currentNode->block_height);
} else {
fprintf(outputFile2, "%c(%d,%d)\n", currentNode->cut_type, currentNode->block_width, currentNode->block_height);
}
}

void assign_coordinates(TreeNode* currentNode, int xOffset, int yOffset, FILE* outputFile3) {
if (!currentNode) return;

currentNode->Xcoordinate = xOffset;
currentNode->Ycoordinate = yOffset;

if (currentNode->cut_type == 'V') {
assign_coordinates(currentNode->leftChild, xOffset, yOffset, outputFile3);
assign_coordinates(currentNode->rightChild, xOffset + currentNode->leftChild->block_width, yOffset, outputFile3);
} else if (currentNode->cut_type == 'H') {
assign_coordinates(currentNode->leftChild, xOffset, yOffset + currentNode->rightChild->block_height, outputFile3);
assign_coordinates(currentNode->rightChild, xOffset, yOffset, outputFile3);
} else {
fprintf(outputFile3, "%d((%d,%d)(%d,%d))\n", currentNode->block_label, currentNode->block_width, currentNode->block_height, currentNode->Xcoordinate, currentNode->Ycoordinate);
}
}

int main (int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s in_file out_file1 out_file2 out_file3\n", argv[0]);
        return 1;
    }
    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        fprintf(stderr, "Error: Couldn't open input file %s\n", argv[1]);
        return 1;
    }
    TreeNode* rootNode = buildTree(inputFile);
    if (!rootNode) {
        fclose(inputFile);
        return 1;
    }
    FILE* outputFile1 = fopen(argv[2], "w");
    FILE* outputFile2 = fopen(argv[3], "w");
    FILE* outputFile3 = fopen(argv[4], "w");

    if (!outputFile1 || !outputFile2 || !outputFile3) {
        fprintf(stderr, "Error: Coultn't open one of the output files\n");
        fclose(inputFile);
        return 1;
    }

    pre_order_traversal(rootNode, outputFile1);
    compute_dimensions(rootNode, outputFile2);
    assign_coordinates(rootNode, 0, 0, outputFile3);

    freeTree(rootNode);
    fclose(inputFile);
    fclose(outputFile1);
    fclose(outputFile2);
    fclose(outputFile3);
}