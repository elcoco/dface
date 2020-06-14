#include <X11/Xlib.h>

#include "blocks.h"

static void xsetroot(const char *name);
static void die(char* msg);
static void parse_args(int*, char** argv);
static uint8_t get_timeout();
static void usage();

static void die(char *msg) {
    printf("%s", msg);
    exit(0);
}

static void usage() {
    printf("dface, a statusline for DWM, © 2020 spambaconspam@protonmail.com, released under GPL3\n");
    printf("usage: dface [-t seconds] [-h]\n");
    exit(0);
}

static uint8_t get_timeout() {
    /* get the smallest timeout value defined in the blocks array */
    uint8_t timeout = 60;
    uint8_t bl = sizeof(blocks)/sizeof(blocks[0]);
    uint8_t i;

    for (i=0 ; i<bl ; i++) {
        Block block = blocks[i];
        if (block.timeout < timeout)
            timeout = block.timeout;
    }
    return timeout;
}

static void parse_args(int *argc, char **argv) {
    char c;

    while ((c=getopt(*argc, argv, "h:")) != -1) {
        switch(c) {
            case 'h':
                usage();    // exits

            case '?':       // for options not in optstring, and missing required arguments
                usage();    // exits

            default:
                printf("setting default\n");
                break;
        }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for (; optind < *argc; optind++){      
        char msg[50] = {'\0'};
        sprintf(msg, "ERROR: unknown argument: %s\n", argv[optind]);
        die(msg);  
    } 
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

int main(int argc, char **argv) {
    bool is_changed;
    uint8_t blen;
    uint8_t i;

    parse_args(&argc, argv);
    print_header();

    while (1) {
        is_changed = false;
        blen = sizeof(blocks)/sizeof(blocks[0]);

        // update all the statusses and check if they've changed
        for (i=0 ; i<blen ; i++) {
            if (blocks[i].get(&blocks[i]))
                is_changed = true;
        }

        if (is_changed) {
            //char status[MAXSTRING+1] = {'\0'};

            printf("[\n");
            for (i=0 ; i<blen ; i++) {
                Block block = blocks[i];

                // don't show block if it is empty
                if (strlen(block.text) > 0) {

                    // strip last comma + newline
                    if ( i == blen-1)
                        block.text[strlen(block.text)-2] = '\0';

                    printf("%s",block.text);
                }
            }

            printf("\n],\n");
            fflush(stdout);
        }

        sleep(get_timeout());
    }
    return 0;
}
