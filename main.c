/*

    Typing Speed Calculator
    Copyleft 2020 by Erich Kohl
    Version 2.01 (original versions written in BASIC)

*/

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "timer.h"

void init(void);
void main_loop(void);
void timer_handler(void);
int title_screen(void);
void wrap_up(void);

int tenths;
int timer_failure;

void init(void)
{
    initscr();
    noecho();
    curs_set(FALSE);
    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    clear();
    refresh();
    timer_failure = 0;
}

void main_loop(void)
{
    WINDOW *winTypingArea;
    WINDOW *winResults;
    char results1[] = "R E S U L T S";
    char results2[] = "PRESS ENTER TO REPEAT OR ESC TO QUIT";
    int totalWords;
    int totalChars;
    int totalSpaces;
    int kOld;
    int y;
    int x;
    int k;

    winTypingArea = newwin(6, 50, 2, 15);
    winResults = newwin(12, 60, 10, 10);

    refresh();

    nodelay(winTypingArea, TRUE);

    do
    {
        wborder(winTypingArea, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_ULCORNER | COLOR_PAIR(2) | A_BOLD, ACS_URCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LLCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LRCORNER | COLOR_PAIR(2) | A_BOLD);
        wrefresh(winTypingArea);
        wborder(winResults, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_ULCORNER | COLOR_PAIR(2) | A_BOLD, ACS_URCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LLCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LRCORNER | COLOR_PAIR(2) | A_BOLD);
        wrefresh(winResults);

        wattron(winTypingArea, COLOR_PAIR(4) | A_BOLD);
        wmove(winTypingArea, 1, 1);
        waddstr(winTypingArea, "<READY>");
        wrefresh(winTypingArea);

        wattron(winResults, COLOR_PAIR(3) | A_BOLD);
        wmove(winResults, 1, (60 - strlen(results1)) / 2);
        waddstr(winResults, results1);
        wrefresh(winResults);

        refresh();

        y = x = 1;
        kOld = 0;
        totalWords = totalChars = totalSpaces = 0;

        tenths = 0;

        while(1)
        {
            k = wgetch(winTypingArea);
            if(k == 10)
            {
                if(!(y == 1 && x == 1))
                    if(tenths >= 1)
                        break;
            }
            if(k == 27)
                break;
            if(k >= 32 && k <= 126)
            {
                if(y == 1 && x == 1)
                {
                    if(k == 32)
                        continue;
                    wattron(winTypingArea, COLOR_PAIR(3) | A_BOLD);
                    wmove(winTypingArea, 1, 1);
                    waddstr(winTypingArea, "       ");
                    wrefresh(winTypingArea);
                    refresh();
                    if(start_timer(100, &timer_handler))
                    {
                        timer_failure = 1;
                        break;
                    }
                }
                wmove(winTypingArea, y, x);
                waddch(winTypingArea, k);
                wrefresh(winTypingArea);
                refresh();
                if(k > 32)
                {
                    if(kOld == 32)
                        totalWords += 1;
                    totalChars += 1;
                }
                else if(k == 32)
                {
                    totalSpaces += 1;
                }
                kOld = k;
                x += 1;
                if(x == 49)
                {
                    y += 1;
                    x = 1;
                }
                if(y == 5)
                    break;
            }
            if(tenths >= 6000)
                break;
        }

        if(timer_failure == 1)
            break;

        if(k == 27)
            break;

        stop_timer();

        totalWords += 1;

        wmove(winResults, 3, 2);
        waddstr(winResults, "Total time (in seconds):");
        wmove(winResults, 4, 2);
        waddstr(winResults, "Total number of words:");
        wmove(winResults, 5, 2);
        waddstr(winResults, "Total number of characters:");
        wmove(winResults, 6, 2);
        waddstr(winResults, "Total number of spaces:");
        wmove(winResults, 7, 2);
        waddstr(winResults, "Words per minute (time X words):");
        wmove(winResults, 8, 2);
        waddstr(winResults, "Words per minute (time X chars / 5):");

        wattron(winResults, COLOR_PAIR(5) | A_BOLD);
        wmove(winResults, 3, 50);
        wprintw(winResults, "%8.1f", (float)tenths / 10.0);
        wmove(winResults, 4, 50);
        wprintw(winResults, "%8i", totalWords);
        wmove(winResults, 5, 50);
        wprintw(winResults, "%8i", totalChars);
        wmove(winResults, 6, 50);
        wprintw(winResults, "%8i", totalSpaces);
        wmove(winResults, 7, 50);
        wprintw(winResults, "%8.2f", (600.0 / (float)tenths) * (float)totalWords);
        wmove(winResults, 8, 50);
        wprintw(winResults, "%8.2f", (600.0 / (float)tenths) * (float)((float)totalChars / 5.0));
        wattron(winResults, COLOR_PAIR(3) | A_BOLD);
        wmove(winResults, 10, (60 - strlen(results2)) / 2);
        waddstr(winResults, results2);

        wrefresh(winResults);

        refresh();

        do
        {
            k = getch();
        }
        while(k != 10 && k != 27);

        if(k == 10)
        {
            timer_failure = 0;
            wclear(winTypingArea);
            wrefresh(winTypingArea);
            wclear(winResults);
            wrefresh(winResults);
            refresh();
        }
    }
    while(k == 10);

    delwin(winTypingArea);
    delwin(winResults);
}

