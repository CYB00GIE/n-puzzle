#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>
#include <limits.h>
#include <algorithm>

enum Step {
    start,
    up,
    down,
    left,
    right
};

class Puzzle {
private:
    std::vector<std::vector<int>> puzzle;
    int side;
    int pos0;
    int manDist;

    void manualInput() {
        if (!puzzle.empty()) {
            throw "Puzzle already initialized";
        }

        int n, value;

        std::cout << "Enter number of blocks in the puzzle: ";
        std::cin >> n;

        std::cout << "Enter position of 0: ";
        std::cin >> pos0;

        if (pos0 < 0 || pos0 > n) {
            pos0 = n;
        }

        side = sqrt(n + 1);

        std::cout << "Enter " << n << " values for the puzzle:" << std::endl;

        for (int i = 0; i < side; i++) {
            std::vector<int> p;
            puzzle.push_back(p);
        }
        
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                std::cin >> value;
                puzzle[i].push_back(value);
            }
        }
    }

public:

    Puzzle() {
        manualInput();
        manDist = -1;
        manhattan();
    }

    Puzzle(std::vector<std::vector<int>> board, int otherSide, int otherPos0) {
        side = otherSide;
        pos0 = otherPos0;

        for (int i = 0; i < side; i++) {
            std::vector<int> p;
            puzzle.push_back(p);
        }

        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                puzzle[i].push_back(board[i][j]);
            }
        }

        manDist = -1;

        manhattan();
    }

    int manhattan() {
        if (manDist == -1) {
            manDist = 0;
            int temp;

            int pos0i = pos0 / side;
            int pos0j = pos0 % side;

            for (int i = 0; i < side; i++) {
                for (int j = 0; j < side; j++) {
                    if (puzzle[i][j] == 0) {
                        manDist += abs(pos0i - i);
                        manDist += abs(pos0j - j);
                    }
                    else if (puzzle[i][j] <= pos0) {
                        manDist += abs((puzzle[i][j] - 1) / side - i);
                        manDist += abs((puzzle[i][j] - 1) % side - j);
                    }
                    else if (puzzle[i][j] > pos0) {
                        manDist += abs(puzzle[i][j] / side - i);
                        manDist += abs(puzzle[i][j] % side - j);
                    }
                }
            }
        }

        return manDist;
    }

    bool isGoal() {
        return manDist == 0;
    }

    static bool compare(std::pair<Puzzle, Step>& el1, std::pair<Puzzle, Step>& el2) {
        return el1.first.manDist < el2.first.manDist;
    }

    std::vector<std::pair<Puzzle, Step>> neighbours() {
        std::vector<std::pair<Puzzle, Step>> neighbours;
        std::pair<Puzzle, Step> neighbour;

        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                if (puzzle[i][j] == 0) {
                    if (j > 0) {
                        std::swap (puzzle[i][j], puzzle[i][j - 1]);
                        neighbour.first = Puzzle(puzzle, side, pos0);
                        neighbour.second = right;
                        neighbours.push_back(neighbour);
                        std::swap (puzzle[i][j], puzzle[i][j - 1]);
                    }
                    
                    if (j < side - 1) {
                        std::swap (puzzle[i][j], puzzle[i][j + 1]);
                        neighbour.first = Puzzle(puzzle, side, pos0);
                        neighbour.second = left;
                        neighbours.push_back(neighbour);
                        std::swap (puzzle[i][j], puzzle[i][j + 1]);
                    }

                    if (i > 0) {
                        std::swap (puzzle[i][j], puzzle[i - 1][j]);
                        neighbour.first = Puzzle(puzzle, side, pos0);
                        neighbour.second = down;
                        neighbours.push_back(neighbour);
                        std::swap (puzzle[i][j], puzzle[i - 1][j]);
                    }

                    if (i < side - 1) {
                        std::swap (puzzle[i][j], puzzle[i + 1][j]);
                        neighbour.first = Puzzle(puzzle, side, pos0);
                        neighbour.second = up;
                        neighbours.push_back(neighbour);
                        std::swap (puzzle[i][j], puzzle[i + 1][j]);
                    }

                }
            }
        }

        std::sort(neighbours.begin(), neighbours.end(), compare);

        return neighbours;
    }

    void print() {
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                std::cout << puzzle[i][j] << ' ';
            }
            std::cout << std::endl;
        }
    }

    bool equals(Puzzle other) {
        int maxSide = std::max(side, other.side);

        for (int i = 0; i < maxSide; i++) {
            for (int j = 0; j < maxSide; j++) {
                if (puzzle[i][j] != other.puzzle[i][j]) 
                    return false;
            }
        }

        return true;
    }
};

bool isUnique(Puzzle& p, std::vector<std::pair<Puzzle, Step>>& vec) {
    for (std::pair<Puzzle, Step> v : vec) {
        if (p.equals(v.first)) {
            return false;
        }
    }

    return true;
}

int aStar(std::vector<std::pair<Puzzle, Step>>& path, int g, int limit) {
    Puzzle lastNode = path.back().first;

    if (lastNode.isGoal()) {
        return 0;
    }

    int f = g + lastNode.manhattan();

    if (f > limit) {
        return f;
    }

    int min = INT_MAX;

    std::vector<std::pair<Puzzle, Step>> next = lastNode.neighbours();

    int t;

    for (std::pair<Puzzle, Step> n : next) {
        if (isUnique(n.first, path)) {
            path.push_back(n);

            t = aStar(path, g + 1, limit);

            if (t == 0) {
                return t;
            }
            
            if (t < min) {
                min = t;
            }
            path.pop_back();
        }
    }

    return min;
}

std::pair<
        std::vector<
                    std::pair<Puzzle, Step>
                    >, 
        int> idaStar(Puzzle root) {

    std::vector<std::pair<Puzzle, Step>> path;

    std::pair<Puzzle, Step> first(root, start);

    path.push_back(first);

    if (root.isGoal()) {
        return std::pair<std::vector<std::pair<Puzzle, Step>>, int> (path, 0);
    }

    int limit = root.manhattan();

    while (!path.back().first.isGoal()) {
       int limit = aStar(path, 0, limit);
    }

    return std::pair<std::vector<std::pair<Puzzle, Step>>, int> (path, path.size() - 1);
} 


int main() {
    Puzzle p;
    
    std::pair<std::vector<std::pair<Puzzle, Step>>, int> result = idaStar(p);

    std::cout << result.second << std::endl;
    for (std::pair<Puzzle, Step> p : result.first) {
        switch (p.second) {
            case 0: break;
            case 1: std::cout << "up" << std::endl; break;
            case 2: std::cout << "down" << std::endl; break;
            case 3: std::cout << "left" << std::endl; break;
            case 4: std::cout << "right" << std::endl; break;
        }
        std::cout << std::endl;
    }
}
