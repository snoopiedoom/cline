#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define DEV 1

/* C11 Standard: 7.142 
 * The header <signal.h> declare a type ... sig_atomic_t which is the 
 * (possibly volatile-qualified) integer type of an object that can be accessed
 * as an atomic entity, even in the presence of asynchronous interrupts.
 */
static volatile sig_atomic_t running = 1;

void sig_handler(int _) {
	(void)_;
	running = 0;
}
uint64_t get_posix_clock_time();

void line(int x0, int y0, int x1, int y1);

void pixel(int x, int y);

int main(int argc, char *argv[]) 
{
	int dx0 = 1;
	int dy0 = 1;

	int dx1 = -1;
	int dy1 = -1;

	int row, col;

	signal(SIGINT, sig_handler);

	// ncurses shit
	WINDOW* win = initscr();
	cbreak();
	noecho();
	nodelay(win, TRUE);
	curs_set(0);

	getmaxyx(stdscr, row, col);
	
	//mvprintw(0,0,"%d,%d", row, col);
	
	srandom(4);
	
	int	x0 = random() / ( RAND_MAX / row + 1);
	int	y0 = random() / ( RAND_MAX / col + 1);

	int	x1 = random() / ( RAND_MAX / row + 1);
	int	y1 = random() / ( RAND_MAX / col + 1);

	uint64_t tstart;
	int sleept = 25000;

	while (running) {

		int ch = getch() & A_CHARTEXT;
		if (ch == 75 || ch == 107)
				sleept += 1000;
		if (ch == 74 || ch == 106)
				sleept -= 1000;
		if (ch == 32)
			do {
				// Zzzz...
			} while (getch() != 32);
		if (ch == 114 || ch == 82) {
			clear();
			x0 = random() / ( RAND_MAX / row + 1);
			y0 = random() / ( RAND_MAX / col + 1);
			x1 = random() / ( RAND_MAX / row + 1);
			y1 = random() / ( RAND_MAX / col + 1);
			refresh();
		}

#if defined(_SC_MONOTONIC_CLOCK) && defined(DEV) 
	//	if (sysconf (_SC_MONOTONIC_CLOCK) > 0) {
	//		mvprintw(0, 0, "%s", "Cock ready");
	//		tstart = get_posix_clock_time();
	//	}
#endif
				
		if (x0 >= row -1 || x0 <= 0)
			dx0 = dx0 * -1;
		if (y0 >= col -1 || y0 <= 0)
			dy0 = dy0 * -1;
		if (x1 >= row -1 || x1 <= 0)
			dx1 = dx1 * -1;
		if (y1 >= col -1 || y1 <= 0)
			dy1 = dy1 * -1;
		
		x0 += dx0;
		y0 += dy0;
		x1 += dx1;
		y1 += dy1;
		
		line(x0, y0, x1, y1);
		
		usleep(sleept);
		
		refresh();
	}

	endwin();

	return EXIT_SUCCESS;
}


void pixel(int x, int y)
{
	move(x, y);
	int ch = inch() & A_CHARTEXT;
	if (ch == 64)
		printw("%c", 32);
	else
		printw("%c", 64);
}

void line(int x0, int y0, int x1, int y1)
{
	// Bresenham's line algorithm
	// y - y1 = (y2 - y1) / (x2 - x1) * (x - x1)
	int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;
	for (;;) {
		pixel(x0, y0);
		if (x0==x1 && y0==y1) break;
		e2 = err;     
		if (e2 >-dx) { 
			err -= dy; 
			x0 += sx;
		}     
		if (e2 < dy) { 
			err += dx; 
			y0 += sy;
		}   
	} 
}

uint64_t get_posix_clock_time ()
{
	struct timespec ts;
	if (clock_gettime (CLOCK_MONOTONIC, &ts) == 0)
		return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
	else
		return 0;
}
