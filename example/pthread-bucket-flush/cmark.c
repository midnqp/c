#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deps/cmark/cmark.h"

#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_REVERSE "\x1b[7m"
#define ANSI_BLACK "\x1b[30m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_WHITE "\x1b[37m"

void render_node(cmark_node *node, int indent) {
    cmark_iter *iter = cmark_iter_new(node);
    cmark_event_type ev_type;
    int entering;

    while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
        entering = cmark_event_get_type(ev_type) == CMARK_EVENT_ENTER;

        switch (cmark_event_get_type(ev_type)) {
            case CMARK_EVENT_NONE:
                break;
            case CMARK_EVENT_DONE:
                break;
            case CMARK_EVENT_ENTER:
            case CMARK_EVENT_EXIT:
                for (int i = 0; i < indent; i++) {
                    putchar(' ');
                }

                cmark_node_type type = cmark_event_get_node_type(ev_type);

                switch (type) {
                    case CMARK_NODE_DOCUMENT:
                    case CMARK_NODE_BLOCK_QUOTE:
                        break;
                    case CMARK_NODE_LIST:
                        putchar(entering ? '\n' : ' ');
                        break;
                    case CMARK_NODE_ITEM:
                        putchar(entering ? '-' : '\n');
                        break;
                    case CMARK_NODE_CODE_BLOCK:
                        printf("%s%s\n%s%s", ANSI_CYAN, cmark_node_get_literal(node), ANSI_RESET, entering ? "" : "\n");
                        break;
                    case CMARK_NODE_HTML_BLOCK:
                        break;
                    case CMARK_NODE_PARAGRAPH:
                        putchar(entering ? '\n' : ' ');
                        break;
                    case CMARK_NODE_HEADING:
                        putchar(entering ? '\n' : ' ');
                        printf("%s", ANSI_BOLD);
                        for (int i = 0; i < cmark_node_get_heading_level(node); i++) {
                            putchar('#');
                        }
                        printf(" ");
                        break;
                    case CMARK_NODE_THEMATIC_BREAK:
                        printf("%s%s%s", ANSI_CYAN, entering ? "------------------------------------\n" : "", ANSI_RESET);
                        break;
                    case CMARK_NODE_TEXT:
                        printf("%s", entering ? "" : ANSI_RESET);
                        fwrite(cmark_node_get_literal(node), sizeof(char), cmark_node_get_literal(node)[0], stdout);
                        printf("%s", entering ? "" : ANSI_RESET);
                        break;
                    case CMARK_NODE_SOFTBREAK:
                        putchar('\n');
                        break;
                    case CMARK_NODE_LINEBREAK:
                        putchar('\n');
                        break;
                    case CMARK_NODE_CODE:
                        printf("%s%s%s", ANSI_CYAN, cmark_node_get_literal(node), ANSI_RESET);
                        break;
                    case CMARK_NODE_HTML_INLINE:
                        break;
                    case CMARK_NODE_CUSTOM_INLINE:
                        break;
                    case CMARK_NODE_EMPH:
                        printf("%s%s%s", entering ? ANSI_UNDERLINE : ANSI_RESET, entering ? "" : ANSI_RESET, entering ? "" : ANSI_UNDERLINE);
                        break;
                    case CMARK_NODE_STRONG:
                        printf("%s%s%s", entering ? ANSI_BOLD : ANSI_RESET, entering ? "" : ANSI_RESET, entering ? "" : ANSI_BOLD);
                        break;
                    case CMARK_NODE_LINK:
                        printf("%s%s", entering ? ANSI_BLUE : ANSI_RESET, cmark_node_get_url(node));
                        break;
                    case CMARK_NODE_IMAGE:
                        printf("%s%s", entering ? ANSI_MAGENTA : ANSI_RESET, cmark_node_get_url(node));
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    cmark_iter_free(iter);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <markdown file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Could not open file");
        return 1;
    }

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (buffer == NULL) {
        perror("Could not allocate memory for file buffer");
        fclose(file);
        return 1;
    }

    fread(buffer, size, 1, file);
    buffer[size] = '\0';

    cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);
