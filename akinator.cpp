
#include <shlobj.h>
#define COBJMACROS

#include <cassert>
#include <cstring>
#include <windows.h>
#include <sapi.h>

#include "Tree.cpp"



const int MAXLEN_ANSWER    = 5;
const int MAXLEN_ADJECTIVE = 51;
const int MAXLEN_CHARACTER = 50;
const int MAXLEN_MODE      = 21;

struct akinator_info_t {
    const char* filename        =  "";
    FILE*       fp_akin_out     = NULL;
    TNode_t*    main_node       = NULL;
    TNode_t*    last_add_node   = NULL;

};

struct new_node_t {
    char* new_character      = NULL;
    char* new_characteristic = NULL;
    char* ptr_last_symbol    = NULL;
};


enum akin_answers_t {
    ANSWER_YES    = 1, 
    ANSWER_NO     = 2,
    ANSWER_ERROR  = -1
};

#define GREEN  "\033[32m"
#define BLUE   "\033[34m"
#define RESET  "\033[37m"
 
#define USER_ANSWER_TO_CONTINUE(question)  printf("%s\n", question);                                                 \
                                                        scanf(" %4[^\n]", answer_quest);                              \
                                                                                                                       \
                                                        if (strcmp("YES", answer_quest) == 0)    return ANSWER_YES;            \
                                                        else                                     return ANSWER_NO;      

#define DEFINATION_MODE  

#ifdef DEFINATION_MODE
    #define ON_DFNT(...) __VA_ARGS__
#else
    #define ON_DFNT(...) 
#endif

#define COMPARE_MODE  

#ifdef COMPARE_MODE
    #define ON_CMPR(...) __VA_ARGS__
#else
    #define ON_CMPR(...) 
#endif

#define SOUND_MODE  

#ifdef SOUND_MODE
    #define ON_SND(...) __VA_ARGS__
#else
    #define ON_SND(...) 
#endif


void             akinator_info_init                     (akinator_info_t* akinator_info, const char* filename_write_preorder);
void             save_tree_in_file                      (akinator_info_t* akinator_info);
akin_answers_t   akinator_add_new_node                  (akinator_info_t* akinator_info, TNode_t* parent_node, bool parent_side);
akin_answers_t   work_with_user_to_add_node             (new_node_t* new_node_info, TNode_t* cur_node, bool parent_side);
akin_answers_t   akinator_iter                          (akinator_info_t* akinator_info, TNode_t* cur_node, bool parent_side);
akin_answers_t   akinator_main                          (const char* filename_write_preorder);

#if defined(COMPARE_MODE) || defined(DEFINATION_MODE)
    TNode_t*    find_node_with_word                     (TNode_t* cur_node, const char* etalon_word);
    TNode_t*    get_character_from_user                 (akinator_info_t* akinator_info);
    void        write_characteristic_depend_on_side               (TNode_t* cur_node, TNode_t* child_node);
#endif

ON_DFNT(
akin_answers_t  write_definations_of_words_in_console   (akinator_info_t* akinator_info);
void            recurtion_func_for_definations          (TNode_t* cur_node, TNode_t* child_node);)

ON_SND(
void            say                                     (const char* text);)

ON_CMPR(    
akin_answers_t write_compares_in_console                (akinator_info_t* akinator_info);
void           recurtion_func_for_comparing             (TNode_t* cur_node1, TNode_t* cur_node2, TNode_t* child_node1,  TNode_t* child_node2, const char* character1, const char* character2);)

int main(const int argc, char* argv[]) {

    if (argc == 2) {
        akinator_main(argv[1]);
        return 0;
    }
    printf("file is not found:(");
    return 1;

}

akin_answers_t akinator_main(const char* filename_write_preorder) {

    assert(filename_write_preorder);

    akinator_info_t akinator_info = {};
    akinator_info_init(&akinator_info, filename_write_preorder);

    char mode[MAXLEN_MODE]           = "";
    char answer_quest[MAXLEN_ANSWER] = "";


    do {
        printf("Choose mode please\n");
        scanf("%20s", mode);

        if (strcmp("AKINATOR", mode) == 0) {
            while (akinator_iter(&akinator_info, akinator_info.main_node, LEFT_SIDE) == ANSWER_YES)
                ;
        }
        
        ON_DFNT(else if (strcmp("DEFINE", mode) == 0) {
                    while (write_definations_of_words_in_console(&akinator_info) == ANSWER_YES)
                        ;
        })
        ON_CMPR(else if (strcmp("COMPARE", mode) == 0) {
                    while (write_compares_in_console(&akinator_info) == ANSWER_YES)
                        ;
        })

        else {
            printf(BLUE "I don't know this mode((, repeat plase\n" RESET);
        }

    } while(true);

    return ANSWER_NO;
}

