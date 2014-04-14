/**
 * Actually each node should be a hashtable. The key is sector code,
 * the value is a pointer to the next hashtable for that sector.
 *
 * This will provide us with constant lookuptimes per hash table, and fixed
 * performance of O(n), where n is the number of codes provided.
 */
struct node {
	struct node *children;
	int *indices;
	unsigned int count;

	short value; // if -1 check indices
};

void tree_init(struct node *root)
{
	// C99. root is initialized, with all values to 0
	*root = (struct node){};
}

struct node* tree_find(struct node *root, short value)
{
	if(root->value == value) {
		return root;
	} else if (root->value >= 0){
		for(int i= 0; i < root->count; i++) {
			struct node *res = tree_find(root->children[i]);
			if(res) {
				return res;
			}
		}
	}

	return 0;
}

struct node tree_new_node(short value)
{
	struct node n = {.children = 0, .indices = 0, .value = value};
	return n;
}

struct node tree_add_node(struct node *parent, short value)
{
//	parent
	struct node n = {.children = 0, .indices = 0, .value = value};

	return n;
}




void add_node(char code[10], int idx)
{
	#define STEP 2
	char buff[STEP + 1];

	for(int i = 0; i <= 8; i += STEP) {
		memcpy(&buff, &(code[i]), STEP);
		buff[STEP] = '\0';
		int c = atoi(&buff);
	}
}
