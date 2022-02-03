#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define CHAR_BUFF_MIN 400
#define ALLOC_CHECK(ptr)                               \
if ( (ptr) ==NULL){                                    \
	fputs("Failed to allocate heap memory.\n", stderr); \
	return 1;                                           \
}                                                      \

struct line_buffer {
	size_t len;
	size_t max;
	char * buffer;
	char * current;
};
int read_lines(struct line_buffer *lb){

	lb->buffer = (char *) malloc( sizeof(char) * CHAR_BUFF_MIN);
	ALLOC_CHECK(lb->buffer);
	lb->len = 0;
	lb->max = CHAR_BUFF_MIN;
	lb->current = lb->buffer;
	int c;
	do{
		c= getchar();
		if (lb->len >= lb->max){
			void * new_buff = realloc(lb->buffer, lb->max * 2);
			if (new_buff){
				lb->buffer = (char *) new_buff;
				lb->current = lb->buffer;
				lb->max *= 2;
			}
			ALLOC_CHECK(new_buff);
		}
		c = c=='\n'?'\0':c;
		lb->buffer[lb->len] = (char)c;
		lb->len++;
	}while(c != EOF);
	return 0;
}

char * get_next_line (struct line_buffer *lb){
	if (lb->current - lb->buffer >= lb->len){
		printf("%p, %p, %lu\n", lb->current,lb->buffer , lb->len);
		return NULL;
	}
	for (;*(lb->current) != '\0'; lb->current++);
	lb->current ++;
	return lb->current;
}

int main(){
	struct line_buffer lb;
	read_lines(&lb);
	char * s = get_next_line(&lb);
	if (s)
		puts(get_next_line(&lb));
	s = get_next_line(&lb);
	if (s)
		puts(get_next_line(&lb));
	free(lb.buffer);
	return 0;
}
