/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "framework.h"
#include "testing.h"

//vraci odkaz na promenou
Data *findInFrame(char *name, StackFrame *sf) {
	for (unsigned int i = 0; i < sf->size; i++) {
		//printf("compare: %s - %s, %d\n", name, sf->data[i].name, strcmp(name, sf->data[i].name));
		if (!strcmp(name, sf->data[i].name)) { 
			//printf("have match \n");
			return &(sf->data[i]); }
		//printf("not same\n");
	}
		
	for (unsigned int i = 0; i < sf->top->size; i++) {
		//printf("compare: %s - %s, %d\n", name, sf->data[i].name, strcmp(name, sf->data[i].name));
		if (!strcmp(name, sf->top->data[i].name)) {
			//printf("have match \n");
			return &(sf->top->data[i]);
		}
		//printf("not same\n");
	}

	return NULL;
}

//vklada novy ramec na VRCHOL,
StackFrame *newFrame(StackFrame *parent, TsTree root, char *name, Data *ret, tInstrListItem *lastActive) {
	debug(" [FRAMEWORK] Creating new frame: %s\n",name);

	TsTree tsTree = tsFind(root, name);

	int size = tsTree->varCount + 8; // pocet promennych, dohledat v TS
	StackFrame *sf = malloc(sizeof(StackFrame));
	if (sf == NULL) return NULL;

	sf->data = malloc(sizeof(Data)*size);
	if (sf->data == NULL) {
		free(sf);
		return NULL;
	}

	sf->identifier = makeString(name);
	sf->parent = parent;
	sf->top = parent->top;
	sf->lastActive = lastActive;
	sf->ret = ret;
	sf->size = size;
	sf->child = NULL;
	
	debug(" [FRAMEWORK] Add vars by hash table\n");
	hashWriteOut(tsTree->ts);
	
	for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++) {
		//sloupec
		if (tsTree->ts[i].key != NULL) {
			for (struct hItem *item = &(tsTree->ts[i]); item != NULL; item = item->next) {
				if (item->type[0] == 'V') { // pokud je pormenna
					debug(" [FRAMEWORK] Add to frame..  id: %s, type: %s\n", item->key, item->type);
					sf->data[item->index].name = makeString(item->key);// jmeno promenne
					sf->data[item->index].defined = false;
					switch (item->type[1]) { //datovy typ
					case 'I':
						sf->data[item->index].type = t_int;
						break;
					case 'D':
						sf->data[item->index].type = t_double;
						break;
					case 'S':
						sf->data[item->index].type = t_string;
						sf->data[item->index].value.v_string = NULL;
						break;
					default: break;
					}
				}
				
			}
		}
	}

	//tmp vars
	int si = tsTree->varCount;
	sf->data[si].name = makeString("#tmpI1");
	sf->data[si].defined = false;
	sf->data[si++].type = t_int;
	sf->data[si].name = makeString("#tmpI2");
	sf->data[si].defined = false;
	sf->data[si++].type = t_int;
	sf->data[si].name = makeString("#tmpI3");
	sf->data[si].defined = false;
	sf->data[si++].type = t_int;

	sf->data[si].name = makeString("#tmpD1");
	sf->data[si].defined = false;
	sf->data[si++].type = t_double;
	sf->data[si].name = makeString("#tmpD2");
	sf->data[si].defined = false;
	sf->data[si++].type = t_double;
	sf->data[si].name = makeString("#tmpD3");
	sf->data[si].defined = false;
	sf->data[si++].type = t_double;

	sf->data[si].name = makeString("#tmpS1");
	sf->data[si].value.v_string = NULL;
	sf->data[si].defined = false;
	sf->data[si++].type = t_string;

	sf->data[si].name = makeString("#tmpS2");
	sf->data[si].value.v_string = NULL;
	sf->data[si].defined = false;
	sf->data[si++].type = t_string;

	debug("[FRAMEWORK] Created frame:\n");
	testWriteOutFrame(sf);

	return sf;
}

//odstanuje posledni ramec ze zasobniku
StackFrame *deleteFrame(StackFrame *sf) {
	debug("[FRAMEWORK] Deleting frame: %s\n",sf->identifier);
	StackFrame *tmp = sf->parent;
	if(tmp!=NULL)
		sf->parent->child = sf->child;

	testWriteOutFrame(sf);

	for (unsigned int i = 0; i < sf->size; i++) {
		//debug("[FRAMEWORK] Deleting %s", sf->data[i].name);
		free(sf->data[i].name);
		if (sf->data[i].type == t_string && sf->data[i].value.v_string)
			free(sf->data[i].value.v_string);
	}

	free(sf->identifier);
	free(sf->data);
	free(sf);
	debug("[FRAMEWORK] Frame Deleted\n");
	return tmp; 
}

StackFrame *newTopFrame(TsTree root) {
	int size = 0;
	debug(" [FRAMEWORK] Creating new frame: %s\n", "#StaticVariables");

	for (TsTree x = root; x != NULL;x=x->next) {
		size+=x->varCount;
	}

	StackFrame *sf = malloc(sizeof(StackFrame));
	if (sf == NULL) return NULL;

	sf->data = malloc(sizeof(Data)*size);
	if (sf->data == NULL) {
		free(sf);
		return NULL;
	}

	sf->identifier = makeString("#StaticVariables");
	sf->lastActive = NULL;
	sf->top = sf;
	sf->parent = NULL;
	sf->child = NULL;
	sf->ret = NULL;
	sf->size = size;


	int index = 0;

	for (TsTree x = root; x != NULL; x = x->next) {
		debug(" [FRAMEWORK] Searching in %s\n", x->name);
		hashWriteOut(x->ts);
		for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++) {
			//sloupec
			if (x->ts[i].key != NULL) {
				for (struct hItem *item = &(x->ts[i]); item != NULL; item = item->next) {
					if (item->type[0] == 'V') { // pokud je pormenna
						char *tmp = cat(x->name,".");
						char *fullname = cat(tmp,item->key);
						free(tmp);
						debug(" [FRAMEWORK] Add to frame..  id: %s, type: %s\n", fullname, item->type);
						sf->data[index].name = fullname;// jmeno promenne
						sf->data[index].defined = false;
						switch (item->type[1]) { //datovy typ
						case 'I':
							sf->data[index].type = t_int;
							break;
						case 'D':
							sf->data[index].type = t_double;
							break;
						case 'S':
							sf->data[index].type = t_string;
							sf->data[index].value.v_string = NULL;
							break;
						default: break;
						}
						index++;
					}
				}
			}
		}
	}
	return sf;
}