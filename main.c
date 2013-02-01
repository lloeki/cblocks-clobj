/*
 * Copyright © 2000 Loic Nageleisen <loic.nageleisen@gmail.com>
 *
 * This work is free. You can redistribute it and/or modify it under the terms
 * of the Do What The Fuck You Want To Public License, Version 2, as published
 * by Sam Hocevar. See the COPYING file for more details.
 *
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
 *
 *                                 ---
 *
 * Fooling around with Blocks in C to create pseudo-classes and -instances.
 *
 * In this case instance-bound methods are closures which happen to be stored
 * in the item they're bound to.
 *
 * Unfortunately, although the compiler knows the difference, a function and a
 * type having the same name is not allowed, else we could just have a nice
 * sugar Post() function doing alloc() and init().
 *
 * Requires (obviously) clang or Apple's GCC block patches.
 *
 *
 *           --- And now for something completely different ---
 */

#define BODY                                                                   \
 " The venerable master Qc Na was walking with his student, Anton. Hoping to  "\
 " prompt the master into a discussion, Anton said “Master, I have heard that "\
 " objects are a very good thing - is this true?” Qc Na looked pityingly at   "\
 " his student and replied, “Foolish pupil - objects are merely a poor man's  "\
 " closures.”                                                                 "\
 "                                                                        \n\n"\
 " Chastised, Anton took his leave from his master and returned to his cell,  "\
 " intent on studying closures. He carefully read the entire ”Lambda: The     "\
 " Ultimate...” series of papers and its cousins, and implemented a small     "\
 " Scheme interpreter with a closure-based object system. He learned much, and"\
 " looked forward to informing his master of his progress.                    "\
 "                                                                        \n\n"\
 " On his next walk with Qc Na, Anton attempted to impress his master by      "\
 " saying “Master, I have diligently studied the matter, and now understand   "\
 " that objects are truly a poor man's closures.” Qc Na responded by hitting  "\
 " Anton with his stick, saying ”When will you learn? Closures are a poor     "\
 " man's object.“ At that moment, Anton became enlightened.                   "\

#define AUTHOR                                      "Anton van Straaten"

/*
 * from http://people.csail.mit.edu/gregs/ll1-discuss-archive-html/msg03277.html
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Block.h>


struct Post_s {
    char *author;
    char *body;

    void (^init)();
    void (^printf)();
    void (^dealloc)();
};
typedef struct Post_s Post;


void Post_init(Post *self) {
    *self->author = '\0';
    *self->body   = '\0';
}


void Post_printf(Post *self) {
    char *nosp = NULL;
    for (char *buf = self->body; *buf != '\0'; buf++) {
        if (*buf != ' ') {
            if (*buf == '\n') {
                printf("\n");
            } else {
                nosp = nosp == NULL ? buf : nosp;
            }
        } else {
            if (nosp != NULL && *(buf+1) == ' ') {
                fwrite(nosp, sizeof(char), buf+1 - nosp, stdout);
                nosp = NULL;
            }
        }
    }
    printf("\n\n-- %s\n", self->author);
}


void Post_dealloc(Post *self) {
    Block_release(self->dealloc);
    Block_release(self->printf);
    Block_release(self->init);

    free(self->body);
    free(self->author);
    free(self);
}


Post *Post_alloc() {
    Post *new = malloc(sizeof(Post));
    new->author = malloc(255);
    new->body = malloc(2048);

    new->init = Block_copy( ^ { Post_init(new); });
    new->printf = Block_copy( ^ { Post_printf(new); });
    new->dealloc = Block_copy( ^ { Post_dealloc(new); });

    return new;
}


int main(void) {
    Post *post = Post_alloc();
    post->init();

    strcpy(post->author, AUTHOR);
    strcpy(post->body, BODY);
    post->printf();

    post->dealloc();

    return 0;
}