void save_tree_in_file(akinator_info_t* akinator_info) {

    assert(akinator_info);

    akinator_info -> fp_akin_out = fopen(akinator_info -> filename, "w");
    print_tree_preorder_in_file(akinator_info -> main_node, akinator_info -> last_add_node, akinator_info -> fp_akin_out);
    fclose(akinator_info -> fp_akin_out);
}
 
void akinator_info_init(akinator_info_t* akinator_info, const char* filename_write_preorder) {

    assert(akinator_info);
    assert(filename_write_preorder);
    
    akinator_info -> filename  = filename_write_preorder;
    akinator_info -> main_node = tree_read_from_file(akinator_info -> filename);
}

akin_answers_t akinator_iter(akinator_info_t* akinator_info, TNode_t* cur_node, bool parent_side) {

    assert(akinator_info);
    assert(cur_node);

    char answer_quest[MAXLEN_ANSWER] = "";
    printf("%s\n", cur_node -> data);

    if (scanf(" %4[^\n]", answer_quest) == 1) {
        if (strcmp(answer_quest, "YES") == 0) {
            if (cur_node -> left_node == NULL && cur_node -> right_node == NULL) {
                printf(GREEN "Your character is %s. UFFF BITCH\n" RESET, cur_node -> data);
            }
            else 
                akinator_iter(akinator_info, cur_node -> left_node, LEFT_SIDE);
        }

        else {
            if (cur_node -> left_node == NULL && cur_node -> right_node == NULL) {
                akinator_add_new_node(akinator_info, cur_node -> parent_node, parent_side);
            }
            else 
                akinator_iter(akinator_info, cur_node -> right_node, RIGHT_SIDE);
        }
    }
    else 
        printf("INCORRECT SENTENCE!\n");

    USER_ANSWER_TO_CONTINUE("Do you want to continue?\n");

    return ANSWER_ERROR;
}

akin_answers_t akinator_add_new_node(akinator_info_t* akinator_info, TNode_t* parent_node, bool parent_side) {

    assert(parent_node);

    new_node_t new_node_info = {};
    if (work_with_user_to_add_node(&new_node_info, parent_node, parent_side) == ANSWER_ERROR) {
        return ANSWER_ERROR;
    };

    TNode_t* old_node = (parent_side == LEFT_SIDE) ? parent_node -> left_node : parent_node -> right_node;
    TNode_t* new_node =  node_create(parent_node, parent_side, new_node_info.new_characteristic);
    akinator_info -> last_add_node = node_create(new_node, LEFT_SIDE,  new_node_info.new_character);
    node_create(new_node, RIGHT_SIDE,  old_node -> data);

    save_tree_in_file(akinator_info);

    return ANSWER_NO;
}

akin_answers_t work_with_user_to_add_node(new_node_t* new_node_info, TNode_t* cur_node, bool parent_side) {

    assert(cur_node);
    assert(new_node_info);

    new_node_info -> new_character       = (char*)calloc(MAXLEN_CHARACTER, sizeof(char));
    new_node_info -> new_characteristic  = (char*)calloc(MAXLEN_ADJECTIVE, sizeof(char));

    printf("I don't know your character(. Who is it?\n"); 
    if (scanf(" %49[^\n]", new_node_info -> new_character) != 1) {
        return ANSWER_ERROR;
    }
    
    if (parent_side == LEFT_SIDE) 
        printf("Help me, what the difference between %s and %s? Your answer ____ (only characteristic, less than 50 symbols, please)\n", 
               cur_node -> left_node ->  data, new_node_info -> new_character);
    else
        printf("Help me, what the difference between %s and %s? Your answer ____ (only characteristic, less than 50 symbols, please)\n", 
                cur_node -> right_node -> data, new_node_info -> new_character);

    if(scanf(" %49[^\n]", new_node_info -> new_characteristic) != 1) {
        return ANSWER_ERROR;
    }
    new_node_info -> ptr_last_symbol = strchr(new_node_info -> new_characteristic, '\0');
    strcpy(new_node_info -> ptr_last_symbol, "?");

    return ANSWER_NO;
}

ON_DFNT(
    akin_answers_t write_definations_of_words_in_console(akinator_info_t* akinator_info) {                                            

        char     character_to_identify[MAXLEN_CHARACTER] = "";
        TNode_t* etalon_node                             = NULL;
        char     answer_quest[MAXLEN_ANSWER]             = "";

        printf("whom want you to identify?\n");
        do {
            etalon_node = get_character_from_user(akinator_info);
        } while (etalon_node == NULL);

        recurtion_func_for_definations(etalon_node, NULL);

        USER_ANSWER_TO_CONTINUE("Do you want to continue?");
    }

    void recurtion_func_for_definations(TNode_t* cur_node, TNode_t* child_node) {

        if (cur_node == NULL) {
            return;
        }
        
        if (cur_node -> left_node == NULL && cur_node -> right_node == NULL) { 
            printf("%s - ", cur_node -> data);
            ON_SND(say( cur_node -> data);)
            recurtion_func_for_definations(cur_node -> parent_node, cur_node);
            return;
        } 

        write_characteristic_depend_on_side(cur_node, child_node);

        if (cur_node -> parent_node == NULL) {
            printf(".\n\n");
            return;
        }
        printf(", ");

        recurtion_func_for_definations(cur_node -> parent_node, cur_node);
        
    }
)

