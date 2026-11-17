#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#include "read_from_file.cpp"

#define PRINT_DUMP_NODE_WITH_CHARACTERISTICS(node_fillcolor)                                                                                 \
        fprintf(debug_out, "elem%zu [shape=Mrecord, style=filled, margin=0.05, fillcolor=\"%s\","                                             \
                "color = \"#585114\" label = \"{ {adress = %zu} | {data = %s} | {left = %zu} | {right = %zu} | {parent = %zu}}\"];\n",       \
                (size_t)node % 10000, node_fillcolor, (size_t)node % 10000,  node -> data,                                                       \
                (size_t)node -> left_node % 10000, (size_t)node -> right_node % 10000, (size_t)node -> parent_node % 10000);    

#define GRAPHVIZ_MODE

#ifdef GRAPHVIZ_MODE
    #define ON_GRPV(...) __VA_ARGS__
#else
    #define ON_GRPV(...) 
#endif

#define SSCANF_IN_READ_PREORDER(format, arg, len_parce_str_to_increase_buffer_IP)                                     \
                                sscanf(buffer + *current_buffer_IP, format, arg, len_parce_str_to_increase_buffer_IP); \
                                (*current_buffer_IP) += *len_parce_str_to_increase_buffer_IP;
                                
typedef char* elem_t;

const bool  LEFT_SIDE            = 0;
const bool  RIGHT_SIDE           = 1;

const int   MAXLEN_STR_PREORDER  = 50;

//dump consts
const int MAXLEN_EXTENSION             = 4; // like .png  .htm
const int MAXLEN_GRAPHVIZ_FOLDER_NAME  = 75;
const int MAXLEN_SYSTEM_STR            = MAXLEN_GRAPHVIZ_FOLDER_NAME +  50;
const int MAXLEN_STR_HTML_IMAGE_INSERT = 75;
const int FILE_PNG_GRAPHVIZ_NAME       = 75;
const int MAXLEN_HTML_DUMP_FILE_NAME   = 75;

struct TNode_t {

    elem_t data;

    TNode_t* left_node;
    TNode_t* right_node;

    TNode_t* parent_node;
};

struct parcer_t {

    char symbol                      = 0; 
    char* cur_str                    = NULL;
    int len_str                      = 0;

    TNode_t* cur_node                = NULL;

};

struct graph_dump_t {
    
    int number_of_dumps = 1;

    FILE* fp_html                                            = NULL;
    char folder_name[MAXLEN_GRAPHVIZ_FOLDER_NAME]         = "";
    char html_file_dump_name[MAXLEN_HTML_DUMP_FILE_NAME]  = "";
};

TNode_t*    node_create                 (TNode_t* parent, const bool parent_side, const elem_t data_value);
void        create_links                (TNode_t* cur_node, TNode_t* parent, const bool parent_side);
void        delete_node                 (TNode_t* cur_node);
  
void        print_tree_preorder_in_file (const TNode_t* main_node, FILE* fp_out);
  
TNode_t*    tree_read_from_file         (const char* filename);
TNode_t*    tree_read_preorder          (char* buffer, int* current_buffer_IP, TNode_t* current_parent, bool side);

#ifdef GRAPHVIZ_MODE
void        create_element_dump         (const TNode_t* node, FILE* debug_out);
void        list_dump_html              (const TNode_t* main_node);
void        list_graph_dump             (const TNode_t* main_node, char* name_graphviz_file_out, graph_dump_t dump_info);
void        print_links_in_dump         (FILE* debug_out, const TNode_t* node, 
                                        const char* color_right, const char* color_failure);
#endif



TNode_t* node_create(TNode_t* parent, const bool parent_side, const elem_t data_value) {

    assert (data_value);

    TNode_t* cur_node = (TNode_t*)calloc(1, sizeof(TNode_t)); 
    cur_node -> data = data_value;
    
    create_links(cur_node, parent, parent_side);

    return cur_node;
}


void create_links(TNode_t* cur_node, TNode_t* parent, const bool parent_side) {

    assert(cur_node);

    cur_node -> left_node   = NULL;
    cur_node -> right_node  = NULL;
    cur_node -> parent_node = parent;

    if (parent) {
        switch(parent_side) {
            case LEFT_SIDE:   parent -> left_node  = cur_node;  break;
            case RIGHT_SIDE:  parent -> right_node = cur_node;  break;
        }
    }
}

