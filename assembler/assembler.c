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
#define UNKNOWN_INSTRUCTION 6
#define UNKNOWN_LABEL 7

typedef struct Node {
	char label_name[MAX_LABEL_SIZE+1];
	int line;
	int instruction_num;
	struct Node* next;
} Node;

Node head_node = {{0}, 0, 0, NULL};

void throw_error(int error_no, FILE *fp);
void create_label_node(Node **current_node);
void label_scraper(FILE *fp);
bool is_line_label(int line_num);
int label_instruction(char *label);
bool is_str_label(char *str);

int main(void)
{
	FILE *fp = fopen("program.txt", "r");

	char buf[BUFFER_SIZE] = {0};
	int line_num = 1;
	char *instr = {0};
	bool match_found = false;
	int errorno = 0;
	char field1[10] = {0};
	char field2[20] = {0};
	char field3[10] = {0};

	if (fp == NULL)
		throw_error(FILE_OPEN_ERROR, fp);

	label_scraper(fp);
	fclose(fp);

	fp = fopen("program.txt", "r");

	while(fgets(buf, BUFFER_SIZE, fp) != NULL)
	{
		if (!isspace(buf[3]) && !is_line_label(line_num)) //since 3rd char is empty
			printf("Error %d\n", line_num);							  //for legal instructions
/*
		if (!is_line_label(line_num))
			printf("%s", buf);
*/
		buf[3] = '\0';
		match_found = false;
		for (int i = 0; i < INSTRUCTION_COUNT; i++)
			if (strcmp(buf, instr_str[i].string) == 0)
			{
				sscanf(&buf[4], "%s %s %s", field1, field2, field3);
//		printf("%s %s %s \n", field1, field2, field3);
				switch (i + 1) {
					case LDR: printf("%.10b%.3b%.3b\n", atoi(field1), atoi(&field2[1]), i); break;
					case LDM: printf("%.13b%.3b\n", atoi(field1), i); break;
					case ARS:
					case LRS:
					case LLS: field3[1] = 0;
					case ADC:
					case SBC:
					case AND:
					case ORR:
					case XOR: printf("0%.3b%.3b%.3b%.3b%.3b\n", i-2, atoi(&field1[1]), atoi(&field3[1]), atoi(&field2[1]), instr_str[i].opcode); break;
					case POP:
					case PSH: printf("00000000%.1b", (i+1)==POP);
							  if (field1[0] == 'r')
							      printf("0%.3b%.3b\n", atoi(&field1[1]), instr_str[i].opcode); 
							  else if (field1[0] == 'p' && field1[1] == 'c')    //psh pc
								  printf("1000%.3b\n", instr_str[i].opcode);
							  else if (field1[0] == 'm' && field1[1] == 'p')    //psh mp
								  printf("1001%.3b\n", instr_str[i].opcode);
							  break;
					case JMR:
					case BZC:
					case BZS:
					case BCC:
					case BCS: printf("0000000%.3b%.3b%.3b\n", field1[1], i-13, instr_str[i].opcode);
							  break;
					case JMI: if(is_str_label(field1))
							  	  printf("%.13b%.3b\n", label_instruction(field1), instr_str[i].opcode);
							  else 
							  {
								  printf("Unknown label at line %d\n", line_num);
								  errorno = UNKNOWN_LABEL;
							  } break;
					case SEC: printf("00000000000" "00" "110\n"); break;
					case CLC: printf("00000000000" "01" "110\n"); break;
					case SEZ: printf("00000000000" "10" "110\n"); break;
					case CLZ: printf("00000000000" "11" "110\n"); break;
				}
				match_found = true;
				break;
			}
		if (!match_found && !is_line_label(line_num)) 
		{
			printf("Unknown instruction at line %d\n", line_num);
			errorno = UNKNOWN_INSTRUCTION;
		}

	if (errorno)
	{
		fclose(fp);
		exit(EXIT_FAILURE);
	}
		line_num++;
		
		field1[0] = 0;
		field2[0] = 0;
		field3[0] = 0;
	}

	fclose(fp);
	return 0;
}

void label_scraper(FILE *fp)
{
	Node * current_node = &head_node;
	char buf[BUFFER_SIZE] = {0};
	int line_num = 1;
	int label_count = 0;
	int instruction_num = 0;
	int error = 0;
	bool symbols_in_label = false;
	bool is_label = false;


	while(fgets(buf, BUFFER_SIZE, fp) != NULL) //fgets returns on encountering \n, EOF or BUFFER_SIZE  
	{                                          //bytes have been read. NULL is returned on EOF.
		for(int i = 0; i < BUFFER_SIZE; i++)   //cycling through the buffer a character at a time
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

bool is_line_label(int line_num)
{
	Node *current_node = &head_node;

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

bool is_str_label(char *str)
{
	Node *current_node = &head_node;

	while(1)
	{
		if (strcmp(str, current_node->label_name) == 0)
			return true;
		if (current_node->next == NULL)
			break;
		else
			current_node = current_node->next;
	}
	return false;
}

int label_instruction(char *label)
{
	Node *current_node = &head_node;

	while(1)
	{
		if (strcmp(label, current_node->label_name) == 0)
			return current_node->instruction_num;
		if (current_node->next == NULL)
			break;
		else
			current_node = current_node->next;
	}
	return 0;
}