#if defined(COMPARE_MODE) || defined(DEFINATION_MODE) 
    
    TNode_t* find_node_with_word(TNode_t* cur_node, const char* etalon_word) {

        if (cur_node == NULL) {
            return NULL;
        }

        if (strcmp(cur_node -> data, etalon_word) == 0 && cur_node -> left_node == NULL && cur_node -> right_node == NULL) {
            return cur_node;
        }

        if (cur_node -> left_node) {
            TNode_t* result = find_node_with_word(cur_node -> left_node,  etalon_word);
            if (result)       return result;
        }
        if (cur_node -> right_node) {
            TNode_t* result = find_node_with_word(cur_node -> right_node,  etalon_word);
            if (result)       return result;
        }

        return NULL;
    }

    TNode_t* get_character_from_user(akinator_info_t* akinator_info) {

        char character_to_identify[MAXLEN_CHARACTER] = "";

        while(scanf(" %49[^\n]", character_to_identify) != 1) {
            printf( BLUE "Repeat, please\n" RESET);
        }

        TNode_t* etalon_node = find_node_with_word(akinator_info -> main_node, character_to_identify);
        if(etalon_node == NULL) {
            printf(BLUE "No such character, repeat\n" RESET);
            return NULL;
        }

        return etalon_node;
    }

    void write_characteristic_depend_on_side(TNode_t* cur_node, TNode_t* child_node) {    


        if (child_node == cur_node -> right_node) {
            printf("not ");
            ON_SND(say("not");)
        }

        printf("%.*s", (int)strlen(cur_node -> data) - 1, cur_node -> data);
        ON_SND(say(cur_node -> data);)
    }      
#endif

ON_CMPR(    
    
    akin_answers_t write_compares_in_console(akinator_info_t* akinator_info) {
    
        char     character_to_identify[MAXLEN_CHARACTER] = "";
        TNode_t* etalon_node1                            = NULL;
        TNode_t* etalon_node2                            = NULL;
        char     answer_quest[MAXLEN_ANSWER]             = "";

        printf("Who do you want to compare?\n");
        do {
            etalon_node1 = get_character_from_user(akinator_info);
        } while (etalon_node1 == NULL);

        do {
            etalon_node2 = get_character_from_user(akinator_info);
        } while (etalon_node2 == NULL);

        recurtion_func_for_comparing(etalon_node1, etalon_node2, NULL, NULL, etalon_node1 -> data, etalon_node2 -> data);

        USER_ANSWER_TO_CONTINUE("Do you want to continue?");
    }

    void recurtion_func_for_comparing(TNode_t* cur_node1, TNode_t* cur_node2, TNode_t* child_node1,  TNode_t* child_node2, const char* character1, const char* character2) {

            if (cur_node1 == NULL || cur_node2 == NULL) {
                printf("\n");
                return;
            }

            if (child_node1 == NULL && child_node2 == NULL) {
                recurtion_func_for_comparing(cur_node1 -> parent_node, cur_node2 -> parent_node, cur_node1, cur_node2, character1, character2);
                return;
            }

            printf("%s", character1);
            ON_SND(say(character1));
            printf(" is ");
            ON_SND(say("is");)

            write_characteristic_depend_on_side(cur_node1, child_node1);
        

            printf(", but ");
            ON_SND(say("but");)

            printf("%s", character2);
            ON_SND(say(character2);)

            printf(" is ");
            ON_SND(say("is");)
           
            write_characteristic_depend_on_side(cur_node2, child_node2);

            printf(";\n");

            recurtion_func_for_comparing(cur_node1 -> parent_node, cur_node2 -> parent_node, cur_node1, cur_node2, character1, character2);

            return;
    })

ON_SND(void say(const char* text) {

    size_t size = MultiByteToWideChar(
    CP_UTF8,
    0,
    text,
    -1,
    NULL,
    0
    );

    wchar_t* wide_text  = (wchar_t*)calloc(size, sizeof(wchar_t));

    MultiByteToWideChar(
    CP_UTF8,
    0,
    text,
    -1,
    wide_text,
    size
    );

    ISpVoice* voice = NULL;
    HRESULT result = CoInitialize(NULL);
    if (FAILED(result)) {
        printf(BLUE "COM initialization error\n" BLUE);
        return;
    }

    CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&voice);
    voice -> SetRate(5);
    voice -> Speak(wide_text, SPF_DEFAULT, NULL);
    
    voice -> Release();
    CoUninitialize();
    })