void delete_node(TNode_t* cur_node) {

    assert(cur_node);
 
    if (cur_node -> left_node) {
        cur_node -> left_node -> parent_node  = cur_node -> parent_node; 
        cur_node -> left_node = NULL;  
    }   

    if (cur_node -> right_node) {    
        cur_node -> right_node -> parent_node = cur_node -> parent_node;
        cur_node -> right_node = NULL;
    }

    cur_node -> parent_node = NULL;    
}


void print_tree_preorder_in_file(const TNode_t* main_node, FILE* fp_out) {

    assert(main_node);

    fprintf(fp_out, "( ");
    fprintf(fp_out, "\"%s\" ", main_node -> data);

    if (main_node -> left_node)      print_tree_preorder_in_file(main_node -> left_node, fp_out);
    else                             fprintf(fp_out, "nil ");

    if (main_node -> right_node)     print_tree_preorder_in_file(main_node -> right_node, fp_out);
    else                             fprintf(fp_out, "nil ");

    fprintf(fp_out, ") ");

    ON_GRPV(if (main_node -> parent_node == NULL) {
        list_dump_html(main_node);
    })
}

TNode_t* tree_read_from_file(const char* filename) {

    assert(filename);

    file_info_t file_info = {};
    char* buffer = read_from_file(&file_info, filename);

    int current_buffer_IP   = 0;
    TNode_t* current_parent =  NULL;

    TNode_t* main_node =  tree_read_preorder(buffer, &current_buffer_IP, current_parent, LEFT_SIDE);

    free(buffer);
    return main_node;

}

TNode_t* tree_read_preorder(char* buffer, int* current_buffer_IP, TNode_t* current_parent, const bool side) {

    parcer_t node_parcer_info = {};
    node_parcer_info.cur_str = (char*)calloc(MAXLEN_STR_PREORDER, sizeof(char));

    SSCANF_IN_READ_PREORDER(" %c%n", &node_parcer_info.symbol, &node_parcer_info.len_str)

    if (node_parcer_info.symbol == 'n') {
        SSCANF_IN_READ_PREORDER("%s%n", node_parcer_info.cur_str, &node_parcer_info.len_str)
        if (strcmp(node_parcer_info.cur_str, "il") == 0) { 
            return NULL;
        }
    }

    if (node_parcer_info.symbol == '(') {
        SSCANF_IN_READ_PREORDER(" \"%49[^\"]\"%n", node_parcer_info.cur_str, &node_parcer_info.len_str)

        node_parcer_info.cur_node = node_create(current_parent, side, node_parcer_info.cur_str);
        current_parent = node_parcer_info.cur_node;

        tree_read_preorder(buffer, current_buffer_IP, current_parent, LEFT_SIDE);
        tree_read_preorder(buffer, current_buffer_IP, current_parent, RIGHT_SIDE);
    }

    SSCANF_IN_READ_PREORDER(" %c%n", &node_parcer_info.symbol, &node_parcer_info.len_str)
    if (node_parcer_info.symbol == ')') {
        return node_parcer_info.cur_node;
    }
    return NULL;
}


