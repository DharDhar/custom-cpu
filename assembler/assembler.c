#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "instructions.h"

/*  TODO: make head_node static
 *		  refactor error messages
 *		  
 */

#define BUFFER_SIZE 50
#define MAX_LABEL_SIZE 7
#define MAX_LABEL_COUNT 20
#define MAX_INSTRUCTIONS 10

#define FILE_OPEN_ERROR 1
#define LABEL_TOO_WIDE 2
#define TOO_MANY_LABELS 3
#define TOO_MANY_INSTRUCTIONS 4
#define SYMBOLS_IN_LABEL 5

typedef struct Node {
	char label_name[MAX_LABEL_SIZE+1];
	int line;
	int instruction_num;
	struct Node* next;
} Node;

void throw_error(int error_no, FILE *fp);
void create_label_node(Node **current_node);
void label_scraper(Node *head_node_ptr, FILE *fp);
bool is_line_label(int line_num, Node *head_node);

int main(void)
{
	FILE *fp = fopen("program.txt", "r");
	Node head_node = {{0}, 0, 0, NULL};

	char buf[BUFFER_SIZE] = {0};
	int line_num = 1;
	char *instr = {0};
	bool match_found = false;

	if (fp == NULL)
		throw_error(FILE_OPEN_ERROR, fp);

	label_scraper(&head_node, fp);
	fclose(fp);

	fp = fopen("program.txt", "r");

	while(fgets(buf, BUFFER_SIZE, fp) != NULL)
	{
		if (!isspace(buf[3]) && !is_line_label(line_num, &head_node)) //since 3rd char is empty
			printf("Error %d\n", line_num);							  //for legal instructions
		buf[3] = '\0';
		match_found = false;
		for (int i = 0; i < INSTRUCTION_COUNT; i++)
			if (strcmp(buf, instr_str[i].string) == 0)
			{
				printf("Match found %.3b\n", instr_str[i].opcode);
				match_found = true;
				break;
			}
		if (!match_found && !is_line_label(line_num, &head_node))
			printf("Unknown instruction at line %d\n", line_num);
		line_num++;
	}

	fclose(fp);
	return 0;
}

void label_scraper(Node *head_node_ptr, FILE *fp)
{
	Node * current_node = head_node_ptr;
	char buf[BUFFER_SIZE] = {0};
	int line_num = 1;
	int label_count = 0;
	int instruction_num = 0;
	int error = 0;
	bool symbols_in_label = false;
	bool is_label = false;


	while(fgets(buf, BUFFER_SIZE, fp) != NULL)
	{
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			if (buf[i] == ':')
			{
				for (int j = 0; j < i; j++)
				{
					if (!isalnum(buf[j]) && buf[j] != '_')
					{
						symbols_in_label = true;	
						printf("At line %d\n", line_num);
						printf("%s\n", buf);
					}
//printf("Here");
				}

				if (i > MAX_LABEL_SIZE && !symbols_in_label)
				{
					printf("At line %d\n", line_num);
					printf("%s\n", buf);
					error = LABEL_TOO_WIDE;
				} else if (label_count == MAX_LABEL_COUNT)
					error = TOO_MANY_LABELS;

				buf[i] = '\0'; 
				
				is_label = true;
				label_count++;
				strcpy(current_node->label_name, buf); //(dest, src)
				current_node->line = line_num;
				current_node->instruction_num = instruction_num;
				create_label_node(&current_node);
			}
		}
 
		line_num++;
		if (!is_label)
			instruction_num++;
		is_label = false;
	}

	if (symbols_in_label)
		error = SYMBOLS_IN_LABEL;

	if (error)
		throw_error(error, fp);

	return;
}

bool is_line_label(int line_num, Node *head_node)
{
	Node *current_node = head_node;

	while(1)
	{
		if (line_num == current_node->line)
			return true;
		if (current_node->next == NULL)
			break;
		else
			current_node = current_node->next;
	}
	return false;
}

void throw_error(int error_no, FILE *fp)
{
	switch(error_no) {
		case FILE_OPEN_ERROR: printf("Error opening program file\n"); break;
		case LABEL_TOO_WIDE: printf("Labels cannot be more than %d characters wide\n", MAX_LABEL_SIZE); break;
		case TOO_MANY_LABELS: printf("Number of labels cannot exceed %d\n", MAX_LABEL_COUNT); break;
		case SYMBOLS_IN_LABEL: printf("Labels cannot have symbols or spaces\n"); break;
	}

	fclose(fp);
	exit(EXIT_FAILURE);
}

void create_label_node(Node **current_node)
{
	(*current_node)->next = calloc(1, sizeof(Node));
	*current_node = (*current_node)->next;
	(*current_node)->next = NULL;
	return;
}
