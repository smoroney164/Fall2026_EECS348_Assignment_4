/**
 * Name: Sydney Moroney
 * EECS 348 Assignment 4
 * This program will take in unsolved Sudoku puzzles and prints
 * all possible solutions to each puzzle
 * Source: written entirely by Claude AI
 * Comments: All comments authored by me
 * Inputs: Files containing unsolved Sudoku puzzles
 * Outputs: Prints all possible solutions to Sudoku puzzles
 * Creation Date: 10/14/2026z
 **/

#include <array> // Allows program to utilize arrays for the Sudoku grid
#include <cctype> // Allows program to check for white space (spaces and newlines)
#include <fstream> //Allows program to get file-stream input for the puzzle files
#include <iostream> //Allows program to output to the console
#include <string> //Allows program to use strings for filenames
#include <vector> //Allows program to use vector data type

class Puzzle { //This class is responsible for storing the Sudoku board and checking validity of original puzzloe
public: //The follow code block is able to be used outside of the Puzzle class
    static constexpr int SIZE = 9; //Defines that a Sudoku board has 9 rows and 9 columns
    static constexpr int ALL_DIGITS = (1 << SIZE) - 1; //Creates 9 integers to represent the digits 1 through 9

    //Creates the board and empty records of which digits are used
    Puzzle() : board_{}, rowUsed_{}, columnUsed_{}, blockUsed_{} {}

    //Will read exactly 81 cells. A cell can only be filled with digits 1-9 and underscores. Spaces and newlines are allowed for formatting
    bool load(const std::string& filename, std::string& error) {
        std::ifstream input(filename); //System attempts to open filename
        if (!input) { //If failed to open the file...
            error = "Could not open the puzzle file."; //Prints error message
            return false; //Stops trying to load this file
        }

        //Resets board and which digits have been used before loading a new puzzle
        board_ = {}; //Clears board
        rowUsed_ = {}; //Clears digits used in each row 
        columnUsed_ = {}; //Clears digits used in each column
        blockUsed_ = {}; //Clears digits used in each 3x3 bloack

        int cellCount = 0; //Tracks how many cells have been read
        char character; //Holds the character in the current cell being read
        while (input.get(character)) { //Goes through each character, even past 81 cells
            // Allow formatting whitespace but reject all other unexpected data.
            if (std::isspace(static_cast<unsigned char>(character))) { //If character is white space
                continue; //Skip the whitespace and move onto the next character
            }
            if (character != '_' && (character < '1' || character > '9')) { //Checks if character is anything other than 1-9 or underscore
                error = "Invalid character in the puzzle file."; //If so, saves error message
                return false; //Stops trying to load this file
            }
            if (cellCount >= SIZE * SIZE) { //If the puzzle has more than 81 cells...
                error = "Extra puzzle data found after 81 cells."; //saves error message
                return false; //Stops trying to load invalid file
            }

            const int row = cellCount / SIZE; //finds and stores current row 
            const int column = cellCount % SIZE; //finds and stores current column
            if (character != '_') { //checks if the character is not an underscore
                const int value = character - '0'; //turns a character into its digit counterpart
                if (!canPlace(row, column, value)) { //Checks if the placement breaks Sudoku game rules
                    error = "Conflicting numbers in the original puzzle."; //If it does, saves error message 
                    return false; //stops trying to load invalid file
                }
                place(row, column, value); //Adds original number to board and adds it to the lists of used-digits
            }
            ++cellCount; //Moves to the next cell 
        }

        if (input.bad()) { //If the system fails to read the file
            error = "Error while reading the puzzle file."; //Saves error message
            return false; //Stops trying to load the failed file
        }
        if (cellCount != SIZE * SIZE) { //Checks if the size is greater than a 9x9 with 81 cells
            error = "Expected 81 cells, but found " + std::to_string(cellCount) + "."; //Saves error message
            return false; //Stops trying to load invalid file
        }
        return true; //If false was never returned, all input checks passed!
    }

    //Determines which digits are possible candidates for a specific cell
    //9 bitmasks, which represent digits 1-9
    int candidateMask(int row, int column) const { //What numbers are allowed in this cell?
        const int blocked = rowUsed_[row] | columnUsed_[column] | //Figures out which numbers have been used in the cells row and column
                            blockUsed_[blockIndex(row, column)]; //Combines with which numbers have been used in the cells 3x3 block
        return ALL_DIGITS & ~blocked; //Returns digits 1-9 minus all blocked (used) numbers
    }

