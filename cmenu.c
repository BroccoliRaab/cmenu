#include <stdio.h>
#include <string.h>
#include <curses.h>

#define DEFAULT_SEL A_STANDOUT

void get_string(char ** , size_t *);
int itemize(char***, char *);
void free_items(char ***, int);
void draw_items(WINDOW *, int,  char **, int, int);

int main (int argc, char *argv[]){
    
    size_t buff_size = sizeof(char)*20;
    char * buffer = (char * ) malloc(buff_size);

    int line;
    int ch;
    int sel;

    char **items = (char **) malloc(sizeof(char*));
    int n_items;
    const int SEL_ATTR  = DEFAULT_SEL;

    int padline =0;

    get_string(&buffer, &buff_size);
    n_items = itemize(&items, buffer);

    FILE *fd = fopen("/dev/tty", "r+");
    newterm(NULL, fd, fd);
    cbreak();

    refresh();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    WINDOW * main_pad = newpad(n_items, getmaxx(stdscr));

    line = 0;

    draw_items(main_pad, SEL_ATTR, items, n_items, line);

    prefresh(main_pad, padline,0,0,0,getmaxy(stdscr)-1, getmaxx(stdscr)-1);

    while (1){
        ch = getch();
        if (ch==KEY_UP||ch == 'k'||ch=='K'){
            if (line != 0) line--; 
            if(line<padline){
                padline = padline-getmaxy(stdscr);
                if (padline <0) padline = 0;
                clear();
                refresh();
            }
            draw_items(main_pad, SEL_ATTR, items, n_items, line);
            prefresh(main_pad, padline,0,0,0,getmaxy(stdscr)-1, getmaxx(stdscr)-1);
        }else if (ch==KEY_DOWN||ch == 'j'||ch=='J'){
            if (line !=(n_items-2)) line++;
            if(line>=(padline+getmaxy(stdscr))){
                padline = getmaxy(stdscr)+padline;
                clear();
                refresh();
            }
            draw_items(main_pad, SEL_ATTR, items, n_items, line);
            prefresh(main_pad, padline,0,0,0,getmaxy(stdscr)-1, getmaxx(stdscr)-1);
        }else if(ch=='\n' || ch==' '){
            sel = line;
            break;
        }
    }


    prefresh(main_pad, padline,0,0,0,getmaxy(stdscr)-1, getmaxx(stdscr)-1);
    
    endwin();

    printf("%s\n", items[sel]);
    free(buffer);
    free_items(&items, n_items);    
    return 0;
}

void get_string(char ** buff_ptr, size_t * cap){
    char c;
    int chars_read = 0;
    char* buff = *buff_ptr;
    while (c != EOF) {
        c = getchar();
        chars_read++;
        if (*cap<=(chars_read-1)){
            *cap = sizeof(char)*(chars_read+1);
            buff = realloc(buff, *cap);
        }
        buff[chars_read-1] = c;
    }
    buff[chars_read] = '\0';
    *buff_ptr = buff;
}

int itemize(char *** items, char * buff){
    char * str = strdup(buff);
    char * tok;
    char * rest;
    int i =0;

    for (tok = strtok_r(str, "\n", &rest);
         tok != NULL;
         tok = strtok_r(NULL, "\n", &rest)) { 

        (*items)[i] = strdup(tok);
        i++;
        *items = (char **) realloc(*items, sizeof(char*) * (i+1));
    }
    free(str);
    return i;
}

void free_items(char*** items, int n){
    int i;
    for (i=0;i<n;i++){
        free((*items)[i]);
    }
    free(*items);
}

void draw_items(WINDOW * w, int selattr, char ** items, int n, int sel){
    int i;
    wmove(w, 0, 0);
    for (i=0; i<n;i++){
        if (i==sel){
            wattron(w, selattr);
        }else{
            wattroff(w, selattr);
        }
        mvwprintw(w, i, 0,"%s", items[i]);
    }
}
