#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#define _XOPEN_SOURCE_EXTENDED
#include <curses.h>

#define CHAR_BUFF_MIN 400
#define SEL_ATTR A_STANDOUT

struct line_buffer {
	size_t len;
	size_t max;
	size_t current_index;
	wchar_t * buffer;
};

int read_lines(struct line_buffer *lb);
wchar_t * get_next_line (struct line_buffer *lb);
wchar_t * draw_lines(WINDOW * pad, struct line_buffer *lb, size_t sel_index);

int main(){
	struct line_buffer lb;
	int n_items = read_lines(&lb);
	if (n_items<1){
		fputs("Failed to read from stdin. Exiting.\n", stderr);
		goto ERR_EXIT;
	}

	FILE *fd = fopen("/dev/tty", "r+");
	if (fd == NULL){
		fputs( "Failed to open tty. Exiting\n", stderr);
		goto ERR_F;
	}

	SCREEN * term = newterm(NULL, fd, fd);
	if (term == NULL){
		fputs( "Failed to make new term from tty. Exiting\n", stderr);
		goto ERR_FB;
	}

	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	int selection_index = 0;
	int padline = 0;
	int previous_padline = 0;
	int result;
	wchar_t * selection = L"";

	WINDOW *main_pad = newpad(n_items, getmaxx(stdscr));
	if (main_pad == NULL){
		fputs( "Failed to initialize pad. Exiting\n", stderr);
		goto ERR_FBW;
	}

	selection = draw_lines(stdscr, &lb, selection_index);
	if (selection == NULL){
		fputs( "Failed to draw to pad. Exiting\n", stderr);
		goto ERR_FBWP;
	}

	result = prefresh(main_pad, padline,0,0,0, getmaxy(stdscr)-1, getmaxx(stdscr)-1);
	if (result == ERR){
		fputs( "Failed to refresh pad. Exiting\n", stderr);
		goto ERR_FBWP;
	}

	int input = 0;
	while (input != '\n' && input != ' '){
		input = getch();
		switch (input){
			case KEY_DOWN:
			case 'j':
			case 'J':
				selection_index++;
				break;
			case KEY_UP:
			case 'k':
			case 'K':
				selection_index--;
				break;
		}

		selection_index = selection_index<0?0:selection_index;
		selection_index = selection_index>n_items-1?n_items-1:selection_index;

		padline = (selection_index/getmaxy(stdscr))*getmaxy(stdscr);
		if (padline != previous_padline){
			clear();
			result = refresh();
			if (result == ERR){
				fputs( "Failed to refreshing screen. Exiting\n", stderr);
				goto ERR_FBWP;
			}
			previous_padline = padline;
		}

		selection = draw_lines(main_pad, &lb, selection_index);
		if (selection == NULL){
			fputs( "Failed to draw to pad. Exiting\n", stderr);
			goto ERR_FBWP;
		}
		result = prefresh(main_pad, padline,0,0,0, getmaxy(stdscr)-1, getmaxx(stdscr)-1);
		if (result == ERR){
			fputs( "Failed to refreshing pad. Exiting\n", stderr);
			goto ERR_FBWP;
		}
	}

	fputws(selection, stdout);
	fputwc(L'\n', stdout);

	delwin(main_pad);
	endwin();
	free(lb.buffer);
	fclose(fd);
	return 0;

ERR_FBWP:
	delwin(main_pad);
ERR_FBW:
	endwin();
ERR_FB:
	free(lb.buffer);
ERR_F:
	fclose(fd);
ERR_EXIT:
	return -1;
}

int read_lines(struct line_buffer *lb){

	lb->buffer = (wchar_t *) malloc( sizeof(wchar_t) * CHAR_BUFF_MIN);
	if (lb->buffer == NULL){
		fputs("Failed to allocate buffer\n", stderr);
		return -1;
	}
	lb->len = 0;
	lb->max = CHAR_BUFF_MIN;
	lb->current_index = 0;
	wint_t c;
	int line_n = 0;

	while ((c=getwchar())!=WEOF){
		if (lb->len+1 >= lb->max){
			void * new_buff = realloc(lb->buffer, lb->max * 2*sizeof(wchar_t));
			if (new_buff){
				lb->buffer = (wchar_t *) new_buff;
				lb->max *= 2;
			}else {
				fputs("Failed to allocate buffer\n", stderr);
				return -1;
			}
		}
		if (c == L'\n'){
			c=L'\0';
			line_n++;
		}

		lb->buffer[lb->len] = (wchar_t)c;
		lb->len++;
	}
	lb->buffer[lb->len] = L'\0';
	lb->len++;

	return line_n;
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

wchar_t * draw_lines(WINDOW * w, struct line_buffer *lb, size_t sel_index){
	wchar_t * selection;
	wchar_t * line;
	lb->current_index = 0;
	for (size_t i = 0; (line = get_next_line(lb)) !=NULL; i++){
		if (i == sel_index){
			wattron(w, SEL_ATTR);
			selection = line;
		}else {
			wattroff(w, SEL_ATTR);
		}
		mvwaddwstr(w, i, 0, line);
	}
	return selection;
}
