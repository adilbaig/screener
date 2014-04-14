hashtable:
	gcc -g -Wall -std=c11 companies/hashtable.c -obin/hashtable
	
company_tree:
	gcc -g -Wall -std=c11 companies/main.c src/vector.c -obin/company_tree
	
screener:
	gcc -g -Wall -std=c99 src/*.c -obin/screener
