#include <curses.h>
#include <iostream.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include <curses.h> /* 引进 curses.h , 并自动引进 
stdio.h */ 

void initial(); 

main() 
{ 
initial();
WINDOW *my_window;
my_window = newwin(10,40,2,6);

box(my_window,'|','-'); 

wrefresh(my_window);

attron(A_REVERSE); 

mvwaddstr(my_window,9,39,"Curses Program"); 

attroff(A_REVERSE); 
wrefresh(my_window);
refresh();
endwin();
}; 

void initial() /* 自定开启 curses 函式 
*/ 

{ 
initscr(); 
cbreak(); 
nonl(); 
noecho(); 
intrflush(stdscr,FALSE); 
keypad(stdscr,TRUE); 
refresh(); 
}; 

