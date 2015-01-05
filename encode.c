//Eva Fineberg
//Ruby Boyd
//encode.c
//This function creates a huffman tree and finds bitstrings for all chars and for the whole tree. It's pretty sweet.

#include"huffman.h"
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdio.h>
#include<limits.h>

//initializes the array which will contain the nodes with each char and its frequency.
int frequency[255];
struct Node* huffman;
int encode_count=0;
int list_count=0;
char* table[UCHAR_MAX+1];
char* temp;
char* binary;
int len;
FILE* fpp;
//This function creates nodes and checks for malloc errors.
struct Node* node_maker(int value, int index){
	struct Node* eva = (struct Node*)malloc(sizeof(struct Node));
	if(eva==NULL){
		printf("%s\n", "Malloc failed :(");
		exit(3);
	}
	eva->freq=value;
	eva->letter=index;
	eva->next=NULL;
	eva->right=NULL;
	eva->left=NULL;
	eva->bitstr=NULL;
	return eva;

}

//This function is passed a size_t of space to malloc
//It attempts to malloc the space and exits if malloc failes
//Otherwise returns a void pointer to the space malloc-ed
void* malloc_checker(size_t amt){
    void *v = malloc(amt);  
    if(!v){
        printf("%s\n", "Malloc failed :(");
        exit(3);
    }
    return v;
}





//This function inserts nodes into a linked list according to their frequencies in ascending order.
struct Node* insert(struct Node* node){
	struct Node* curr= huffman;
	struct Node* checked=NULL;
	struct Node* list = huffman;
	if(huffman==NULL){
		return node;
	}

	while(node->freq > curr->freq){
		if(curr->next!=NULL){
			checked=curr;
			curr=curr->next;	
		}	
		else{
			curr->next=node;
			return list;
		}
	}
	node->next=curr; //if its less than or equal put at beginning
	if(checked!=NULL){
		checked->next=node;
	}
	else{
		list=node;
	}
	return list;	
} 

//This function returns the number of nodes in a list
void count_list(){
	list_count=0;
	struct Node* curr=huffman;
	while(curr->next!=NULL){
	    list_count++;	
	    curr=curr->next;
	}
	list_count++;

}

//This function prints the list
void print_list(){
	struct Node* curr=huffman;
	while(curr->next!=NULL){
		printf("%c %d",curr->letter, curr->freq);
		curr=curr->next;
	}
	printf("%c %d\n", curr->letter, curr->freq);

	
	

}


//This function appends chars to the bitstring paths.
char* append(char* s, char c){
        len = 0;
	len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
	return s;
}

//This function removes chars from the bitstring paths.
char* shorten(char* s){
	int len=strlen(s);
	s[len-1]='\0';
	return s;

}

//This function traverses the tree to find the bitstring paths for each node in the tree.
void tree_traversal(struct Node* node, char* ret){
	struct Node* root=node;
	if(root->left == NULL && root->right == NULL){
		temp=malloc_checker(sizeof(char)*(UCHAR_MAX+1));
		strcpy(temp, ret);
		root->bitstr =temp;
		table[root->letter]=temp;
	}else{
		encode_count++;

		ret=append(ret, '0');
		char* tempLeft=malloc_checker(sizeof(char)*(UCHAR_MAX+1));
		strcpy(tempLeft,ret);
		tree_traversal(root->left, tempLeft);

		ret=shorten(ret);
		ret=append(ret, '1');
		free(tempLeft);

		char* tempRight = malloc_checker(sizeof(char)*(UCHAR_MAX+1));
		strcpy(tempRight,ret);
		tree_traversal(root->right, tempRight);

		ret=shorten(ret);
		free(tempRight);
	}
}

//This function converts a char to its binary representation.
char* tobinary(int d){
	int a, b;
	binary = malloc_checker((UCHAR_MAX+1)*sizeof(char));
	binary[0]='\0';
	for(a=7;a>=0; a--){
		b = d>>a;
		if(b & 1){
			binary=append(binary, '1');
		}
		else{
			binary=append(binary, '0');
		}
	}
	return binary;


}

//This is called by print_string, it takes in a file and a char 
//and prints to the file
void print_bit(FILE* f, char c){
	static int calls=0;
	static int acc=0;
	acc=(acc*2)+(c-'0');
	calls++;
	if(calls==CHAR_BIT){
		fprintf(f,"%c",acc);
		acc=0;
		calls=0;
	}


}
//pring_string takes in a file and a string and calls print_bit
//This will then print the bit_representation of the string
void print_string(FILE* f, char* s){
	int i =0;
	while(s[i]!='\0'){
		print_bit(f, s[i]);
		i++;

	}




}
//This function does a preorder traversal of the tree to produce the bitstring of the entire tree.
void pre_order(struct Node* node){
	
	if(node->left==NULL && node->right == NULL){
		print_string(fpp,"1");
		binary=tobinary(node->letter);
		print_string(fpp, binary);
		free(binary);
	}
	else{
		print_string(fpp, "0");
		pre_order(node->left);
		pre_order(node->right);
	}
	
	
}

//This function removes nodes
struct Node* remove_node(struct Node* node1, struct Node* node2){
	huffman=node2->next;
	node1->next=NULL;
	node2->next=NULL;
	return huffman;	
	
}


//This function constructs the huffman tree from the linked list.
struct Node* make_tree(){
	struct Node* curr = huffman;
	count_list();
	while(list_count>=2){
		curr=huffman;
		int sum = curr->freq + curr->next->freq;
		struct Node* n = node_maker(sum,122);
		n->left=curr;
		n->right=curr->next;
		huffman=remove_node(curr, curr->next);
		huffman=insert(n);
		list_count--;


	}
	return huffman;


}


//This function goes through the tree and frees up all the nodes
//made by node_maker
void destroy(struct Node* node){
	struct Node* curr=node;
	if(curr==NULL){
		return;
	}
	else{
		destroy(curr->left);
		destroy(curr->right);	
	}
	free(curr->bitstr);
	free(curr);


}

//This function opens a file and reads it in.
//It creates nodes for each letter and calculates the frequency of that letter. 
//It inserts nodes according to frequency.
//It writes the tree bitstring to the file.

int main(int argc, char* argv[]){
	const char* file=argv[1];
	int x;
	int freq_count=0;
	FILE* fp = fopen(file,"r");
	while((x=fgetc(fp))!=EOF){
		frequency[x]=frequency[x]+1;
		freq_count++;
	}
	
	fclose(fp);


	struct Node* end = node_maker(1,-1);
	end->next=NULL;
	end->left=NULL;
	end->right=NULL;
	huffman=end;
	for(int i=0; i<255; i++){
		if(frequency[i]!=0){
			struct Node* temp_node=node_maker(frequency[i], i);
			huffman=insert(temp_node);
		}
	}
	huffman=make_tree();
	char ret[CHAR_BIT+1];
	ret[0]='\0';
	tree_traversal(huffman, ret);
	
	fp = fopen(file,"r");

	fpp=stdout;
	
	if(argc > 2){
		const char* f = argv[2];
		fpp=fopen(f, "w");
	}
	pre_order(huffman);;


	print_string(fpp,end->bitstr);
	while((x=fgetc(fp))!=EOF){
		print_string(fpp,table[x]);
	}
	print_string(fpp, end->bitstr);
	for(int h=0; h<CHAR_BIT-1; h++){
		print_bit(fpp,'0');
	}
	fclose(fpp);
	fclose(fp);
	//free up space here	
	destroy(huffman);
	return 0;


}
