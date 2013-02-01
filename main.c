#include <stdlib.h>
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
    *self->body = '\0';
}

void Post_printf(Post *self) {
    printf("%s\n\n%s\n", self->author, self->body);
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
    new->body = malloc(1024);

    new->init = Block_copy( ^ { Post_init(new); });
    new->printf = Block_copy( ^ { Post_printf(new); });
    new->dealloc = Block_copy( ^ { Post_dealloc(new); });

    return new;
}

int main(void) {
    Post *post = Post_alloc();
    post->init();
    post->printf();
    post->dealloc();
    return 0;
}