    //Returns whether or not a specific number can be placed in a specific cell
    bool canPlace(int row, int column, int value) const {
        const int digitBit = 1 << (value - 1); //Represents the number being tested as a singular bit
        return board_[row][column] == 0 && //Returns if the cell is empty and...
               (candidateMask(row, column) & digitBit) != 0; //if the number is allowed in that space
    }

    int get(int row, int column) const { return board_[row][column]; } //Returns number stored in the cell at position [row][column]

    // Updates the board and used digits for each row, column, and block
    void place(int row, int column, int value) { //Places a number to make it used and updates masks
        const int digitBit = 1 << (value - 1); //Represents the number being tested as a singular bit
        board_[row][column] = value; //Updates position on board
        rowUsed_[row] |= digitBit; //Updates digits used in cells row
        columnUsed_[column] |= digitBit; //Updates digits used in cells column
        blockUsed_[blockIndex(row, column)] |= digitBit; //Updates digits used in cells 3x3 block
    }

    // Remove a digit in case of backtracking
    void remove(int row, int column, int value) { //Removes a digit to make it avaliable again and updates masks
        const int digitBit = 1 << (value - 1); //Represents the number being tested as a singular bit
        board_[row][column] = 0; //Updates position on board
        rowUsed_[row] &= ~digitBit;  //Updates digits used in cells row
        columnUsed_[column] &= ~digitBit; //Updates digits used in cells column
        blockUsed_[blockIndex(row, column)] &= ~digitBit; //Updates digits used in cells 3x3 block
    }

    //Prints the organized 9x9 board, with underscores signialing an empty cell
    void print(std::ostream& output = std::cout) const { //Prints puzzle board to the terminal
        for (int row = 0; row < SIZE; ++row) { //goes through all 9 rows
            for (int column = 0; column < SIZE; ++column) { //Goes through all 9 columns
                const int value = board_[row][column]; //Gets number in cell at position [row][column]
                output << (value == 0 ? '_' : static_cast<char>('0' + value)); //Prints an underscore, if not a digit
                if (column != SIZE - 1) output << ' '; //Prints space after cell as long as it is not the last one in column
            }
            output << '\n'; //prints newline
        }
    }

private: //The follow code block's contents are only avaliable to the puzzle class
    //Finds which block the current cell is in
    static int blockIndex(int row, int column) {
        return (row / 3) * 3 + column / 3; //Calculates and returns block number from 0 to 8
    }

    std::array<std::array<int, SIZE>, SIZE> board_; //Stores 9x9 board in a fixed array, since the size does not need to change
    std::array<int, SIZE> rowUsed_; //Stores used digits for each row
    std::array<int, SIZE> columnUsed_; //Stores used digits for each column
    std::array<int, SIZE> blockUsed_; //Stores used digits for each block
}; //END OF PUZZLE CLASS

class Solver { //This class is responsible for searching and printing every solution
public: //States that the following members are accessible outside of solver class
    explicit Solver(const Puzzle& originalPuzzle) //takes the original puzzle
        : puzzle_(originalPuzzle), solutionCount_(0) {} //copies original and starts number of solutions at 0

    // Finds all solutions and returns the number of solutions found
    unsigned long long solveAll() {
        solutionCount_ = 0; //restarts counter of solutions for this run
        search(); //starts recursive search
        return solutionCount_; //returns number of solutions found
    }

private: //the following members are hidden from outside the solver class
    Puzzle puzzle_; //stores a copy of the original puzzle for the solver to modify
    unsigned long long solutionCount_; //keeps track of how many solutions are found

