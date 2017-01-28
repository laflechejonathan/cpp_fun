#include <iostream>
#include <curses.h>
#include <string>
#include <vector>
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

struct Position {
    int x;
    int y;

    Position() {
    }

    Position(int x, int y) :
        x(x), y(y) {
    } 

    bool operator == (const Position &p) const {
        return x == p.x && y == p.y;
    }

    int distance(const Position& p) {
        return abs(x - p.x) + abs(y - p.y);
    }
};

class Maze {

public:

    Maze() {
        // ncurses initialization
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);

        // stores screen dimensions in height/width vars
        getmaxyx(stdscr, height, width);
        
        // set up some colors to be used later
        start_color();
        init_pair(1, 1, 0); // red on black
        init_pair(2, 0, 0); // black
        init_pair(3, 1, 1); // red
        init_pair(4, 2, 2); // green
        init_pair(5, 3, 4); // blue
        wattron(stdscr, COLOR_PAIR(1));

        // setup maze based on screen size
        maze.resize(width);
        for (int i = 0; i < width; ++i) {
            maze[i].resize(height);
        }

        // Initialize start and goal based on screen size
        // (offset is to keep a margin for prettiness)
        start_node = Position(OFFSET, OFFSET);
        goal_node = Position(width - OFFSET, height - OFFSET);
    }

    void renderHeader() {
        string header[] = {
            "==============================",
            "Welcome to MazeRunner 2000!!",
            "==============================",
        };

        int i = 1;
        for (auto& l: header) {
            move(center(l), i++);
            print(l);
        }
    }

    void renderMaze() {
        // render each node in the maze according to its state
        for (int i = OFFSET; i <= width - OFFSET; ++ i) {
            for (int j = OFFSET; j <= height - OFFSET; ++j) {
                move(i, j);
                switch (maze[i][j]) {
                    case MazeNode::UNCARVED:
                        addch(' ' | COLOR_PAIR(2));
                        break;
                    case MazeNode::CARVED:
                        addch(' ' | COLOR_PAIR(3));
                        break;
                    case MazeNode::SEARCHED:
                        addch(' ' | COLOR_PAIR(4));
                        break;
                    case MazeNode::SOLVED:
                        addch(' ' | COLOR_PAIR(5));
                        break;
                }
            }
        }

        // goal / start node get special treatment
        move(start_node.x, start_node.y);
        addch(' ' | COLOR_PAIR(5));
        move(goal_node.x, goal_node.y);
        addch(' ' | COLOR_PAIR(5));

        // actually renders the changes
        wrefresh(stdscr);
    }

    bool search() {
        return search(start_node, goal_node);
    }

    void randomMaze() {
        randomMaze(start_node, goal_node);
    }

private:

    bool search(Position current, Position destination) {
        // render search 'in real time' - tweak this to speed/slow animation
        renderMaze();
        usleep(500);

        if (current == destination || BREAK_RENDER) {
            // reached position
            return true;
        }
        
        if (outOfBounds(current) ||
            maze[current.x][current.y] != MazeNode::CARVED) {
            return false;
        }

        maze[current.x][current.y] = MazeNode::SEARCHED;

        for (auto& direction: shuffle_directions()) {
            Position new_pos(current.x + direction.x, current.y + direction.y);
            if (search(new_pos, destination)) {
                maze[new_pos.x][new_pos.y] = MazeNode::SOLVED;
                return true;
            }
        }

        return false;
    }

    void randomMaze(Position current, Position destination) {
        if (outOfBounds(current) ||
            exceedsNeighbors(current) ||
            current == destination ||
            maze[current.x][current.y] == MazeNode::CARVED ||
            BREAK_RENDER) {
            return;
        }

        maze[current.x][current.y] = MazeNode::CARVED;

        for (auto& direction: shuffle_directions()) {
            Position new_pos(current.x + direction.x, current.y + direction.y);
            randomMaze(new_pos, destination);
        }
    }

    vector<Position> shuffle_directions() {
        vector<Position> copy(directions);
        std::shuffle(copy.begin(), copy.end(), RAND_ENGINE);
        return copy;
    }

    bool exceedsNeighbors(Position pos) {
        // for maze creation, we want to avoid fully connecting paths
        // but rule doesn't apply near edges
        if (pos.distance(start_node) < OFFSET ||
            pos.distance(goal_node) < OFFSET) {
            return false;
        } else {
            int neighbor_count = 0;
            for (auto& d: directions) {
                neighbor_count += maze[pos.x + d.x][pos.y + d.y] ? 1 : 0;
            }
            return neighbor_count > 1;
        }
    }

    bool outOfBounds(Position pos) {
        return pos.x < start_node.x ||
            pos.x > goal_node.x ||
            pos.y < start_node.y ||
            pos.y > goal_node.y;
    }


    enum MazeNode {
        UNCARVED = 0,
        CARVED = 1,
        SEARCHED = 2,
        SOLVED = 3,
    };

    /* ncurses utils */

    void move(int x, int y) {
        wmove(stdscr, y, x);
    }

    void print(const string& s) {
        waddstr(stdscr, s.c_str());
    }

    int center(const string& s) {
        return (width - s.size()) / 2;
    }

    int center(int size) {
        return (width - size) / 2;
    }

    int height;
    int width;

    Position start_node;
    Position goal_node;

    vector<vector<MazeNode>> maze;
    static vector<Position> directions;
    static const int OFFSET = 5;
};

vector<Position> Maze::directions = {
    Position(0, 1),
    Position(1, 0),
    Position(0, -1),
    Position(-1, 0),
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
        Maze m;
        m.renderHeader();
        m.randomMaze();
        m.search();
        m.renderMaze();

        while (!BREAK_RENDER) {
            usleep(5 * 1000);
        }

        BREAK_RENDER = false;
    }
}

int main() {
    responsiveRenderLoop();
}

