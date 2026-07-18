#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "instructions.h"

/*  TODO: cleanup 
 *		 
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
void write_u16_big_endian(FILE *f, uint32_t value);
bool is_buf_whitespace(char *buf);

int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "r");
	FILE *fo = fopen(argv[2], "wb");

	uint16_t instruction_word;
	char instruction_buf[17] = {0};

	char buf[BUFFER_SIZE] = {0};
	int line_num = 1;
	char *instr = {0};
	bool match_found = false;
	int errorno = 0;
	char field1[10] = {0};
	char field2[20] = {0};
	char field3[10] = {0};

	if (fp == NULL)
	{
		printf("Error opening prog file\n");
		exit(EXIT_FAILURE);
	}

	if (fo == NULL)
	{
		printf("Error opening output file\n");
		exit(EXIT_FAILURE);
	}

	label_scraper(fp);
	fclose(fp);

	fp = fopen(argv[1], "r");

	while(fgets(buf, BUFFER_SIZE, fp) != NULL)
	{
		if (isspace(buf[0]))
			continue;
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
					case LDR: sprintf(instruction_buf,"%.10b%.3b%.3b", atoi(field1), atoi(&field2[1]), i); break;
					case MOV: sprintf(instruction_buf, "00000"); 
							  if (field2[0] == 'r')
								  sprintf(&instruction_buf[5], "0%.3b", atoi(&field2[1]));
							  else if (strcmp(field2, "@MP") == 0)
								  sprintf(&instruction_buf[5], "1000");
							  if (field1[0] == 'r')
								  sprintf(&instruction_buf[9], "0%.3b", atoi(&field1[1]));
							  else if (strcmp(field1, "@MP") == 0)
								  sprintf(&instruction_buf[9], "1000");
							  sprintf(&instruction_buf[13], "001");
						  break;
					case ARS:
					case LRS:
					case LLS: sprintf(instruction_buf, "0%.3b%.3b%.3b%.3b%.3b", i-2, atoi(&field2[1]), 0, atoi(&field1[1]), instr_str[i].opcode); break;
					case ADC:
					case SBC:
					case AND:
					case ORR:
					case XOR: sprintf(instruction_buf, "0%.3b%.3b%.3b%.3b%.3b", i-2, atoi(&field3[1]), atoi(&field2[1]), atoi(&field1[1]), instr_str[i].opcode); break;
					case POP:
					case PSH: sprintf(instruction_buf, "00000000%.1b", (i+1)==POP); //pop->1  push->0
							  if (field1[0] == 'r')
							      sprintf(&instruction_buf[9], "0%.3b%.3b", atoi(&field1[1]), instr_str[i].opcode); 
							  else if (strcmp(field1, "PC") == 0)    //psh pc
								  sprintf(&instruction_buf[9], "1000%.3b", instr_str[i].opcode);
							  else if (strcmp(field1, "@MP") == 0)    //psh mp
								  sprintf(&instruction_buf[9], "1001%.3b", instr_str[i].opcode);
							  break;
					case JMR:
					case BZC:
					case BZS:
					case BCC:
					case BCS: sprintf(instruction_buf, "0000000%.3b%.3b%.3b", atoi(&field1[1]), i-13, instr_str[i].opcode);
							  break;
					case JMI: if(is_str_label(field1))
							  	  sprintf(instruction_buf, "%.13b%.3b", label_instruction(field1), instr_str[i].opcode);
							  else 
							  {
								  printf("Unknown label at line %d\n", line_num);
								  errorno = UNKNOWN_LABEL;
							  } break;
					case SEC: sprintf(instruction_buf, "00000000000" "00" "110"); break;
					case CLC: sprintf(instruction_buf, "00000000000" "01" "110"); break;
					case SEZ: sprintf(instruction_buf, "00000000000" "10" "110"); break;
					case CLZ: sprintf(instruction_buf, "00000000000" "11" "110"); break;
				}
				match_found = true;
				break;
			}
	if (!match_found && !is_line_label(line_num)) 
	{
		printf("Unknown instruction at line %d\n", line_num);
		errorno = UNKNOWN_INSTRUCTION;
	}

	if (match_found)
	{
		printf("%s\n", instruction_buf);
		instruction_word = strtol(instruction_buf, NULL, 2);
		write_u16_big_endian(fo, instruction_word);
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


bool is_buf_whitespace(char *buf)
{
	for (int i = 0; buf[i] != '\0'; i++)
		if (!isspace(buf[i]))
			return false;

	return true;
}

void write_u16_big_endian(FILE *f, uint32_t value)
{
	uint8_t bytes[2];
	bytes[0] = (value>>8) & 0xFF;
	bytes[1] = (value) & 0xFF;

	fwrite(bytes, 1, 2, f); 
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