#ifdef GRAPHVIZ_MODE

    void list_graph_dump(const TNode_t* main_node, char* name_graphviz_file_out, graph_dump_t dump_info) { 

        snprintf(name_graphviz_file_out, FILE_PNG_GRAPHVIZ_NAME, "%s/log_%d.dot", dump_info.folder_name, dump_info.number_of_dumps);
        FILE* debug_out = fopen(name_graphviz_file_out, "w");

        //general settings
        fprintf(debug_out, "digraph List {\n"
                            "rankdir = TB;    \n"
                            "ranksep = 0.7    \n"
                            "nodesep = 0.7;   \n"
                            "splines = true;\n");
            
        
        create_element_dump(main_node, debug_out);
        fprintf(debug_out, "}\n");

        fclose(debug_out);

        char system_str[MAXLEN_GRAPHVIZ_FOLDER_NAME + 50] = "";
        snprintf(system_str, MAXLEN_SYSTEM_STR,  "dot -Tpng \"%s\" -o \"%.*s.png\"", name_graphviz_file_out, 
                (int)strlen(name_graphviz_file_out) - MAXLEN_EXTENSION, name_graphviz_file_out);
        system(system_str);

        char* extension = strchr(name_graphviz_file_out, '.');
        strcpy(extension, ".png");
    }

    void list_dump_html(const TNode_t* main_node) {

        static graph_dump_t dump_info = {};
        if (dump_info.number_of_dumps == 1) {

            time_t now = time(NULL);
            tm* t      = localtime(&now);
            strftime(dump_info.folder_name, MAXLEN_GRAPHVIZ_FOLDER_NAME, "logs_images_%m-%d_%H-%M-%S", t);
            _mkdir(dump_info.folder_name);

            snprintf(dump_info.html_file_dump_name, MAXLEN_HTML_DUMP_FILE_NAME, "%s/general_dump.html", dump_info.folder_name);
            dump_info.fp_html = fopen(dump_info.html_file_dump_name, "w");
            fprintf(dump_info.fp_html,
            "<!DOCTYPE html>\n"
                    "<html>\n\n"
                    "\t<meta charset=\"UTF-8\">\n"
                    "\t<title>List dump</title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<div style=\"display: flex; flex-direction: column; gap: 30px;\">\n");
            
        }

        dump_info.fp_html = fopen(dump_info.html_file_dump_name, "a");
        fprintf(dump_info.fp_html, "<h2 style = \"font-family: Georgia, serif;"
                                                "font-size: 42px;"
                                                "color: #34495e;\">"
                                                "Dump #%d</h2>\n", dump_info.number_of_dumps);

        // Generate PNG
        char name_graphviz_file[FILE_PNG_GRAPHVIZ_NAME];
        list_graph_dump(main_node, name_graphviz_file, dump_info);


        // Two blocks: dump and image
        fprintf(dump_info.fp_html,
            "<div style=\"display: flex; flex-direction: row; "
            "gap: 40px; align-items: flex-start; margin-bottom: 30px;\">\n"
        );
        // IMAGE
        char image_html_str[MAXLEN_STR_HTML_IMAGE_INSERT] = "";
        char* image_name = strrchr(name_graphviz_file, '/') + 1;
        snprintf(image_html_str, MAXLEN_STR_HTML_IMAGE_INSERT, "<img src=\"%s\" width=\"1250\">\n", image_name);
        fprintf(dump_info.fp_html, "%s", image_html_str);
        fprintf(dump_info.fp_html, "%s", image_html_str);
        // HTML ending
        fprintf(dump_info.fp_html,"</div>\n");
        fclose(dump_info.fp_html);
        dump_info.number_of_dumps++; 
    }

    void create_element_dump(const TNode_t* node, FILE* debug_out) {

        if (strchr(node -> data, '?') != NULL) {
            PRINT_DUMP_NODE_WITH_CHARACTERISTICS("#9ba344")
        }
        else {
            PRINT_DUMP_NODE_WITH_CHARACTERISTICS("#f2f3e8");
        }

        if (node -> left_node)             create_element_dump(node -> left_node, debug_out);
        if (node -> right_node)            create_element_dump(node -> right_node, debug_out);

        if (node -> left_node && node -> right_node) {
            fprintf(debug_out, "{rank = same; elem%zu; elem%zu};", 
            (size_t)node -> left_node % 10000, (size_t)node -> right_node % 10000);
        }

        fprintf(debug_out, "edge[dir=\"forward\", weight=0, arrowsize=1];");
        print_links_in_dump(debug_out, node -> left_node, "#6dfc74",  "#0b61ed");
        print_links_in_dump(debug_out, node -> right_node,"#f30e0e",  "#0b61ed");
    }

    void print_links_in_dump(FILE* debug_out, const TNode_t* node, const char* color_right, const char* color_failure) {
        
        if (node) {
            if (node  == node ->  parent_node -> left_node || node == node ->  parent_node -> right_node)
                fprintf(debug_out, "elem%zu -> elem%zu[color = \"%s\"];\n", 
                        (size_t)node -> parent_node % 10000, (size_t)node % 10000, color_right);
            else 
                fprintf(debug_out, "elem%zu -> elem%zu[color = \"%s\"];\n", 
                        (size_t)node -> parent_node % 10000, (size_t)node % 10000, color_failure);
        }
    }
#endif
