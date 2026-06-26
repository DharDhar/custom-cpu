#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"

/*  TODO: implement label count limit
 *		  refactor error messages
 *
 */

#define BUFFER_SIZE 50
#define LABEL_SIZE 20

typedef struct Node {
	char label_name[LABEL_SIZE+1];
	int line;
	struct Node* next;
} Node;

int main(void)
{
	FILE *fp = fopen("program.txt", "r");
	char buf[BUFFER_SIZE] = {0};
	Node head_node = {{0}, 0,NULL};
	Node * current_node = &head_node;
	int i, line_num = 1;

	if (fp == NULL)
	{
		perror("Error opening file");
		return 1;
	}	

	while(fgets(buf, BUFFER_SIZE - 1, fp) != NULL)
	{
		for(i = 0; i < BUFFER_SIZE; i++)
			if (buf[i] == ':')
			{
					if (i > LABEL_SIZE + 1)
					{
						printf("Label cannot be greater than %d characters wide\n", LABEL_SIZE);
						fclose(fp);
						return 1;
					}
				buf[i] = '\0'; 
				strcpy(current_node->label_name, buf);
				current_node->line = line_num;
				current_node->next = calloc(1, sizeof(Node));
				current_node = current_node->next;
				current_node->next = NULL;
			}
		line_num++;
	}
	
	current_node = &head_node;
	do
	{
		printf("%s %d\n", current_node->label_name, current_node->line);
		current_node = current_node->next;
	}
	while (current_node->next != NULL);
		

	fclose(fp);

	return 0;
}
