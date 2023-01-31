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
    // Value, must be filled at some point. If -1, not yet evaluated.
    int32 value;
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
    // Value, is filled at creation!
    int32 value;
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
    // Value, must be filled at some point. If -1, not yet evaluated.
    int32 value;
    // The signal only has one parent, so it's value is the parent's.
    Node *parent;
} SignalNode;

int32 SignalNode_eval(Node *node) {
    SignalNode *signode = (SignalNode *)node;
    // Only evaluate if it was never done!
    if (node->value == -1)
        node->value = signode->parent->eval(signode->parent);

    if (node->value == -1) {
        printf("Fatal: Could not evaluate: %s\n", node->name);
        exit(1);
    }

#ifndef NDEBUG
    printf("%s -> %s = %d -> %d\n", signode->parent->name, node->name, signode->parent->value, node->value);
#endif

    return node->value;
}

void SignalNode_new(SignalNode *node, const char *name, Node *parent) {
    Node_new((Node *)node, name);
    node->type = SIGNAL_NODE;
    node->eval = SignalNode_eval;
    node->value = -1;
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
    // Value, must be filled at some point. If -1, not yet evaluated.
    int32 value;
    // The port has two parents. Except NOT which has one (left).
    Node *left;
    Node *right;
    uint8 op_type; // AND, OR, NOT etc.
} PortNode;

int32 PortNode_eval(Node *node) {
    // If already evaluated, return.
    if (node->value != -1)
        return node->value;

    PortNode *portnode = (PortNode *)node;

    int32 vl = portnode->left->eval(portnode->left);
    int32 vr = -1;
    if (portnode->right != NULL)
        vr = portnode->right->eval(portnode->right);

    // Based on op type.
    int32 val = 0;

    switch (portnode->op_type) {
    case OP_NOT: {
        // Could not eval arg, quit.
        if (vl < 0)
            val = -1;
        else
            val = (uint16) ~(uint16)vl;
        break;
    }
    case OP_AND: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            val = -1;
        else
            val = (uint16)((uint16)vl & (uint16)vr);
        break;
    }
    case OP_OR: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            val = -1;
        else
            val = (uint16)((uint16)vl | (uint16)vr);
        break;
    }
    case OP_LSHIFT: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            val = -1;
        else
            val = ((uint16)vl) << ((uint16)vr);
        break;
    }
    case OP_RSHIFT: {
        // Could not eval arg, quit.
        if (vl < 0 || vr < 0)
            val = -1;
        else
            val = vl >> ((uint16)vr);
        break;
    }
    default: {
        printf("Unknown op %d\n", portnode->op_type);
        val = -1;
    }
    }

    if (val == -1) {
        printf("Fatal: Could not evaluate: %s\n", node->name);
        exit(1);
    }

#ifndef NDEBUG
    if (portnode->op_type == OP_NOT)
        printf("NOT %s -> ... = NOT %d -> %d\n", portnode->left->name, portnode->left->value, node->value);
    else
        printf(
            "%s %s %s -> ... = %d %s %d -> %d\n", portnode->left->name, portnode->name, portnode->right->name,
            portnode->left->value, portnode->name, portnode->right->value, portnode->value
        );
#endif

    node->value = val;
    return val;
}

void PortNode_new(PortNode *node, const char *name, Node *left, Node *right, uint8 op_type) {
    Node_new((Node *)node, name);
    node->type = PORT_NODE;
    node->eval = PortNode_eval;
    node->value = -1;
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

    // For debug.
    int32 n_const;
    int32 n_signal;
    int32 n_port;
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
    graph->n_const = 0;
    graph->n_signal = 0;
    graph->n_port = 0;
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

    if (node->type == CONSTANT_NODE)
        graph->n_const++;
    else if (node->type == SIGNAL_NODE)
        graph->n_signal++;
    else if (node->type == PORT_NODE)
        graph->n_port++;

    return graph->n_nodes - 1;
}

int32 Graph_add_const_or_sig(Graph *graph, char *arg) {
    int32 out = -1;

    if (is_number(arg)) {
        ConstantNode *new_node = (ConstantNode *)malloc(sizeof(ConstantNode));
        uint16 value = (uint16)atoi(arg);
        ConstantNode_new(new_node, arg, value);
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
    SignalNode *out_node = (SignalNode *)graph->nodes[out_node_i];

    switch (n_args) {
    case 1: {
        // printf("Output node has parent: %s\n", args[0]);
        int32 par_i = Graph_add_const_or_sig(graph, args[0]);
        out_node->parent = graph->nodes[par_i];
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
        out_node->parent = (Node *)port_node;

        // Add the port node.
        Graph_add_node(graph, (Node *)port_node);

        break;
    }
    case 3: {
        // Add port node.
        PortNode *port_node = (PortNode *)malloc(sizeof(PortNode));
        PortNode_new(port_node, args[1], NULL, NULL, 0);

        // Two signals or constants.
        int32 left_i = Graph_add_const_or_sig(graph, args[0]);
        int32 right_i = Graph_add_const_or_sig(graph, args[2]);
        port_node->left = graph->nodes[left_i];
        port_node->right = graph->nodes[right_i];

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
        out_node->parent = (Node *)port_node;

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

    return 0;
}

int32 Graph_eval_index(const Graph *graph, const int index) {
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

    return Graph_eval_index(graph, sig_i);
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
        if (Graph_add_nodes(&graph, line) != 0)
            exit(1);

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    printf(
        "The graph has:\n\t%d signal nodes.\n\t%d port nodes.\n\t%d constant nodes.\n", graph.n_signal, graph.n_port,
        graph.n_const
    );

    // Print result. Should be 16076
    int32 a = Graph_eval_signal(&graph, "a");
    printf("Signal \"a\": %d.\n", a);

    // Reset graph.
    for (int i = 0; i < graph.n_nodes; i++)
        if (graph.nodes[i]->type != CONSTANT_NODE)
            graph.nodes[i]->value = -1;

    // Set b to the old signal from a.
    graph.nodes[graph.indices[signal_index("b")]]->value = a;

    // Find a again.
    int32 new_a = Graph_eval_signal(&graph, "a");
    printf("Signal \"a\" after override: %d\n", new_a);

    Graph_free(&graph);
    fclose(input);

    return 0;
}