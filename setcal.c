/**
 * @author Martin Machálek (xmacha83@stud.fit.vutbr.cz)
 * @author Filip Křepelka (xkrepe05@stud.fit.vutbr.cz)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ERROR_INVALID_DEFINITION 1
#define ERROR_OUT_OF_MEMORY 2
#define ERROR_COMMAND_NO_PARAMETERS 3
#define ERROR_INVALID_LINE 4
#define ERROR_TOO_MANY_LINES 5
#define ERROR_INVALID_COMMAND 6
#define ERROR_ILLEGAL_NAME 7
#define ERROR_INVALID_ITEM 8
#define ERROR_REPEATING_ITEM_IN_SET 9
#define ERROR_EXPECTED_SET 10
#define ERROR_EXPECTED_RELATION 11
#define ERROR_RIGHT_SIDE_OF_RELATION_NOT_DEFINED 12
#define ERROR_REPEATING_ITEM_IN_RELATION 13
#define ERROR_RELATION_FORMAT 14
#define ERROR_EXCEEDED_30_CHARS 30

// *************
// * Universum *
// *************
typedef struct {
    char ** names;
    int size;
} Universum;
void Universum_constructor(int * error_code, Universum ** universum){
    *universum = malloc(sizeof(Universum));
    if (*universum == NULL){
        *error_code = ERROR_OUT_OF_MEMORY;
    } else {
        (*universum)->size = 0;
        (*universum)->names = NULL;
    }
};
void Universum_destructor(Universum * universum){
    if (universum == NULL) return;
    for (int i = 0; i < universum->size; i++)
        free(universum->names[i]);
    free(universum->names);
    free(universum);
};
void Universum_add(int * error_code, Universum * universum, char * name){
    universum->size++;
    universum->names = realloc(universum->names, universum->size * sizeof(char *));
    if (universum->names == NULL){
        *error_code = ERROR_OUT_OF_MEMORY;
        return;
    }
    universum->names[universum->size-1] = name;
};
char * getName(Universum * universum, int id){
    if ((id < 0) || (id > universum->size)) return NULL;
    return universum->names[id];
}

// *************
// * Set *
// *************
typedef struct Set {
    int line;
    int * items;
    int size;
    struct Set *next;
} Set;

void Set_constructor(int * error_code, Set ** set){
    *set = malloc(sizeof(Set));
    if(*set == NULL){
        *error_code = ERROR_OUT_OF_MEMORY;
    } else {
        (*set)->line = 0;
        (*set)->size = 0;
        (*set)->items = NULL; 
        (*set)->next = NULL;
    }
}
void Set_destructor(Set * set){
    if (set == NULL) return;
    if (set->next != NULL) Set_destructor(set->next);
    free(set->items);
    free(set);
}
void Set_add(int * error_code, Set * set, int index){
    set->size++;
    set->items = realloc(set->items, sizeof(int) * set->size);
    if (set->items == NULL){
        *error_code = ERROR_OUT_OF_MEMORY;
        return;
    }
    set->items[set->size-1] = index;
};
Set * getSet(Set * set, int line){
    Set * res = set;
    while (res != NULL){
        if (res->line == line)
            return res;
        res = res->next;
    }
    return NULL;
}

// *************
// * Relation *
// *************
typedef struct Relation {
    int line;
    int * items_first;
    int * items_second;
    int size;
    struct Relation *next;
} Relation;

void Relation_constructor(int * error_code, Relation ** relation){
    *relation = malloc(sizeof(Relation));
    if (*relation == NULL){
        *error_code == ERROR_OUT_OF_MEMORY;
    } else {
        (*relation)->line = 0;
        (*relation)->items_first = NULL;
        (*relation)->items_second = NULL;
        (*relation)->size = 0;
        (*relation)->next = NULL; 
    }
}
void Relation_destructor(Relation * relation){
    if (relation == NULL) return;
    if (relation->next != NULL) Relation_destructor(relation->next);
    free(relation->items_first);
    free(relation->items_second);
    free(relation);
};
void Relation_add(int * error_code, Relation * relation, int first_index, int second_index){
    relation->size++;
    relation->items_first = realloc(relation->items_first, sizeof(int) * relation->size);
    relation->items_second = realloc(relation->items_second, sizeof(int) * relation->size);
    if ((relation->items_first == NULL) || (relation->items_second == NULL)){
        *error_code = ERROR_OUT_OF_MEMORY;
        return;
    }
    relation->items_first[relation->size-1] = first_index;
    relation->items_second[relation->size-1] = second_index;
};
Relation * getRelation(Relation * relation, int line){
    Relation * res = relation;
    while (res != NULL){
        if (res->line == line)
            return res;
        res = res->next;
    }
    return NULL;
}

// * Commands
int empty(Universum * universum, Set * set, Relation * relation, int *lines){
    Set * s1 = getSet(set, lines[0]);
    if (s1 == NULL) return ERROR_EXPECTED_SET;
    if (s1->size<=0){ printf("empty"); return 0; }
    else printf("not empty"); 
    return 1;
};

int card(Universum * universum, Set * set, Relation * relation, int *lines){
    Set * s1 = getSet(set, lines[0]);
    if (s1 == NULL) return ERROR_EXPECTED_SET;
    printf("Number of elements in set on line %d: %d", s1->line, s1->size);
    return 0;
};

int complement(Universum * universum, Set * set, Relation * relation, int *lines){
    Set * s1 = getSet(set, lines[0]);
    if(s1 == NULL) return ERROR_EXPECTED_SET;
    char resItems[] = s1->items;
    bool new = true; int n = 0;
    for (int i = 1; i>universum->size+1; i++){
        for (int j = 0; j>s1->size; i++){
            if(strcmp(universum->names[i], s1->items[j]) == 0){
                new = false;
                break;
            }
        }
        if (new){
            resItems[s1->size+i] = universum->names[i];
            n++;
        } 
    }
    printf("S ");
    for (int i = 0; i<s1->size+n; i++){
        printf("%s ", resItems[i]);
    }
    return 0;
};

int set_union(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if (s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    bool new = true;
    int n = 0;
    char ret[s1->size + s2->size];
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    for (int i = 0; i<s1->size; i++){
        for (int j = i+1; j<s2->size; j++){
            if (s1->items[i] == s2->items[j]) new = false;
        }
        if(new) ret[n] = s1->items[i]; n++;
    }
    new = true;
    for (int i = 0; i<s2->size; i++){
        for (int j = i+1; j<s1->size; j++){
            if(s2->items[i] == s1->items[j]) new = false;
        }
        if(new) ret[n] = s2->items[i]; n++; 
    }
    printf("S ");
    for (int i = 0; i<n-1;i++){
        printf("%s ", ret[i]);
    }
    return 0;
};

int intersect(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    char res[] = {0}; int n = 0;
    for (int i = 0; i < s1->size; i++){
        for (int j = 0; j < s2->size; j++){
            if(s1->items[i] == s2->items[j]){
                res[i] = s1->items[i];
                n++;
            }
        }
    }
    printf("S ");
    for (int i = 0; i < n; i++){
        printf("%s ", res[i]);
    }
    return 0;
};

int minus(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    char resItems[] = s1->items;
    bool new = true; int n = 0;
    for (int i = 1; i>s2->size+1; i++){
        for (int j = 0; j>s1->size; i++){
            if(strcmp(s2->items[i], s1->items[j]) == 0){
                new = false;
                break;
            }
        }
        if (new){
            resItems[s1->size+i] = s2->items[i];
            n++;
        } 
    }
    printf("S ");
    for (int i = 0; i<s1->size+n; i++){
        printf("%s ", resItems[i]);
    }
    return 0;   
};

int subseteq(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    char res[] = {0}; bool subel = false;
    for (int i = 0; i < s1->size; i++){
        for (int j = 0; j < s2->size; i++){
           if (strcmp(s1->items[i],s2->items[j]) == 0){
               subel = true;
           } 
        }
        if(!subel) break;
        subel = false;
    }
    if (subel) return 0;
    return 1;
};

int subset(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    char res[] = {0}; bool subel = false;
    for (int i = 0; i < s1->size; i++){
        for (int j = 0; j < s2->size; i++){
           if (strcmp(s1->items[i],s2->items[j]) == 0){
               subel = true;
           } 
        }
        if(!subel) break;
        subel = false;
    }
    if (subel && s2->size > s1->size) return 0;
    return 1;
};

int equals(Universum * universum, Set * setA, Set * setB, int *lines){
    Set * s1 = getSet(setA, lines[0]);
    Set * s2 = getSet(setB, lines[1]);
    if(s1 == NULL || s2 == NULL) return ERROR_EXPECTED_SET;
    char res[] = {0}; bool subel = false;
    for (int i = 0; i < s1->size; i++){
        for (int j = 0; j < s2->size; i++){
           if (strcmp(s1->items[i],s2->items[j]) == 0){
               subel = true;
           } 
        }
        if(!subel) break;
        subel = false;
    }
    if (subel && s1->size == s2->size) return 0;
    return 1;
};




int printline(Universum * universum, Set * set, Relation * relation, int *lines){
    for (int i = 0; i < 3; i++){
        if (lines[i] == 0) break;
        Set * cset = getSet(set, lines[i]);
        Relation * crelation = getRelation(relation, lines[i]);
        if (cset != NULL){
            printf("Set(%d):", cset->line);
            for (int o = 0; o < cset->size; o++)
                printf(" %s", getName(universum, cset->items[o]));
            printf("\n");
        } 
        if (crelation != NULL){
            printf("Relation(%d):", crelation->line);
            for (int o = 0; o < crelation->size; o++)
                printf(" (%s %s)", getName(universum, crelation->items_first[o]), getName(universum, crelation->items_second[o]));
            printf("\n");
        } 
    }
}

// ********
// * Read *
// ********
char * readline(int * code){
    char * buf = malloc(31);
    int i = 0;
    char c;
    while (1){
        c = getchar();
        if (c == '\r') continue; // Windows thing
        if (c == ' '){ *code = 0; break; }
        if ((c == '\n') || (c == EOF)){ *code = 1; break; }
        if (i == 31){ *code = 2; break; }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return buf;
}
bool validate_name(char * name){
    // ? alphabet only
    for (int i = 0; name[i] != '\0'; i++){
        if((name[i] < 'A' || name[i] > 'Z') && (name[i] < 'a' || name[i] > 'z')){
            return false;
        }
    }

    // ? can't be true/false/U/S/R/C/
    char *forbiden_names[] = {"true", "false", "U", "S", "R", "C", NULL};

    for (int i = 0; forbiden_names[i] != NULL; i++){
        if(strcmp(name, forbiden_names[i]) == 0){
            return false;
        }
    }

    return true;
}
void read_universum(int * error_code, Universum ** universum){
    Universum_constructor(error_code, universum);    
    if (*error_code != 0) return;
    int code;
    while(1){
        char * name = readline(&code);
        if (code == 2){ free(name); *error_code = ERROR_EXCEEDED_30_CHARS; return; }
        if (validate_name(name) == false){ free(name); *error_code = ERROR_ILLEGAL_NAME; return; }
        Universum_add(error_code, *universum, name);        
        if (*error_code != 0) { free(name); return; }
        if (code == 1) break;
    }
};
void read_set(int * error_code, int line, Universum * universum, Set ** set){
    if (*set != NULL){
        do {
            set = &((*set)->next);
        } while (*set != NULL);
    }
    Set_constructor(error_code, set);
    if (*error_code != 0) return;
    (*set)->line = line;
    
    int code, index;
    while(1){
        char * name = readline(&code);
        if (code == 2){ free(name); *error_code = ERROR_EXCEEDED_30_CHARS; return; }
        
        index = -1;
        for (int i = 0; i < universum->size; i++){
            if (strcmp(name, universum->names[i]) == 0){
                index = i; break;
            }
        }
        free(name);
        /*if (index == -1){ *error_code = ERROR_INVALID_ITEM; return; }*/
        
        for (int i = 0; i < (*set)->size; i++){
            if ((*set)->items[i] == index){
                *error_code = ERROR_REPEATING_ITEM_IN_SET; return;
            }
        }
        
        Set_add(error_code, *set, index);
        if (*error_code != 0) return;
        if (code == 1) break;
    }
};
void read_relation(int * error_code, int line, Universum * universum, Relation ** relation){
    if (*relation != NULL){
        do {
            relation = &((*relation)->next);
        } while (*relation != NULL);
    }
    Relation_constructor(error_code, relation);
    if (*error_code != 0) return;
    (*relation)->line = line;

    int code, index1, index2;
    while(1){
        char * name1 = readline(&code);
        if (code == 2){ free(name1); *error_code = ERROR_EXCEEDED_30_CHARS; return; }
        if (code == 1){ free(name1); *error_code = ERROR_RIGHT_SIDE_OF_RELATION_NOT_DEFINED; return; }
        if (name1[0] != '('){ free(name1); *error_code = ERROR_RELATION_FORMAT; return; }
        name1++;

        index1 = -1;
        for (int i = 0; i < universum->size; i++){
            if (strcmp(name1, universum->names[i]) == 0){
                index1 = i; break;
            }
        }
        free(name1-1);
        if (index1 == -1){ free(name1); *error_code = ERROR_INVALID_ITEM; return; }

        char * name2 = readline(&code);
        if (code == 2){ free(name1); free(name2);  *error_code = ERROR_EXCEEDED_30_CHARS; return; }
        if (name2[strlen(name2)-1] != ')'){ free(name1); free(name2); *error_code = ERROR_RELATION_FORMAT; return; }
        name2[strlen(name2)-1] = '\0';
        
        index2 = -1;
        for (int i = 0; i < universum->size; i++){
            if (strcmp(name2, universum->names[i]) == 0){
                index2 = i; break;
            }
        }
        free(name2);
        if (index2 == -1){ *error_code = ERROR_INVALID_ITEM; return; }
        
        for (int i = 0; i < (*relation)->size; i++){
            if (((*relation)->items_first[i] == index1) && ((*relation)->items_second[i] == index2)){
                *error_code = ERROR_REPEATING_ITEM_IN_RELATION; return;
            }
        }
        
        Relation_add(error_code, *relation, index1, index2);
        if (*error_code != 0) return;
        if (code == 1) break;
    }
};
void read_command(int * error_code, Universum * universum, Set * set, Relation * relation){
    int code;
    char * command = readline(&code);
    if (code == 1){ *error_code = ERROR_COMMAND_NO_PARAMETERS; return; }
    if (code == 2){ *error_code = ERROR_EXCEEDED_30_CHARS; return; }

    int lines[3] = {0,0,0};
    int i = 0;
    while(1){
        char * codestr = readline(&code);
        lines[i] = atoi(codestr);
        free(codestr);
        if (lines[i] == 0){ *error_code = ERROR_INVALID_LINE; return; }
        if (code == 1) break;
        if (i == 2){ *error_code = ERROR_TOO_MANY_LINES; return; }
        i++;
    }

    if (strcmp(command, "empty") == 0) *error_code = empty(universum, set, relation, lines);
    else if (strcmp(command, "intersect") == 0) *error_code = printline(universum, set, relation, lines);
    else if (strcmp(command, "minus") == 0) *error_code = printline(universum, set, relation, lines);
    else if (strcmp(command, "card") == 0) *error_code = card(universum, set, relation, lines);
    else if (strcmp(command, "union") == 0) *error_code = set_union(universum, set, set->next, lines);
    else *error_code = ERROR_INVALID_COMMAND;

    free(command);
};

void read(int * error_code, Universum ** universum, Set ** set, Relation ** relation){
    int line = 1;
    while (*error_code == 0){
        char c = getchar();
        
        if (c == EOF) break;
        else if (getchar() != ' ') *error_code = ERROR_INVALID_DEFINITION;
        else if (c == 'U') read_universum(error_code, universum);
        else if (c == 'S') read_set(error_code, line, *universum, set);
        else if (c == 'R') read_relation(error_code, line, *universum, relation);
        else if (c == 'C') read_command(error_code, *universum, *set, *relation);
        else *error_code = ERROR_INVALID_DEFINITION;

        line++;
    }
}
void print_error(int error_code){
    switch (error_code){
        case ERROR_INVALID_DEFINITION: fprintf(stderr, "...\n"); break; 
        default: fprintf(stderr, "Undocumented error: %d\n", error_code);
    }
}

int main(){
    int error_code = 0;

    Universum * universum = NULL;
    Set * set = NULL;
    Relation * relation = NULL;

    read(&error_code, &universum, &set, &relation);

    // TODO
    if (error_code != 0)
        print_error(error_code);
        
    Universum_destructor(universum);
    Set_destructor(set);
    Relation_destructor(relation);
    return error_code;
}
