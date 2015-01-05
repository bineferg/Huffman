//Eva Fineberg Ruby Boyd
//huffman.h


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#ifndef HUFF_H
#define HUFF_H

//This in the constructor for a node object.
//Nodes have next pointers to allow them to function as elements in a linked list.
//Nodes also have right and left pointers to allow them to be nodes in a tree.
//Nodes also contain attributes which represent the frequency of the char, the char itself and the bitstring which is the path taken in the huffman tree to get to that node.
typedef struct Node{
	int freq;
	int letter;
	char* bitstr;
	struct Node* next;
	struct Node* left;
	struct Node* right;

}HUFF;
#endif
