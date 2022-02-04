#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#define CHAR_BUFF_MIN 400
#define ALLOC_CHECK(ptr)                               \
if ( (ptr) ==NULL){                                    \
	fputs("Failed to allocate heap memory.\n", stderr); \
	return 1;                                           \
}                                                      \

struct line_buffer {
	size_t len;
	size_t max;
	size_t current_index;
	wchar_t * buffer;
};
int read_lines(struct line_buffer *lb);
wchar_t * get_next_line (struct line_buffer *lb);

int main(){
	struct line_buffer lb;
	read_lines(&lb);
	wchar_t * s;
	while ((s=get_next_line(&lb))){
		fputws(s, stdout);
		fputwc(L'\n', stdout);
	}
	free(lb.buffer);
	return 0;
}

int read_lines(struct line_buffer *lb){

	lb->buffer = (wchar_t *) malloc( sizeof(wchar_t) * CHAR_BUFF_MIN);
	ALLOC_CHECK(lb->buffer);
	lb->len = 0;
	lb->max = CHAR_BUFF_MIN;
	lb->current_index = 0;
	wint_t c;

	while ((c=getwchar())!=WEOF){
		if (lb->len+1 >= lb->max){
			void * new_buff = realloc(lb->buffer, lb->max * 2*sizeof(wchar_t));
			if (new_buff){
				lb->buffer = (wchar_t *) new_buff;
				lb->max *= 2;
			}
			ALLOC_CHECK(new_buff);
		}
		c = c==L'\n'?L'\0':c;
		lb->buffer[lb->len] = (wchar_t)c;
		lb->len++;
	}
	lb->buffer[lb->len] = L'\0';
	lb->len++;

	return 0;
}

wchar_t * get_next_line (struct line_buffer *lb){
	wchar_t * line = lb->buffer + lb->current_index;
	if (lb->current_index >= lb->len){
		return NULL;
	}
	for (;lb->buffer[lb->current_index] != '\0'; lb->current_index++);
	lb->current_index++;
	return line;
}
