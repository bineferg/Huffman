//Eva Fineberg
//Ruby Boyd
//decode.c
//This program reads in text that has been encodes and writes the decoded version to a file or stdout.

#include "huffman.h"
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdio.h>
#include<limits.h>

char str[CHAR_BIT];
struct Node* tree;
char encode_array[UCHAR_MAX+1];
char end_array[CHAR_BIT+1];
int read_bit(FILE* f);

//This is the function that makes a node and returns it.
//It sets next, right, and left initially to null.
//If we malloc space for a node and malloc fails it prints to stdout and exits with a nonzero value.
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









//This function takes in the file* and reads bits
//It converts the btis into a string and returns the string.
char* make_string(FILE* f){
	char* str = malloc_checker(sizeof(char));
	for(int i=0; i<CHAR_BIT; i++){
		str[i]=read_bit(f)+'0';
	}
	return str;	
	
	
}

//Once we know we have read a leaf during the building of the tree
//This function reads the next 8 bits and converts it into an int character.
int read_leaf(FILE* f){
	
	int acc=0;
	for(int i=0; i<CHAR_BIT; i++){
		acc=(acc<<1)+read_bit(f);
	}
	return acc;



}

//This function takes in a char* to an array of 1s and 0s
//and converts it to a dcimal number and returns it.
int todecimal(char* binary){
	int i=CHAR_BIT-1;
	int j=1;
	int decimal=0;
	while(i>=0){
		decimal = decimal + (binary[i]-'0')*j;
		j=j*2;
		i--;
	}


	return decimal;


}

//While reading in the pre_order traversal sequence of the file
//This function builds a huffman tree from the sequence and returns it.
struct Node*  build_tree(FILE* f){
	if(read_bit(f)==1){
		int letter = read_leaf(f);
		//if(letter==255){
		   // letter=-1;   
		//}
		struct Node* n = node_maker(0, letter);
		//printf("%d\n",n->letter);
		return n;		

	}else{
	    struct Node* n = node_maker(0,-9);
	    n->left = build_tree(f);
	    n->right = build_tree(f);
	    return n;
	}	
	
}

//This takes in a string and a char and appends the char to the string.
char* append(char* s, char c){ 
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
        return s;
}

//This function checks if the node n we are currently at is a leaf.
//If it is a leaf it returns 1, otherwise returns 0.
int isLeaf(struct Node* n){
	if(n->left==NULL && n->right==NULL){
		return 1;
		
	}
	else{
		return 0;
	
	}


}
//This function takes in a pointer to a node and frees up all nodes
//that were used to make the huffman tree during the build_tree function.
void destroy(struct Node* node){
         struct Node* curr=node;
         if(curr==NULL){
                 return;
         }
         else{
                 destroy(curr->left);
                 destroy(curr->right);
         }
         free(curr);
 
 
 }


//This function finds the node which represents the EOF character and sets its value to EOF. 
//This is necessary for the next part of decode, in which reaching this node indicates the end of the traversal.
void findEnd(FILE* f){
	struct Node* root=tree;
	while(isLeaf(root)==0){
		if(read_bit(f)==1){
			root = root->right;

		}
		else{
			root = root->left;
		}

	}
	root->letter=EOF;

}

//This function traverses the tree and writes the character values of each node in the traversal to the file.
void findText(FILE* input, FILE* output){
	struct Node* root=tree;
	while(1){
		if(isLeaf(root)==1){
			if((root->letter)==EOF){
				break;
			} else {
				fprintf(output,"%c",root->letter);
				root=tree;	
			}

		} else { 
		    int x = read_bit(input);
		    if(x==1){
			root = root->right;

		    }
		    else{
			    root = root->left;

		    }
		}
	}
}

//Test for read_bit
void test(FILE* f){
	char x;
	while((x=read_bit(f))!=EOF){
		printf("%c",x);
	}



}

//This function reads in a character from the input text.
//Then it returns 1 bit of that character (0 or 1) each time it is called.
//When all the bits in the char have been read, it reads in another character.
int read_bit(FILE* f){
	static int calls=0;
	static int acc;
	static int first = 1;
	if(first==1){
		acc=fgetc(f);
		first=0;
	}
	if(acc==EOF){
		return EOF;

	}
	if(calls==CHAR_BIT){
		acc=fgetc(f);
		if(acc==EOF){
			return EOF;
		}
		calls=0;
	}
	int offset = CHAR_BIT - calls -1;
	int mask = 1 << offset; 
	int bit = ((acc & mask)>>offset);	
	calls++;
	return bit;

}

//main gets input from user. 
//It calls all functions necessary to decode the file.
int main(int argc, const char* argv[]){
	if(argc<2){
		printf("%s\n", "Please enter a file next time");	
		exit(1);
	}	

	FILE* file = fopen(argv[1], "r");

	FILE* out = stdout;
	
	if(argc>2){
		out=fopen(argv[2], "w");
	}//test();
	tree=build_tree(file);
	findEnd(file);
	findText(file,out);
	fclose(file);
	destroy(tree);
	return 0;
}
