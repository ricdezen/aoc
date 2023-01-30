// This has been an absolute nightmare in C. Kill me.

#define _CRT_SECURE_NO_DEPRECATE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

#define MAX_LINE_LENGTH 128

// --- Graph structure ---

#define INITIAL_NODES 8
#define CONSTANT_NODE 0
#define SIGNAL_NODE 1
#define PORT_NODE 2

#define OP_NOT 0
#define OP_AND 1
#define OP_OR 2
#define OP_LSHIFT 3
#define OP_RSHIFT 4

// --- Base node ---

typedef struct Node {
    // Type of the node (constant, signal, logical port etc.).
    int type;
    // Name of the node, for debugging purposes.
    char *name;
    // Virtual function used to evaluate the node.
    int32 (*eval)(struct Node *);
} Node;

void Node_new(Node *node, const char *name) {
    int len = strlen(name);
    node->name = (char *)malloc(len + 1);
    strncpy(node->name, name, len);
    node->name[len] = 0;
}

// --- Constant node ---

typedef struct {
    // Type of the node (should always be CONSTANT_NODE).
    int type;
    // Name of the node, for debugging purposes.
    char *name;
    // Should always be ConstantNode_eval.
    int32 (*eval)(Node *);
    // Only relevant field is value.
    uint16 value;
} ConstantNode;

int32 ConstantNode_eval(Node *node) { return ((ConstantNode *)node)->value; }

void ConstantNode_new(ConstantNode *node, const char *name, uint16 value) {
    Node_new((Node *)node, name);
    node->type = CONSTANT_NODE;
    node->eval = ConstantNode_eval;
    node->value = value;
}

// --- Signal node ---

typedef struct {
    // Type of the node (should always be SIGNAL_NODE).
    int type;
    // Name of the node, for debugging purposes.
    char *name;
    // Should always be SignalNode_eval.
    int32 (*eval)(Node *);
    // The signal only has one parent, so it's value is the parent's.
    Node *parent;
} SignalNode;

int32 SignalNode_eval(Node *node) {
    SignalNode *signode = (SignalNode *)node;
    return signode->parent->eval(signode->parent);
}

void SignalNode_new(SignalNode *node, const char *name, Node *parent) {
    Node_new((Node *)node, name);
    node->type = SIGNAL_NODE;
    node->eval = SignalNode_eval;
    node->parent = parent;
}

// --- Port node ---

typedef struct {
    // Type of the node (should always be PORT_NODE).
    int type;
    // Name of the node, for debugging purposes.
    char *name;
    // Runs the operation after evaluating the parents.
    int32 (*eval)(Node *);
    // The port has two parents. Except NOT which has one (left).
    Node *left;
    Node *right;
    uint8 op_type; // AND, OR, NOT etc.
} PortNode;

int32 PortNode_eval(Node *node) {
    PortNode *portnode = (PortNode *)node;

    uint16 vl = portnode->left->eval(portnode->left);
    uint16 vr = 0;
    if (portnode->right != NULL)
        vr = portnode->right->eval(portnode->right);

    // Based on op type.
    switch (portnode->op_type) {
    case OP_NOT: {
        // Could not eval arg, quit.
        if (vl < 0)
            return -1;
        uint16 val = ~((uint16)vl);
        return val;
    }
    case OP_AND: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            return -1;
        uint16 val = (uint16)vl & (uint16)vr;
        return val;
    }
    case OP_OR: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            return -1;
        uint16 val = (uint16)vl | (uint16)vr;
        return val;
    }
    case OP_LSHIFT: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            return -1;
        uint16 val = (uint16)vl << (uint16)vr;
        return val;
    }
    case OP_RSHIFT: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            return -1;
        uint16 val = (uint16)vl >> (uint16)vr;
        return val;
    }
    default: {
        printf("Unknown op %d\n", portnode->op_type);
        return -1;
    }
    }
}

