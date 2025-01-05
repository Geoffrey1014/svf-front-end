int main() {
    int* a[5][7];
	return 0;
}	
	
	// int* source[2] = {&a, &b};

	// int* source[2]: source is an array of 2 elements, each element in the array is a pointer to an int (int*)
	// &a and &b give the memory addresses of a and b, which are int variables.
	// &a, &b are the addresses of a and b
	// You cannot assign the address of a variable to itself directly, but you can store it in a pointer.
	// int source[2]* = { &a, &b, }; is not valid C++ syntax

	// |--init_declarator: 		
    // |--array_declarator
    //   |--pointer_declarator(*)
    //     |--id: source
    //   |--NumberLiteral
    //     |--value: 2
	
    // |--pointer_declarator(*)
    //     |--id: source

	// |--init_declarator: 		
    //  |--pointer_declarator(*)
	//		|--array_declarator
    //     		|--id: source
    //   	|--NumberLiteral
    //     		|--value: 2
	// int *p;
	// p = &a;

	// int* x = source[1];
	// svf_assert(x==&b);