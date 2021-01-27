#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "../stack.h"
#include "../queue.h"
#include "../element.h"
#include "../strings.h"


 
int currentIndex = 1;
int startIndex = -1;

Stack* initStack() {
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	if (stack==NULL)
	{
		return NULL;
	}
	stack->size = currentIndex;
	stack->topIndex = startIndex;
	stack->content = (Element*)malloc(sizeof(Element));
	return stack;
}

void destroyStack(Stack* stack) {
	if (stack) {
		free(stack->content);
		free(stack);
	}
}

int isStackEmpty(Stack* stack) {
	return stack->topIndex == -1;
}

int lenOfStack(Stack* stack) {
	return stack->topIndex + 1;
}

int capacityOfStack(Stack* stack) {
	return stack->size;
}

int isStackFull(Stack* stack)
{
	return (lenOfStack(stack) == capacityOfStack(stack));
}

void push(Stack* stack, Element element) {
	if (isStackFull(stack)) {
		stack->size *= 2;
		Element* copy = stack->content;
		stack->content = (Element*)realloc(stack->content, sizeof(Element)*stack->size);
		if (stack->content == NULL) {
			stack->content = copy;
		}
	}

	stack->topIndex++;
	stack->content[stack->topIndex] = element;	
}


Element pop(Stack* stack) {
	assert(!isStackEmpty(stack));

	Element elem = stack->content[stack->topIndex];

	if ((lenOfStack(stack) - 1) == (capacityOfStack(stack) / 2))
	{
		stack->size /= 2;
		if (0 == stack->size) {
			stack->size = 1;
		}
		Element* copy = stack->content;
			stack->content = (Element*)realloc(stack->content, sizeof(Element) * (stack->size));
			if (stack->content == NULL) {
				stack->content = copy;
			}
	}
	
	
	stack->topIndex--;
	return elem;
}

Element top(Stack* stack) {
	return stack->content[stack->topIndex];
}

void printStack(Stack* stack) {
	int tempTopIndex = stack->topIndex;
	for (int i = 0; i <= tempTopIndex; i++) {
		printElement(top(stack));
		printf(" ");
		stack->topIndex -= 1;
	}
	stack->topIndex = tempTopIndex; //need to save top index?
}


Queue* initQueue() {
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	if (queue==NULL)
	{
		return NULL;
	}
	queue->s1 = initStack();
	queue->s2 = initStack();
	return queue;
};

void destroyQueue(Queue* queue) {
	if (queue) {
		destroyStack(queue->s1);
		destroyStack(queue->s2);
		free(queue);
	}
}

int isQueueEmpty(Queue* queue) {
	return isStackEmpty(queue->s1);
};

int isQueueFull(Queue* queue)
{
	return isStackFull(queue->s1);
}

int lenOfQueue(Queue* queue) {
	return queue->s1->topIndex + 1;
};
int capacityOfQueue(Queue* queue) {
	return queue->s1->size;
};

void enqueue(Queue* queue, Element element) {
	push(queue->s1, element);
};

Element dequeue(Queue* queue) {
	assert(!isQueueEmpty(queue));

	while (!isStackEmpty(queue->s1)) {
		push(queue->s2, pop(queue->s1));
	}

	Element elem = pop(queue->s2);
	while (!isStackEmpty(queue->s2)) {
		push(queue->s1, pop(queue->s2));
	}

	return elem;
};
Element peek(Queue* queue) {
	assert(!isQueueEmpty(queue));

	while (!isStackEmpty(queue->s1)) {
		push(queue->s2, pop(queue->s1));
	}

	Element elem = top(queue->s2);
	while (!isStackEmpty(queue->s2)) {
		push(queue->s1, pop(queue->s2));
	}

	return elem;
};

int isLegalString(char str[]) {
	Stack* stack = initStack();
	Element current_var;
	int i = 0;

	while (str[i] != '\0') {
		if ((str[i] == '(') || (str[i] == '[') || (str[i] == '{')||(str[i]=='<')) {
			current_var.c = str[i];
			push(stack, current_var);
			i++;
			continue;
		}
		
		current_var.c = top(stack).c;
		switch (str[i]) {
		case ')':
			if (current_var.c != '(') {
				destroyStack(stack);
				return 0;
			}

			pop(stack);
			i++;
			continue;

		case '}':
			if (current_var.c != '{') {
				destroyStack(stack);
				return 0;
			}

			pop(stack);
			i++;
			continue;
		case ']':
			if (current_var.c != '[') {
				destroyStack(stack);
				return 0;
			}

			pop(stack);
			i++;
			continue;
		case '>':
			if (current_var.c != '<') {
				destroyStack(stack);
				return 0;
			}

			pop(stack);
			i++;
			continue;
		default:
			i++;
			continue;
		}
	}

	return isStackEmpty(stack);
};