    //finds the empty cell with the least amount of valid numbers
    //if the cell has no choices, returns 0. if finished, returns -1. If cell is selected returns 1.
    int findBestCell(int& bestRow, int& bestColumn, int& bestMask) const {
        int smallestCount = Puzzle::SIZE + 1; //starts with more than the possible amount of numbers in a row
        bestRow = -1; //no best row
        bestColumn = -1; //no best column
        bestMask = 0; //no best candidates

        for (int row = 0; row < Puzzle::SIZE; ++row) { //goes through each row
            for (int column = 0; column < Puzzle::SIZE; ++column) { //goes through each column
                if (puzzle_.get(row, column) != 0) continue; //skips cells that are already assigned a numnber

                const int mask = puzzle_.candidateMask(row, column); //finds possible numbers for current cell
                const int count = countCandidates(mask); //finds how many possible numbers fit into cell

                if (count == 0) return 0; //if there are no possible candidates, this attempt cannot solve the puzzle

                if (count < smallestCount) { //compares current cell to current best cell
                    //if current cell is smaller...
                    smallestCount = count; //assigns new smallest count to count
                    bestRow = row; //assigns new bestRow to current row
                    bestColumn = column; //assigns new bestColumn to current column
                    bestMask = mask; //assigns new bestMask to current mask

                    // No positive candidate count can be smaller than one. If it finds one, it will stop searching
                    if (count == 1) return 1;
                }
            }
        }
        return bestRow == -1 ? -1 : 1; //if board is full, returns -1. Otherwise, returns 1
    }

    //Counts how many digits are represented by the 1-bit masks
    static int countCandidates(int mask) {
        int count = 0; //starts count at zero
        while (mask != 0) { //while mask still contains possible digits to be used
            mask &= mask - 1; //removes a possible digit from mask
            ++count; //adds one to the count
        }
        return count; //returns amount of possible candidates
    }

    //Recursive, backtracking search. Find all solutions by trying each valid number and going back if needed.
    void search() {
        int row, column, candidates; //variables for a certain cell and its possible digits
        const int result = findBestCell(row, column, candidates); //finds next cell or determines that the path does not work

        if (result == 0) return; //if a cell has no valid numbers, it gives up on this pathway
        if (result == -1) { //if every cell has been filled
            ++solutionCount_; //adds to the solution count
            std::cout << "Solution " << solutionCount_ << ":\n"; //prints text with solution number
            puzzle_.print(); //prints the solved board
            std::cout << '\n'; //new line
            return; //end of this solution, starts trying to find new solutions
        }

        
        for (int value = 1; value <= Puzzle::SIZE; ++value) { //tests numbers 1 to 9
            if ((candidates & (1 << (value - 1))) == 0) continue; //does not test numbers that are not valid in cell
            puzzle_.place(row, column, value); //tries placing each number
            search(); //solves remaining cells
            puzzle_.remove(row, column, value); //removes the previously placed number to test others
        }
    }
}; //END OF SOLVER CLASS 

int main(int argc, char* argv[]) { //Starts program, holds command-line input
    std::vector<std::string> files; //stores puzzle filenames
    if (argc > 1) { //if user supplied filenames
        for (int index = 1; index < argc; ++index) files.push_back(argv[index]); //adds each filename given by user
    } else { //if user did not provide a puzzle name
        for (int index = 1; index <= 5; ++index) //looks for 5 defult puzzles
            files.push_back("puzzle" + std::to_string(index) + ".txt"); //builds file name for all 5 puzzles
    }

    for (const std::string& filename : files) { //runs for all puzzle files
        std::cout << "==============================\n"; //formatting
        std::cout << "Puzzle File: " << filename << "\n\n"; //prints puzzle file as header

        Puzzle puzzle; //creates puzzle object for file
        std::string error; //stores error message
        if (!puzzle.load(filename, error)) { //if puzzle fails to load
            std::cout << error << "\n\n"; //prints stored error message
            continue; //skips file and moves onto the next
        }

        std::cout << "Original Puzzle:\n"; //prints heading for unsolved puzzle
        puzzle.print(); //prints original puzzle
        std::cout << "\n"; //new line

        Solver solver(puzzle); //Makes a solver object for current puzzle being solved
        const unsigned long long solutions = solver.solveAll(); //finds all solutions and keeps track of each solution
        if (solutions == 0) std::cout << "No solution found\n"; //prints no solution found if none are found
        else std::cout << "Total solutions found: " << solutions << "\n"; //if there is a solution(s), prints number of solutions
        std::cout << '\n'; //new line
    }
    return 0; //End of Program! TERMINATE!!
}
