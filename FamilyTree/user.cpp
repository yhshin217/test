#define MAX_FAMILY_TREE_LENGTH 30000
#define MAX_NAME_LENGTH 12

#define NAMELEN 12
#define HASHTABLE 545

struct ENTRY
{
	int type;
	char name[NAMELEN];
	int nameLen;
	char bfname[NAMELEN];
	int bfnameLen;
	int childNo;
	ENTRY* firstChild;
	ENTRY* lastChild;
	ENTRY* preSibling;
	ENTRY* nextSibling;
	ENTRY* parent;
	ENTRY* preHash;
	ENTRY* nextHash;
};

ENTRY entry[200];
ENTRY* hashTable[HASHTABLE];
ENTRY* root;
int count=0;
char* mPtr;
int tSize;

int strcpy(char* src, char* dst)
{
	int ret = 0;
	while (*dst != 0){
		*src++ = *dst++;
		ret++;
	}
	return ret;
}

int strcmp(char* src1, char* src2)
{
	while (*src1 != 0)
	{
		if (*src1 != *src2) {
			return 0;
		}
		src1++;
		src2++;
	}
	if ((*src1 == 0) && (*src2 == 0)) {
		return 1;
	}
	else {
		return 0;
	}
}

ENTRY* getNode(void)
{
	entry[count].type = 1;
	entry[count].nameLen = entry[count].bfnameLen = entry[count].childNo = 0;
	entry[count].firstChild = entry[count].lastChild = entry[count].preSibling = entry[count].nextSibling = \
	entry[count].parent = entry[count].preHash = entry[count].nextHash = 0;
	return &entry[count++];
}

void addHash(ENTRY* node)
{
	unsigned long long hash = 0;
	unsigned long long mul = 1;
	for (int i = 0 ; i < NAMELEN, (node->name[i] != 0) ; i++, mul *= 26) {
		hash += (node->name[i] - 'A') * mul;
	}

	int hashT = (int)(hash % HASHTABLE);
	if (hashTable[hashT] == 0) {
		hashTable[hashT] = node;
	}
	else {
		node->nextHash = hashTable[hashT];
		hashTable[hashT]->preHash = node;
		hashTable[hashT] = node;
	}
}

ENTRY* getHash(char name[NAMELEN])
{
	unsigned long long hash = 0;
	unsigned long long mul = 1;
	for (int i = 0 ; i < NAMELEN, (name[i] != 0) ; i++, mul *= 26) {
		hash += (name[i] - 'A') * mul;
	}

	int hashT = (int)(hash % HASHTABLE);
	ENTRY* entry = hashTable[hashT];
	while (!strcmp(entry->name, name)) {
		entry = entry->nextHash;
	}
	return entry;
}

void parseDFS(ENTRY* node)
{
	int i;
	ENTRY* tNode = getNode();
	tNode->parent = node;
	if (node->firstChild == 0){
		node->firstChild = node->lastChild = tNode;
	}
	else if (node->firstChild == node->lastChild) {
		tNode->preSibling = node->firstChild;
		node->firstChild->nextSibling = tNode;
		node->lastChild = tNode;
	}
	else {
		tNode->preSibling = node->lastChild;
		node->lastChild->nextSibling = tNode;
		node->lastChild = tNode;
	}
	tNode->type = *mPtr++;
	mPtr++;
	tNode->nameLen = *mPtr++;
	for (i=0 ; i < tNode->nameLen ; i++) {
		tNode->name[i] = *mPtr++;
	}
	tNode->name[i] = 0;

	addHash(tNode);

	if (tNode->type == 1) {
		mPtr++;
		tNode->bfnameLen = *mPtr++;
		for (i=0 ; i < tNode->bfnameLen ; i++) {
			tNode->bfname[i] = *mPtr++;
		}
		tNode->bfname[i] = 0;
	}
	else {
		mPtr++;
		tNode->childNo = *mPtr++;
		for (i = 0 ; i < tNode->childNo ; i++) {
			parseDFS(tNode);
		} 
	}
}

void parse(char orgFamilyTree[MAX_FAMILY_TREE_LENGTH], int size) {
	count = 0;
	root = getNode();
	mPtr = orgFamilyTree;
	tSize = size;
	parseDFS(root);
}

void change(char name[MAX_NAME_LENGTH], char bfName[MAX_NAME_LENGTH]){
	ENTRY* entry = getHash(name);
	entry->nameLen = strcpy(bfName, entry->name);
}

void registeration(char parent[MAX_NAME_LENGTH], char child[MAX_NAME_LENGTH], char childBfName[MAX_NAME_LENGTH]){
	ENTRY* entry = getHash(parent);
	ENTRY* childNode = getNode();
	childNode->nameLen = strcpy(child, childNode->name);
	childNode->bfnameLen = strcpy(childBfName, childNode->bfname);
	childNode->childNo++;

	addHash(childNode);

	childNode->preSibling =	entry->lastChild;
	entry->lastChild->nextSibling = childNode;
	entry->lastChild = childNode;
}

void remove(char name[MAX_NAME_LENGTH]){
	ENTRY* entry = getHash(name);
	entry->parent->childNo--;
	if (!entry->preSibling && !entry->nextSibling) {
		entry->parent->firstChild = entry->parent->lastChild = 0;
	}
	else if (!entry->preSibling && entry->nextSibling) {
		entry->parent->firstChild = entry->nextSibling;
	}
	else if (entry->preSibling && !entry->nextSibling) {
		entry->parent->lastChild = entry->preSibling;
	}
	else {
		entry->preSibling->nextSibling = entry->nextSibling;
		entry->nextSibling->preSibling = entry->preSibling;
	}
}

int retFamily;
char* retPtr;

void makeTree(ENTRY* entry)
{
	if ((entry->firstChild == 0) && (entry->lastChild == 0)) {
		*retPtr++ = 1;
	}
	else {
		*retPtr++ = 2;
	}
	retPtr++;
	*retPtr++ = entry->nameLen;
	strcpy(entry->name, retPtr);
	retPtr += entry->nameLen;

	if ((entry->firstChild == 0) && (entry->lastChild == 0)) {
		retPtr++;
		*retPtr++ = entry->bfnameLen;
		strcpy(entry->bfname, retPtr);
		retPtr += entry->bfnameLen;
	}
	else {
		retPtr++;
		*retPtr++ = entry->childNo;
		ENTRY* child = entry->firstChild;
		for (int i = 0 ; i < entry->childNo ; i++) {
			makeTree(child);
			child = child->nextSibling;
		}
	}
}

int familyTree(char name[MAX_NAME_LENGTH], char newFamilyTree[MAX_FAMILY_TREE_LENGTH]) {
	ENTRY* entry = getHash(name);
	retPtr = newFamilyTree;

	makeTree(entry);

	return (retPtr - newFamilyTree);
}