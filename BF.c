/* 
 * BF(Brainf***) language implementation
 * Started on: Sat Jul  3 EDT 2016
 * Tested to work on: Sun Jul  3 22:27:25 EDT 2016
 * Used non-recursive(iterative) approach for finding, jumping to matching braces
 * operators: + - .  , > < [ ]
 */
/*
 * Sun Aug 14 19:11:55 EDT 2016
 * fixed return code from main to use return code of process_code()
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CODE_LEN (10000)
#define MAX_DATA_LEN (30000) // original BF had this size ?...
static char code[MAX_CODE_LEN];
static char data[MAX_DATA_LEN];

/* 
 * return code:
 * error: 1
 * success: 0
 */
int process_code(int code_len)
{
    char tmp;
    int depth;
    int r;

    for (int c = 0 /* code pointer */, d = 0 /* data pointer */
            ; c < code_len
            ; ++c) {
        switch (code[c]) {
            /* increment the data byte by one */
            case '+':
                data[d] += 1;
                break;

            /* decrement the data byte by one */
            case '-':
                data[d] -= 1;
                break;

            /* print the data byte */
            case '.':
                if (write(STDOUT_FILENO, &data[d], 1) != 1) {
                    perror("write");
                    return 1;
                }
                break;

            /* read one byte of input and store it in the data byte */
            case ',':
                if ((r = read(STDIN_FILENO, &tmp, 1)) != 1) {
                    perror("read");
                    fprintf(stderr, "Error: read only %d bytes\n", r);
                    return 1;
                }
                data[d] = tmp;
                break;

            /* increment the data pointer by one */
            case '>':
                if ( !(++d < MAX_DATA_LEN) ) {
                    fprintf(stderr, "Error: data pointer reached the the right limit of data segment\n");
                    return 1;
                }
                break;

            /* decrement the data pointer by one */
            case '<':
                if ( !(--d > -1) ) {
                    fprintf(stderr, "Error: data pointer reached the the left limit of data segment\n");
                    return 1;
                }
                break;

            /* if the data byte is zero, jump forward the code pointer to one byte after the matching bracket ']' */
            case '[':
                if (data[d] == 0) {
                    depth = 0;
                    for (;;) { // move the code pointer to the matching ']'
                        if ( !(++c < code_len) ) {
                            fprintf(stderr, "Error: code pointer reached the the right limit of code segment\n");
                            return 1;
                        }
                        if ( !(code[c] == ']' && depth == 0) ) {
                            if (code[c] == '[') ++depth;
                            if (code[c] == ']') --depth;
                            continue;
                        }
                        break;
                    }
                }
                break;

            /* If the data byte is nonzero, jump backwards the code pointer to one byte after the matching bracket '[' */
            case ']': 
                if (data[d] != 0) {
                    depth = 0;
                    for (;;) { // move the code pointer to the matching '['
                        if ( !(--c > -1) ) {
                            fprintf(stderr, "Error: code pointer reached the the left limit of code segment\n");
                            return 1;
                        }
                        if ( !(code[c] == '[' && depth == 0) ) {
                            if (code[c] == ']') ++depth;
                            if (code[c] == '[') --depth;
                            continue;
                        }
                        break;
                    }
                }
                break;

            /* ignore the new line character */
            case '\n':
                break;

            /* handle unknown input */
            default:
                fprintf(stderr, "Error: unknown character '%c'\n", code[c]);
                return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    int code_len;

    if (argc != 1+1) {
        fprintf(stderr, "Usage: %s code_file\n", argv[0]);
        return 1;
    }

    /* read the input file into memory */ {
        int fd;

        if ((fd = open(argv[1], O_RDONLY)) == -1) {
            perror("open");
            return 1;
        }

        if ((code_len = read(fd, code, MAX_CODE_LEN)) == -1) {
            perror("read");
            return 1;
        }

        close(fd);
    }

    return process_code(code_len);
}