void PortNode_new(PortNode *node, const char *name, Node *left, Node *right, uint8 op_type) {
    Node_new((Node *)node, name);
    node->type = PORT_NODE;
    node->eval = PortNode_eval;
    node->op_type = op_type;
    node->left = left;
    node->right = right;
}

// --- Actual graph ---

typedef struct {
    // Keeps index of node with name "xx" in the array nodes (which are not all the nodes).
    int32 *indices;
    // List of nodes, will be expanded as needed.
    Node **nodes;
    // Number of nodes in the graph.
    int32 n_nodes;
    int32 capacity;
} Graph;

// Signal MUST be of length 2, or length 1 with 0 terminator.
int32 signal_index(const char *signal) {
    int32 s0 = (uint8)signal[0];
    int32 s1 = (uint8)signal[1];
    return s0 * 256 + s1;
}

int is_number(const char *string) {
    int len = strlen(string);
    for (int i = 0; i < len; i++)
        if (string[i] < '0' || string[i] > '9')
            return 0;
    return 1;
}

void Graph_new(Graph *graph) {
    graph->indices = (int32 *)malloc(sizeof(int32) * 256 * 256);
    for (int i = 0; i < 256 * 256; i++)
        graph->indices[i] = -1;
    graph->nodes = (Node **)malloc(sizeof(Node *) * INITIAL_NODES);
    graph->capacity = INITIAL_NODES;
    graph->n_nodes = 0;
}

void Graph_free(Graph *graph) {
    for (int i = 0; i < graph->n_nodes; i++) {
        free(graph->nodes[i]->name);
        free(graph->nodes[i]);
    }
    free(graph->indices);
    free(graph->nodes);
    graph->n_nodes = 0;
    graph->capacity = 0;
}

int32 Graph_add_node(Graph *graph, Node *node) {
    // Resize node array if needed.
    if (graph->n_nodes == graph->capacity) {
        graph->nodes = (Node **)realloc(graph->nodes, sizeof(Node *) * graph->capacity * 2);
        graph->capacity *= 2;
    }
    // Push new node.
    graph->nodes[graph->n_nodes++] = node;
    return graph->n_nodes - 1;
}

int32 Graph_add_const_or_sig(Graph *graph, char *arg) {
    int32 out = -1;

    if (is_number(arg)) {
        ConstantNode *new_node = (ConstantNode *)malloc(sizeof(ConstantNode));
        ConstantNode_new(new_node, arg, (uint16)atoi(arg));
        out = Graph_add_node(graph, (Node *)new_node);
    } else {
        int32 par_i = signal_index(arg);
        out = graph->indices[par_i];

        if (out == -1) {
            SignalNode *new_node = (SignalNode *)malloc(sizeof(SignalNode));
            SignalNode_new(new_node, arg, NULL);
            out = Graph_add_node(graph, (Node *)new_node);
            graph->indices[par_i] = out;
        }
    }

    return out;
}

