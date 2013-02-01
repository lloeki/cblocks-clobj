#include <stdlib.h>
#include <stdio.h>
#include <Block.h>

struct Post_s {
    char author[255];
    char body[2048];
};
typedef struct Post_s Post;

Post *Post_alloc() {
    return malloc(sizeof(Post));
}

void Post_init(Post *self) {
    *self->author = '\0';
    *self->body = '\0';
}

void Post_dealloc(Post *self) {
    free(self);
}

int main(void) {
    Post *post = Post_alloc();
    printf("%s\n\n%s\n", post->author, post->body);
    Post_dealloc(post);
    return 0;
}
