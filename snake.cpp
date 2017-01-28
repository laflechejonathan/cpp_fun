#include <iostream>
#include <curses.h>
#include <string>
#include <deque>
#include <algorithm>
#include <random>
#include <unistd.h>
#include <signal.h>

/**
 *
 * Stupid program that generates a maze using a randomized DFS,
 * renders it to the terminal using ncurses. Likely won't work
 * on Windows.
 *
 */

using namespace std;

auto RAND_ENGINE = default_random_engine(random_device{}());

volatile bool BREAK_RENDER = false;


enum Direction {
    WEST = 0,
    EAST = 1,
    NORTH = 2,
    SOUTH = 3,
    NONE = 4,
};

enum Color {
    RED_ON_BLACK = 1,
    BLACK = 2,
    RED = 3,
    GREEN = 4,
    BLUE = 5
};

struct Position {
    int x;
    int y;
    Direction heading;

    Position() {
    }

    Position(int x, int y, Direction d) :
        x(x), y(y), heading(d) {
    } 

    bool operator == (const Position &p) const {
        return x == p.x && y == p.y;
    }

};

class NCurseWindow {

public:
        NCurseWindow() {
        // ncurses initialization
        initscr();
        cbreak();
        noecho();
        curs_set(0);
        keypad(stdscr, TRUE);
        timeout(50);

        // stores screen dimensions in height/width vars
        getmaxyx(stdscr, height, width);
        
        // set up some colors to be used later
        start_color();
        init_pair(RED_ON_BLACK, 1, 0); // red on black
        init_pair(BLACK, 0, 0); // black
        init_pair(RED, 1, 1); // red
        init_pair(GREEN, 2, 2); // green
        init_pair(BLUE, 3, 4); // blue

        for (int i = 0; i < height; ++i) {
            print(string(width, ' '), BLACK);
        }
    }

    /* ncurses utils */
    void renderBorder() {
        wattron(stdscr, COLOR_PAIR(5));
        box(stdscr, 1, 1);
        wattron(stdscr, COLOR_PAIR(BLACK));
    }

    void move(int x, int y) {
        wmove(stdscr, y, x);
    }

    int get_color(int x, int y) {
        return (mvwinch(stdscr, y, x) & A_COLOR);
    }

    void refresh() {
        wrefresh(stdscr);
    }

    void print(const string& s, Color color) {
        wattron(stdscr, COLOR_PAIR(color));
        waddstr(stdscr, s.c_str());
    }

    void print(const char c, Color color) {
        addch(c | COLOR_PAIR(color));
    }

    int center(const string& s) {
        return (width - s.size()) / 2;
    }

    int center(int size) {
        return (width - size) / 2;
    }

    int height;
    int width;
};


class Snake: public NCurseWindow {

public:

    Snake() {
        nodes.push_front(Position(10, 5, Direction::EAST));
        random_width = uniform_int_distribution<int>(5, width - 5);
        random_height = uniform_int_distribution<int>(5, height - 5);
        multiplier = 1.0;
        eat_food();
    }

    void renderHeader() {
        string header[] = {
            "==============================",
            "Welcome to CLI Snake!!",
            "==============================",
        };

        int i = 1;
        for (auto& l: header) {
            move(center(l), i++);
            print(l, RED_ON_BLACK);
        }
    }

    void renderGameOver() {
        string header[] = {
            "==============================",
            "Sorry, GAME OVER",
            "==============================",
        };

        int i = 1;
        for (auto& l: header) {
            move(center(l), i++);
            print(l, RED_ON_BLACK);
        }
    }


    void render() {
        renderHeader();
        renderBorder();

        int character = ' ';
        while (step(character) && !BREAK_RENDER) {
            refresh();
            character = getch();
        }

        clear();
        renderGameOver();
        refresh();
    }

    bool step(int last_command) {
        Position current = nodes.front();

        if (last_command == KEY_UP && (current.heading == EAST || current.heading == WEST)) {
            current.heading = NORTH;
        } else if (last_command == KEY_DOWN && (current.heading == EAST || current.heading == WEST)) {
            current.heading = SOUTH;
        } else if (last_command == KEY_LEFT && (current.heading == NORTH || current.heading == SOUTH)) {
            current.heading = WEST;
        } else if (last_command == KEY_RIGHT && (current.heading == NORTH || current.heading == SOUTH)) {
            current.heading = EAST;
        } else if (last_command == ' ') {
            timeout(-1);
            getch();
        }

        switch(current.heading) {
            case WEST:
                current.x--;
                timeout(int(50 * multiplier));
                break;
            case EAST:
                current.x++;
                timeout(int(50 * multiplier));
                break;
            case NORTH:
                current.y--;
                timeout(int(125 * multiplier));
                break;
            case SOUTH:
                current.y++;
                timeout(int(125 * multiplier));
                break;
            case NONE:
                break;
        }

        nodes.push_front(current);
        move(current.x, current.y);

        if (get_color(current.x, current.y) == COLOR_PAIR(BLUE) ||
            get_color(current.x, current.y) == COLOR_PAIR(RED)) {
            return false;
        }

        print(' ', RED);

        if (current == food) {
            eat_food();
        }

        eating = max(0, eating - 1);
        if (eating) {
            return true;
        }

        Position last = nodes.back();
        nodes.pop_back();
        move(last.x, last.y);
        print(' ', BLACK);

        switch(last.heading) {
            case WEST:
                last.x++;
                break;
            case EAST:
                last.x--;
                break;
            case NORTH:
                last.y--;
                break;
            case SOUTH:
                last.y++;
                break;
            case NONE:
                break;
        }

        return true;
    }

    void eat_food() {
        int x = random_width(RAND_ENGINE);
        int y = random_height(RAND_ENGINE);
        food = Position(x, y, NONE);
        move(x, y);
        print(' ', GREEN);
        eating = 10;
        multiplier *= 0.9;
    }

private:

    Position food;
    int eating;
    float multiplier;

    uniform_int_distribution<int> random_height;
    uniform_int_distribution<int> random_width;
    deque<Position> nodes;
};

/* Awkwardly try to react to screen change events */
void resizeHandler(int) {
    endwin();
    clear();
    refresh();
    BREAK_RENDER = true;
}

void responsiveRenderLoop() {
    signal(SIGWINCH, resizeHandler);
    while (true) {
        Snake s;
        s.render();

        while (!BREAK_RENDER) {
            usleep(5 * 1000);
        }

        BREAK_RENDER = false;
    }
}

int main() {
    responsiveRenderLoop();
}

