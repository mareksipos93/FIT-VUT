/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "interpret.h"
#include "testing.h"

void interpret(tInstrList iList,TsTree *root,labelAdress *la) {
	debug("[INTERPRET] Staring interpret...\n");
	tsWriteOutTreeTS(*root);
	//stack pro vnitrni mezi vypocty a parametry fci
	Stack interStack = stackInit(100);

	tInstr ins; // aktualni instrukce

	// tabulka ramcu
	StackFrame *sf = newTopFrame(*root);
	
	sf = newFrame(sf,*root,"Main.run",NULL,NULL);

	debug("[INTERPRET] Content of TOP FRAME\n");
	testWriteOutFrame(sf->top);
	//printf("### END TEST SF wOut\n");

	

	Operand *dest = NULL, *src1 = NULL, *src2 = NULL;
	char *tmpStr1,*tmpStr2;
	Data tmpData;
	int tmpInt;
	double tmpDouble;


	testWriteOutInstr(iList);
	instrListSetActiveFirst(&iList);


	debug("[INTERPRET] Content of FRAME before first execute\n");
	testWriteOutFrame(sf);

	// prochazim seznam instrukci az do konce
	while (!instrListGetActiveInstr(&iList, &ins)) {	
		
		debug("[INTERPRET] Instruction execute: \n ");
		testWriteOutI(ins);

		dest = src1 = src2 = NULL;
		tmpStr1 = tmpStr2 = NULL; // reset adres


		int destT = 0, src1T = 0, src2T = 0;
		// Priradim zdroje a cile
		if (ins.instr == I_LABEL || ins.instr == I_CALL) {
			//none
		}
		else if (ins.addr1 == NULL) { //only return
			
		}
		else if (ins.addr2 == NULL && ins.addr3 == NULL) { // dest NULL NULL
			dest = ins.addr1;
			src1 = dest;
			destT = getType(dest);
			src1T = destT;
		}
		else if (ins.addr3 == NULL) { // dest/src1 src2 NULL
			dest = ins.addr1;
			src1 = dest;
			src2 = ins.addr2;

			destT = getType(dest);
			src1T = destT;
			src2T = getType(src2);
		}
		else { // src1 src2 dest
			dest = ins.addr3;
			src1 = ins.addr1;
			src2 = ins.addr2;

			if(ins.instr<I_JMP && ins.instr>I_JMPGE)
			destT = getType(dest);
			src1T = getType(src1);
			src2T = getType(src2);
		}

		

//ENDTODO

		//printf("switch");
		switch (ins.instr) {
//Zakladni operace
			//ADD
			break;
		case I_MOV:
			findInFrame(dest->value.name, sf)->defined = true; // definuju dest
			if(src2->type != name || findInFrame(src2->value.name, sf)->defined == true) //byl src definovany? 
				switch (destT) {
					case t_int:
						findInFrame(dest->value.name, sf)->value.v_int = getIntVar(src2);
						break;
					case t_double:
						findInFrame(dest->value.name, sf)->value.v_double = byType(src2);
						break;
					case t_string:
					//TODO nema se udelat kopie retezce??
						findInFrame(dest->value.name, sf)->value.v_string = makeString((src2->type == name) ? findInFrame(src2->value.name, sf)->value.v_string : src2->value.v_string);
						break;
					default: break;
				}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_CALL:
			//kam ulozit return? na stack? nebo si necham tady v promenne adresu a pri ret to tam poslu
			//vytvorim novy ramec
			//prepnu se do neho
			//nactu paramety
			//continue a pokracuju v instrukcich 

			//TODO non-void bez returnu!
			sf = newFrame(sf, *root,  ins.addr1->value.name,ins.addr3? findInFrame(ins.addr3->value.name,sf) : NULL, iList.active);
			debug("[INTERPRET] Switched to frame: %s",sf->identifier);
			testWriteOutFrame(sf);
			instrListSetActive(&iList, tsFind(*root, ins.addr1->value.name)->addr);
			debug("[INTERPRET] Looking for params\n");
			//nacitani parametru
			int cpar = 0;
			for (TsTree x = *root; x != NULL; x = x->next)
				if (isHisParent(sf->identifier, x->name)) {
					cpar = strlen(searchInHashTable(x->ts, sf->identifier)->type) - 2;
					break;
				}
			debug("[INTERPRET] found %d params for %s\n", cpar, sf->identifier);

			for (int i = cpar - 1; i >= 0; i--) {
				stackPop(&interStack, &tmpData);
				debug("[INTERPRET] Add: %d\n", tmpData.type);
				sf->data[i].value = tmpData.value;
				sf->data[i].type = tmpData.type;
				sf->data[i].defined = true;
			}
			continue;

			break;
		case I_RET:
			//vlozim do returnu
			// odstnim ramec
			//prepnu se z5
			//pokud jsem v NULL tak je konec Main.run
			if (ins.addr1 && sf->ret) {
				sf->ret->defined = true;
				switch (destT) {
				case t_int:
					sf->ret->value.v_int = getIntVar(src1);
					break;
				case t_double:
					sf->ret->value.v_double = byType(src1);
					break;
				case t_string:
					sf->ret->value.v_string = makeString((dest->type == name) ? findInFrame(dest->value.name, sf)->value.v_string : dest->value.v_string);
					break;
				default: break;
				}
			}
			
			instrListSetActive(&iList,sf->lastActive); // pokud je konec Main.run tak je to null a smycka konci
			if (sf->parent != sf->top) {
				debug("[INTERPRET] Switched to frame: %s", sf->parent->identifier);
			}
			else {
				debug("[INTERPRET] Switched out of Main.run -> End \n");
			}
			sf = deleteFrame(sf); // navraceni do otcovskeho ramce		
			break;
		case I_PUSH:
			if (dest->type != name || findInFrame(dest->value.name, sf)->defined == true)
				switch (destT){
					case t_int: 
					case t_double: 
						tmpData = *findInFrame(dest->value.name, sf);
						stackPush(&interStack,tmpData);
						break;
					case t_string: 
						tmpData = *findInFrame(dest->value.name, sf);
						tmpData.value.v_string = makeString(tmpData.value.v_string); // novou kopii 
						stackPush(&interStack, tmpData); // vlozim na stack
						break;
					default:
					break;
				}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_POP:
			findInFrame(dest->value.name, sf)->defined = true;
			switch (destT){
				case t_string:
					if (dest->value.v_string) free(dest->value.v_string); // prepisovani puvodniho retezce
				case t_int:
				case t_double:
				 // retezec je vytvoren pres push, nemusim tvorit novy 
					stackPop(&interStack,&tmpData);
					findInFrame(dest->value.name, sf)->value = tmpData.value; // NERUCIM za spravnost 
				default:
					break;
			}
			break;

		case I_LABEL: 
			//none		
			break;
//JMPs
		case I_JMP: 
			instrListSetActive(&iList, getLabelAdress(la,src1->value.v_string));
			break;
		case I_JMPE: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if(byType(src1) == byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_JMPNE: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if (byType(src1) != byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}break;
		case I_JMPL: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if (byType(src1) < byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_JMPLE: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if (byType(src1) <= byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_JMPG: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if (byType(src1) > byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;
		case I_JMPGE: 
			if ((src1->type != name || findInFrame(src1->value.name, sf)->defined == true) && (src2->type != name || findInFrame(src2->value.name, sf)->defined == true)) {
				if (byType(src1) >= byType(src2))
					instrListSetActive(&iList, getLabelAdress(la, dest->value.v_string));
			}
			else {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			break;


//Matematicke operace
		case I_ADD:
			// ? src2 ? // src2 je vzdy
			if (src2 && src2->type == name && findInFrame(src2->value.name,sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			} // src1 src2 dest
			if (src1 != dest && src1->type == name && findInFrame(src1->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}	// dest src2
			else if (findInFrame(dest->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			findInFrame(dest->value.name, sf)->defined = true;

			//test pro dvou adresni

			//typ destinace
			switch (destT) { 
				case t_int: // int + int/double only
					//printf("intOP\n");
					if(dest != src1){ // nepotrebuju delat kopii toho sameho
						findInFrame(dest->value.name, sf)->value.v_int = getIntVar(src1);
					}
					//printf("%d+%d\n", findInFrame(dest->value.name, sf)->value.v_int, byType(src2));

					findInFrame(dest->value.name, sf)->value.v_int += getIntVar(src2);

					//printf("%d+%d\n", findInFrame(dest->value.name, sf)->value.v_int, byType(src2));
					break;
				case t_double: 
					//printf("bouble\n");
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_double = byType(src1);
					}
					findInFrame(dest->value.name, sf)->value.v_double += byType(src2);
					break;
				case t_string: // konkatenace retezcu str + str/int/double
					
					//konverze dat
					if (src1T == t_string) {
						tmpStr1 = (src1->type == name) ? findInFrame(src1->value.name, sf)->value.v_string : src1->value.v_string;
					}
					else if (src1T == t_int) {
						debug(" [INTERPRET] Converting int1 to string");
						tmpStr1 = intToString((src1->type == name) ? findInFrame(src1->value.name, sf)->value.v_int : src1->value.v_int);
					}
					else {
						debug(" [INTERPRET] Converting double1 to string");
						tmpStr1 = doubleToString((src1->type == name) ? findInFrame(src1->value.name, sf)->value.v_double : src1->value.v_double);
					}

					if (src2T == t_string) {
						
						tmpStr2 = (src2->type == name) ? findInFrame(src2->value.name, sf)->value.v_string : src2->value.v_string;
					}
					else if (src2T == t_int) {
						debug(" [INTERPRET] Converting int2 to string");
						tmpStr2 = intToString((src2->type == name) ? findInFrame(src2->value.name, sf)->value.v_int : src2->value.v_int);
					}
					else {
						debug(" [INTERPRET] Converting double2 to string");
						tmpStr2 = doubleToString((src2->type == name) ? findInFrame(src2->value.name, sf)->value.v_double : src2->value.v_double);
					}
					findInFrame(dest->value.name, sf)->value.v_string = cat(tmpStr1,tmpStr2);
					//printf("stringOP\n");
					break;
				default: break;
			}

			break;
		case I_SUB:
			if (src2 && src2->type == name && findInFrame(src2->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			} // src1 src2 dest
			if (src1 != dest && src1->type == name && findInFrame(src1->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}	// dest src2
			else if (findInFrame(dest->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
				//ERR
			}
			findInFrame(dest->value.name, sf)->defined = true;
			switch (destT) {
				case t_int:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_int = getIntVar(src1);
					}
					findInFrame(dest->value.name, sf)->value.v_int -= getIntVar(src2);
					break;
				case t_double:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_double = byType(src1);
					}
					findInFrame(dest->value.name, sf)->value.v_double -= byType(src2);
					break;
				default: break;
			}
			break;
		case I_MUL:
			if (src2 && src2->type == name && findInFrame(src2->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			} // src1 src2 dest
			if (src1 != dest && src1->type == name && findInFrame(src1->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}	// dest src2
			else if (findInFrame(dest->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			findInFrame(dest->value.name, sf)->defined = true;
			switch (destT) {
				case t_int:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_int = getIntVar(src1);
					}
					findInFrame(dest->value.name, sf)->value.v_int *= getIntVar(src2);
					break;
				case t_double:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_double = byType(src1);
					}
					findInFrame(dest->value.name, sf)->value.v_double *= byType(src2);
					break;
				default: break;
			}
			break;
		case I_DIV:
			if (src2 && src2->type == name && findInFrame(src2->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			} // src1 src2 dest
			if (src1 != dest && src1->type == name && findInFrame(src1->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}	// dest src2
			else if (findInFrame(dest->value.name, sf)->defined == false) {
				clearAll(sf, root, interStack, &iList, la);
				error(ERR_RUN_INIT);
			}
			findInFrame(dest->value.name, sf)->defined = true;
			switch (destT) {
				case t_int:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_int = getIntVar(src1);
					}
					if (byType(src2) == 0) {
						clearAll(sf, root, interStack, &iList, la);
						error(ERR_RUN_DEV);
					}
					findInFrame(dest->value.name, sf)->value.v_int /= getIntVar(src2);
					break;
				case t_double:
					if (dest != src1) {
						findInFrame(dest->value.name, sf)->value.v_double = byType(src1);
					}
					if (byType(src2) == 0) {
						clearAll(sf, root, interStack, &iList, la);
						error(ERR_RUN_DEV);
					}
					findInFrame(dest->value.name, sf)->value.v_double /= byType(src2);
					break;
				default: break;
			}
			break;

		case I_INC:
			switch (destT) {
				case t_int:
					findInFrame(dest->value.name, sf)->value.v_int++;
					break;
				case t_double:
					findInFrame(dest->value.name, sf)->value.v_double++;
					break;
				default: break;
			}
			break;
		case I_DEC:
			switch (destT) {
				case t_int:
					findInFrame(dest->value.name, sf)->value.v_int--;
					break;
				case t_double:
					findInFrame(dest->value.name, sf)->value.v_double--;
					break;
				default: break;
			}
			break;

//Vstup - vystup
		case I_READ:
			findInFrame(dest->value.name, sf)->defined = true;
			switch(destT) {
				case t_int: 
					if (!readInt(&tmpInt)) { //TODO skonceni
						clearAll(sf, root, interStack, &iList, la);
						error(ERR_RUN_NUM);
					}
					findInFrame(dest->value.name, sf)->value.v_int = tmpInt;
					break;
				case t_double: 
					if (!readDouble(&tmpDouble)) {
						clearAll(sf, root, interStack, &iList, la);
						error(ERR_RUN_NUM);
					}
					findInFrame(dest->value.name, sf)->value.v_double = tmpDouble;
					break;
				case t_string: 
					findInFrame(dest->value.name, sf)->value.v_string = readString();
					break;
				default: break;
			}
			
		case I_IREAD:
			if (dest) {
				findInFrame(dest->value.name, sf)->defined = true;
				if (!readInt(&tmpInt)) { //TODO skonceni
					clearAll(sf, root, interStack, &iList, la);
					error(ERR_RUN_NUM);
				}
				findInFrame(dest->value.name, sf)->value.v_int = tmpInt;
			}
			break;
		case I_DREAD:
			if (dest) {
				if (!readDouble(&tmpDouble)) {
					clearAll(sf, root, interStack, &iList, la);
					error(ERR_RUN_NUM);
				}
				findInFrame(dest->value.name, sf)->value.v_double = tmpDouble;
			}
			break;
		case I_SREAD:
			if (dest) {
				if (findInFrame(dest->value.name, sf)->defined == true) {
					free(findInFrame(dest->value.name, sf)->value.v_string);
				}
				findInFrame(dest->value.name, sf)->defined = true;
				if (!readInt(&tmpInt)) { //TODO skonceni
					clearAll(sf, root, interStack, &iList, la);
					error(ERR_RUN_NUM);
				}
				findInFrame(dest->value.name, sf)->value.v_int = tmpInt;
			}
			break;

			break;
		case I_WRITE:
			tmpInt = stackPop(&interStack, &tmpData);
			debug("[OUTPUT %d] ",tmpInt);
			//TODO pretyp pak write

			switch (tmpData.type) {
			case t_int:
				printf("%d", tmpData.value.v_int);
				break;
			case t_double:
				printf("%g", tmpData.value.v_double);
				break;
			case t_string:
				printf("%s", tmpData.value.v_string);
				break;
			default: break;
			}

			debug("\n");


			break;
// Vestavene funkce dest je cil kam se uklada vysledek
		case I_LEN: // int lenght(String)
			findInFrame(dest->value.name,sf)->defined = true;
			stackPop(&interStack, &tmpData);
			findInFrame(dest->value.name,sf)->value.v_int = strlen(tmpData.value.v_string);
			break;
		case I_SUBS: // String substr(String s, int i, int n)
			findInFrame(dest->value.name, sf)->defined = true;
			stackPop(&interStack, &tmpData);
			int tmpInt2 = tmpData.value.v_int;
			stackPop(&interStack, &tmpData);
			int tmpInt1 = tmpData.value.v_int;
			stackPop(&interStack, &tmpData);
			tmpStr1 = tmpData.value.v_string;

			findInFrame(dest->value.name, sf)->value.v_string = getSubString(tmpStr1,tmpInt1,tmpInt2);
			break;
		case I_CMP: // int compare(String s1, String s2)
			findInFrame(dest->value.name, sf)->defined = true;
			stackPop(&interStack, &tmpData);
			tmpStr2 = tmpData.value.v_string;
			stackPop(&interStack, &tmpData);
			tmpStr1 = tmpData.value.v_string;
			
			findInFrame(dest->value.name, sf)->value.v_int = compare(tmpStr1, tmpStr2);
			break;
		case I_FIND: // int find(String s, String search) 
			findInFrame(dest->value.name, sf)->defined = true;
			stackPop(&interStack, &tmpData);
			tmpStr2 = tmpData.value.v_string;
			stackPop(&interStack, &tmpData);
			tmpStr1 = tmpData.value.v_string;
			
			findInFrame(dest->value.name, sf)->value.v_int = findSubstring(tmpStr1,tmpStr2);
			break;
		case I_SORT: // String sort(String s)
			if (findInFrame(dest->value.name, sf)->defined == true) {
				free(findInFrame(dest->value.name, sf)->value.v_string);
			}
			else findInFrame(dest->value.name, sf)->defined = true;

			stackPop(&interStack, &tmpData);
			//TODO prepisy
			findInFrame(dest->value.name, sf)->value.v_string = makeString(tmpData.value.v_string);
			tmpStr1 = findInFrame(dest->value.name, sf)->value.v_string;
			listMergeSort(tmpStr1);
			break;



			break;
		default: break;

		}

		debug("[INTERPRET] Content of FRAME after execute\n");
		testWriteOutFrame(sf->top);
		testWriteOutFrame(sf);

		instrListSetActiveNext(&iList); // posunu aktivitu na další 
	}//while ins
	debug("[INTERPRET] Ending interpret...\n");
	clearAll(sf,root,interStack,&iList,la);
	debug("[INTERPRET] End.\n");
}

// extrahuje parametry funkce
void extractParams(StackFrame *sf, TsTree root, Stack stack) {
	debug("[INTERPRET] Looking for params\n");

	int cpar = 0;
	for (TsTree x = root; x != NULL; x = x->next) 
		if (isHisParent(sf->identifier, x->name)) {
			cpar = strlen(searchInHashTable(x->ts, sf->identifier)->type) - 2;
			break;
		}
	debug("[INTERPRET] found %d params for %s\n", cpar,sf->identifier);

	Data tmp;

	for (int i = cpar-1; i >= 0; i--) {
		stackPop(&stack, &tmp);
		debug("[INTERPRET] Add: %d",tmp.type);
		sf->data[i].value = tmp.value;
		sf->data[i].defined = true;
	}
}

// Vycisti veskerou doposud alokovanou pamet 
void clearAll(StackFrame *sf, TsTree *root, Stack interStack, tInstrList *iList, labelAdress *la) {
	//docastne promenne? nebo jine?
	//identifikatory, hash table, data ramce, stringy v ramci, istrukce, 
	//operandy instrukci podle toho jak se budou vytvaret

	tsDel(root);
	stackFree(&interStack);
	while ((sf=deleteFrame(sf)));
	instrListFree(iList);
}

labelAdress *labelAdressInit() {
	labelAdress *la = malloc(sizeof(labelAdress)*101);
	for (int i = 0; i < 101; i++)
		la[i].name = NULL;
	return la;
}
void addLabelAdress(labelAdress *la, char *name, tInstrListItem *addr) {
	int key = hash(name);
	if (la[key].name == NULL) {
		la[key].name = makeString(name);
		la[key].addr = addr;
		la[key].next = NULL;
		return;
	}
	labelAdress *item = &(la[key]); //last
	for (; item->next != NULL; item = item->next);
	labelAdress *tmp = malloc(sizeof(labelAdress));
	tmp->name = makeString(name);
	tmp->next = NULL;
	item->next = tmp;

}
tInstrListItem *getLabelAdress(labelAdress *la, char *name) {
	for (labelAdress *item = &(la[hash(name)]); item != NULL && item->name != NULL; item = item->next) {
		if (!strcmp(name, item->name))
			return item->addr; //vracim odkaz na zaznam
	}

	// nenalezen
	return NULL;
}
void deleteLabels(labelAdress la) {
	//TODO
}