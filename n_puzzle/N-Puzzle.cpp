#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>
#include <limits.h>
#include <algorithm>

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

        std::cin >> n;

        std::cin >> pos0;

        if (pos0 < 0 || pos0 > n) {
            pos0 = n;
        }

        side = sqrt(n + 1);

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

    static bool compare(Puzzle& p1, Puzzle& p2) {
        return p1.manDist < p2.manDist;
    }

    std::vector<Puzzle> neighbours() {
        std::vector<Puzzle> neighbours;

        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                if (puzzle[i][j] == 0) {
                    if (j > 0) {
                        std::swap (puzzle[i][j], puzzle[i][j - 1]);
                        neighbours.push_back(Puzzle(puzzle, side, pos0));
                        std::swap (puzzle[i][j], puzzle[i][j - 1]);
                    }
                    
                    if (j < side - 1) {
                        std::swap (puzzle[i][j], puzzle[i][j + 1]);
                        neighbours.push_back(Puzzle(puzzle, side, pos0));
                        std::swap (puzzle[i][j], puzzle[i][j + 1]);
                    }

                    if (i > 0) {
                        std::swap (puzzle[i][j], puzzle[i - 1][j]);
                        neighbours.push_back(Puzzle(puzzle, side, pos0));
                        std::swap (puzzle[i][j], puzzle[i - 1][j]);
                    }

                    if (i < side - 1) {
                        std::swap (puzzle[i][j], puzzle[i + 1][j]);
                        neighbours.push_back(Puzzle(puzzle, side, pos0));
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

    bool equals(Puzzle& other) {
        if (side != other.side) 
            return false;

        int maxSide = std::max(side, other.side);

        if (manDist != other.manDist) {
            return false;
        }

        for (int i = 0; i < maxSide; i++) {
            for (int j = 0; j < maxSide; j++) {
                if (puzzle[i][j] != other.puzzle[i][j]) 
                    return false;
            }
        }

        return true;
    }

    bool isUnique(std::vector<Puzzle>& path, std::vector<Puzzle>& visited) {
        /*for (Puzzle p : path) {
            if (equals(p)) {
                return false;
            }
        }*/ 

        for (Puzzle v : visited) {
            if (equals(v)) {
                return false;
            }
        }


        return true;
    }

    bool isSolvable() {
        int inversions = 0;
        int row0;
        int array1[side * side], array2[side * side];
        for (int i = 0; i < side; i++) {
            for (int j = 0; j < side; j++) {
                array1[i * side + j] = array2[i * side + j] = puzzle[i][j];
                if (puzzle[i][j] == 0) {
                    row0 = i;
                }
            }
        }
        for (int i = 0; i < side * side; i++) {
            if (array1[i] == 0) {
                continue;
            }
            for (int j = i + 1; j < side * side; j++) {
                if (array2[j] == 0) {
                    continue;
                }

                if (i < j && array1[i] > array2[j]) {
                    inversions++;
                }
            }
        }

        if (side % 2 && inversions % 2) {
            return false;
        }
        if (side % 2 == 0 && (inversions + row0) % 2 == 0) {
            std::cout << "Inversions: " << inversions << std::endl;
            std::cout << "0 at row " << row0 << std::endl;
            return false;
        }
        else return true;
    }
};

bool isUnique(Puzzle& p, std::vector<Puzzle>& vec) {
    for (Puzzle v : vec) {
        if (p.equals(v)) {
            return false;
        }
    }

    return true;
}

int aStar(std::vector<Puzzle>& path, int g, int limit) {
    Puzzle lastNode = path.back();
    std::vector<Puzzle> visited;

    if (lastNode.isGoal()) {
        return 0;
    }

    int f = g + lastNode.manhattan();

    if (f > limit) {
        return f;
    }

    int min = INT_MAX;

    std::vector<Puzzle> next = lastNode.neighbours();

    int t;

    for (Puzzle n : next) {
        if (n.isUnique(path, visited)) {
            path.push_back(n);
            visited.push_back(n);

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

std::pair<std::vector<Puzzle>, int> idaStar(Puzzle root) {
    std::cout << "Starting!" << std::endl;
    std::vector<Puzzle> path;

    if (!root.isSolvable()) {
        return std::pair<std::vector<Puzzle>, int> (path, -1); 
    }

    root.print();

    path.push_back(root);

    if (root.isGoal()) {
        return std::pair<std::vector<Puzzle>, int> (path, 0);
    }

    int limit = root.manhattan();

    while (!path.back().isGoal()) {
        std::cout << "Searching with limit " << limit << std::endl;
        limit = aStar(path, 0, limit);
    }

    return std::pair<std::vector<Puzzle>, int> (path, path.size() - 1);
} 


int main() {
    Puzzle p;

    //std::cout << p.isSolvable() << std::endl;
    
    std::pair<std::vector<Puzzle>, int> result = idaStar(p);

    std::cout << result.second << std::endl;
    for (Puzzle p : result.first) {
        p.print();
        std::cout << std::endl;
    }
}