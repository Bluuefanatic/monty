#include "monty.h"
#include <string.h>

void (*get_op_func(char *opcode))(stack_t **stack, unsigned int line_number, int arg);
void push(stack_t **stack, unsigned int line_number, int arg);
void parse_and_execute(char *line, stack_t **stack, unsigned int line_number);
void process_file(FILE *file);

/**
 * get_op_func - Get the function pointer for the given opcode.
 * @opcode: The opcode to search for.
 * Return: Function pointer or NULL if not found.
 */
void (*get_op_func(char *opcode))(stack_t **stack, unsigned int line_number, int arg)
{
    int i;
    instruction_t instructions[] = {
        {"push", push},
        {"pall", pall},
        {NULL, NULL}};

    for (i = 0; instructions[i].opcode; i++)
    {
        if (strcmp(opcode, instructions[i].opcode) == 0)
            return instructions[i].f;
    }

    return NULL; /* Opcode not found */
}

/**
 * push - Pushes an element onto the stack.
 * @stack: Pointer to the stack.
 * @line_number: Line number in the Monty byte code file.
 * @arg: The argument to be pushed onto the stack.
 */
void push(stack_t **stack, unsigned int line_number, int arg)
{
	stack_t *new_node;

	(void)line_number;

    new_node = malloc(sizeof(stack_t));

    if (!new_node)
    {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }

    new_node->n = arg;
    new_node->prev = NULL;
    new_node->next = *stack;

    if (*stack)
        (*stack)->prev = new_node;

    *stack = new_node;
}

/**
 * pall - Prints all values on the stack.
 * @stack: Pointer to the stack.
 * @line_number: Line number in the Monty byte code file (not used in pall).
 * @arg: The argument (not used in pall).
 *
 * Description: The pall function prints all values on the stack,
 * starting from the top of the stack. If the stack is empty, it does nothing.
 */
void pall(stack_t **stack, unsigned int line_number, int arg)
{
    stack_t *current;

    (void)line_number;
    (void)arg;

    current = *stack;

    while (current != NULL)
    {
        printf("%d\n", current->n);
        current = current->next;
    }
}

/**
 * parse_and_execute - Parse a line from the Monty byte code file and execute the opcode.
 * @line: The line to parse.
 * @stack: Pointer to the stack.
 * @line_number: Line number in the Monty byte code file.
 *
 * Description: The parse_and_execute function parses a line from the Monty byte code file,
 * extracts the opcode and its argument, and executes the corresponding function.
 */
void parse_and_execute(char *line, stack_t **stack, unsigned int line_number)
{
	int int_arg = 0;
    char *opcode, *arg;
    void (*op_func)(stack_t **stack, unsigned int line_number, int arg);

    opcode = strtok(line, " \t\n");
    if (!opcode || opcode[0] == '#') /* Ignore comments and empty lines */
        return;

    arg = strtok(NULL, " \t\n");

    op_func = get_op_func(opcode);
    if (!op_func)
    {
        fprintf(stderr, "L%d: unknown instruction %s\n", line_number, opcode);
        exit(EXIT_FAILURE);
    }

    int_arg = arg ? atoi(arg) : 0;

    op_func(stack, line_number, int_arg);
}

/**
 * process_file - Process the Monty byte code file.
 * @file: Pointer to the Monty byte code file.
 *
 * Description: The process_file function reads the Monty byte code file line by line,
 * and for each line, it parses and executes the corresponding opcode.
 */
void process_file(FILE *file)
{
    char *line = NULL;
    size_t len = 0;
    unsigned int line_number = 0;
    stack_t *stack = NULL;

    while (fgets(line, len, file) != NULL)
    {
        line_number++;

        parse_and_execute(line, &stack, line_number);
    }

    free(line);
}

/**
 * main - Entry point of the Monty interpreter.
 * @argc: Number of command-line arguments.
 * @argv: Array of command-line arguments.
 *
 * Description: The main function checks for the correct number of command-line arguments,
 * opens the Monty byte code file, and processes it using the process_file function.
 */
int main(int argc, char *argv[])
{
	FILE *file;
    if (argc != 2)
    {
        fprintf(stderr, "USAGE: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    file = fopen(argv[1], "r");
    if (!file)
    {
        fprintf(stderr, "Error: Can't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    process_file(file);

    fclose(file);
    exit(EXIT_SUCCESS);
}