void timer_handler(void)
{
    tenths++;
}

int title_screen()
{
    chtype bkgd_fill;
    unsigned char block = 255;
    WINDOW *winTitle;
    char title1[] = "Typing Speed Calculator";
    char title2[] = "Copyleft 2020 by Erich Kohl";
    char title3[] = "Version 2.01";
    WINDOW *winInstructions;
    char inst1[] = "INSTRUCTIONS";
    char inst2[] = "This program tells you how fast you can type.";
    char inst3[] = "When the next screen appears, you will see two windows.";
    char inst4[] = "The top is where you type, the bottom displays the results.";
    char inst5[] = "Start typing a paragraph when you see <READY> displayed.";
    char inst6[] = "Hit Enter when done.  You can repeat as necessary.";
    char inst7[] = "PRESS ENTER TO PROCEED OR ESC TO QUIT";
    int k;

    bkgd_fill = block | COLOR_PAIR(1);
    bkgd(bkgd_fill);

    winTitle = newwin(5, 40, 2, 20);
    refresh();

    wborder(winTitle, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_ULCORNER | COLOR_PAIR(2) | A_BOLD, ACS_URCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LLCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LRCORNER | COLOR_PAIR(2) | A_BOLD);

    wattron(winTitle, COLOR_PAIR(3) | A_BOLD);
    wmove(winTitle, 1, (40 - strlen(title1)) / 2);
    waddstr(winTitle, title1);
    wmove(winTitle, 2, (40 - strlen(title2)) / 2);
    waddstr(winTitle, title2);
    wmove(winTitle, 3, (40 - strlen(title3)) / 2);
    waddstr(winTitle, title3);
    wrefresh(winTitle);

    winInstructions = newwin(11, 64, 9, 8);
    refresh();

    wborder(winInstructions, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_VLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_HLINE | COLOR_PAIR(2) | A_BOLD, ACS_ULCORNER | COLOR_PAIR(2) | A_BOLD, ACS_URCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LLCORNER | COLOR_PAIR(2) | A_BOLD, ACS_LRCORNER | COLOR_PAIR(2) | A_BOLD);

    wattron(winInstructions, COLOR_PAIR(3) | A_BOLD);
    wmove(winInstructions, 1, (64 - strlen(inst1)) / 2);
    waddstr(winInstructions, inst1);
    wmove(winInstructions, 3, (64 - strlen(inst2)) / 2);
    waddstr(winInstructions, inst2);
    wmove(winInstructions, 4, (64 - strlen(inst3)) / 2);
    waddstr(winInstructions, inst3);
    wmove(winInstructions, 5, (64 - strlen(inst4)) / 2);
    waddstr(winInstructions, inst4);
    wmove(winInstructions, 6, (64 - strlen(inst5)) / 2);
    waddstr(winInstructions, inst5);
    wmove(winInstructions, 7, (64 - strlen(inst6)) / 2);
    waddstr(winInstructions, inst6);
    wmove(winInstructions, 9, (64 - strlen(inst7)) / 2);
    waddstr(winInstructions, inst7);
    wrefresh(winInstructions);

    refresh();

    do
    {
        k = getch();
    }
    while(k != 10 && k!= 27);

    delwin(winTitle);
    delwin(winInstructions);
    clear();

    if(k == 10)
        return 0;
    else
        return 1;
}

void wrap_up(void)
{
    chtype bkgd_fill;

    bkgd_fill = ' ' | COLOR_PAIR(6);
    bkgd(bkgd_fill);
    clear();
    refresh();
    endwin();
    if(timer_failure == 1)
    {
        printf("\n\nError: Timer failure.\n\n");
        getchar();
    }

}

int main()
{
    init();
    if(title_screen() == 0)
        main_loop();
    wrap_up();
    return 0;
}