int32 Graph_add_nodes(Graph *graph, char *line) {
    // If the operands are already present in the nodes, then no problem.
    // Otherwise I must add nodes for the operands. But their parents could be unknown (-1).
    // If there is an operation, that is a new node too, which has operands as parents.
    // The output must be added if not present, and its parent is known!
    char *args[4] = {0};
    args[0] = strtok(line, " ");
    int n_args = 1;

    // Parse args till arrow.
    while (strncmp(args[n_args++] = strtok(NULL, " "), "->", 2))
        ;
    n_args--; // Exclude arrow.

    // Output arg.
    char *output = strtok(NULL, " ");

    // Add output, if not already present.
    int32 out_i = signal_index(output);
    int32 out_node_i = graph->indices[out_i];

    if (out_node_i == -1) {
        // Add a new Signal node and store the new index.
        SignalNode *new_node = (SignalNode *)malloc(sizeof(SignalNode));
        SignalNode_new(new_node, output, NULL);
        out_node_i = Graph_add_node(graph, (Node *)new_node);
        graph->indices[out_i] = out_node_i;
    } // We don't need all the rest if there is an error.
    else if (((SignalNode *)(graph->nodes[out_node_i]))->parent != NULL) {
        printf("Someone is already parent to output node %s!\n", output);
        return -1;
    }

    // Add appropriate nodes from the inputs to the node.
    // At the end we will set the parent of the output node.

    Node *parent = NULL;

    switch (n_args) {
    case 1: {
        // printf("Output node has parent: %s\n", args[0]);
        parent = graph->nodes[Graph_add_const_or_sig(graph, args[0])];
        break;
    }
    case 2: {
        // NOT signal or constant.
        if (strncmp(args[0], "NOT", 3)) {
            printf("Expected NOT and got %s\n", args[0]);
            return -1;
        }

        // Add NOT node.
        PortNode *port_node = (PortNode *)malloc(sizeof(PortNode));
        PortNode_new(port_node, "NOT", NULL, NULL, OP_NOT);

        // One signal or constant.
        port_node->left = graph->nodes[Graph_add_const_or_sig(graph, args[1])];

        // Parent of the output is the port node.
        parent = (Node *)port_node;

        // Add the port node.
        Graph_add_node(graph, (Node *)port_node);

        break;
    }
    case 3: {
        // Add port node.
        PortNode *port_node = (PortNode *)malloc(sizeof(PortNode));
        PortNode_new(port_node, args[1], NULL, NULL, 0);

        // Two signals or constants.
        port_node->left = graph->nodes[Graph_add_const_or_sig(graph, args[0])];
        port_node->right = graph->nodes[Graph_add_const_or_sig(graph, args[2])];

        if (!strncmp(args[1], "AND", 3))
            port_node->op_type = OP_AND;
        else if (!strncmp(args[1], "OR", 2))
            port_node->op_type = OP_OR;
        else if (!strncmp(args[1], "LSHIFT", 6))
            port_node->op_type = OP_LSHIFT;
        else if (!strncmp(args[1], "RSHIFT", 6))
            port_node->op_type = OP_RSHIFT;
        else {
            printf("Unknown operation type: %s\n", args[1]);
            return -1;
        }

        // Parent of the output is the port node.
        parent = (Node *)port_node;

        // Add the port node.
        Graph_add_node(graph, (Node *)port_node);

        break;
    }
    default: {
        printf("Wrong args, probably.\n");
        return -1;
        break;
    }
    }

    // Set parent of the output node, be it added or pre-existing.
    ((SignalNode *)(graph->nodes[out_node_i]))->parent = parent;

    return 0;
}

int32 Graph_eval_index(const Graph *graph, const int index, int depth) {
    // Index is not valid.
    if (index < 0 || index >= graph->n_nodes) {
        printf("Index %d is invalid.\n", index);
        return -1;
    }

    // Node is in graph. Evaluate it recursively based on parents.
    Node *node = graph->nodes[index];
    return node->eval(node);
}

// Returns 0-65535 if signal can be evaluated or -1 if not.
int32 Graph_eval_signal(const Graph *graph, const char *name) {
    // Check name makes sense.
    if (strlen(name) > 2) {
        printf("Wrong name length for: %s (must be 1 or 2).\n", name);
        return -1;
    }

    // If signal is not in graph, fails.
    int32 sig_i = graph->indices[signal_index(name)];
    if (sig_i == -1) {
        printf("Signal %s not in graph.\n", name);
        return -1;
    }

    return Graph_eval_index(graph, sig_i, 0);
}

// --- Main ---

int freadline(FILE *file, char *line, int max_len) {
    int c = EOF, len = 0;
    while (len < max_len && (c = fgetc(file)) != EOF && c != '\n')
        line[len++] = (char)c;
    line[len] = 0;
    return len;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_7.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, sizeof(line));
    Graph graph;
    Graph_new(&graph);

    while (len != 0) {
        // Parse line to add nodes.
        Graph_add_nodes(&graph, line);

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Print result. Should be 16076
    printf("Signal \"a\": %d.\n", Graph_eval_signal(&graph, "a"));

    Graph_free(&graph);
    fclose(input);

    return 0;
}