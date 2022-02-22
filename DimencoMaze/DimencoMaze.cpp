// DimencoMaze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "MazeCell.h"
#include "MazeBranch.h"

using namespace std;

int mazeWidth = 1;
int mazeHeight = 1;

std::vector<MazeCell*> mazeCells;

//Ensure the mazeCells array is filled when calling this method.
void placeEntranceAndExit() {
    if (mazeCells.size() > 1) {
        int randomEntrance = rand() % mazeCells.size();
        mazeCells[randomEntrance] -> cellType = CellType::ENTRANCE;

        int randomExit = rand() % mazeCells.size();
        while (randomEntrance == randomExit) {
            randomExit = rand() % mazeCells.size();
        }
        mazeCells[randomExit] -> cellType = CellType::EXIT;
    }
}

int translateXYToMazeIndex(int x, int y) {
    return x + (y * mazeWidth);
}

void showGeneratedMaze(std::vector<MazeCell*> maze) {
    //Start by drawing the roof.
    std::string roof = "";
    for (int i = 0; i < mazeWidth; i++) {
        roof += " _";
    }

    std::cout << roof + "\n";

    //Draw all the left walls and floors, don't forget to append a right wall at the end!
    int newLineCount = 0;
    const string UNDERLINE = "\x1B[4m";
    const string RESET = "\x1B[0m";

    while (newLineCount < mazeHeight) {
        std::string currentLine = "";
        for (int i = 0 + (mazeWidth * newLineCount); i < mazeWidth + (newLineCount * mazeWidth); i++) {
            if (maze[i] -> wallLeft) {
                currentLine += "|";
            }
            else {
                currentLine += " ";
            }

            //We're saving on resources by using one cell to draw only a left and bottom wall. 
            //This means we do need to check if an exit or entrance sits on top of a bottom wall. If so, underline it.
            if (maze[i]->cellType == CellType::ENTRANCE) {
                
                if (maze[i]->wallDown) {
                    currentLine += UNDERLINE + "B" + RESET;
                }
                else {
                    currentLine += "B";
                }
            }
            else if (maze[i]->cellType == CellType::EXIT) {
                if (maze[i]->wallDown) {
                    currentLine += UNDERLINE + "E" + RESET;
                }
                else {
                    currentLine += "E";
                }
            }
            else if (maze[i]->wallDown) {
                currentLine += "_";
            }
            else {
                currentLine += " ";
            }
        }
        std::cout << currentLine + "|\n";
        newLineCount++;
    }

}

//We only need to check one cell against master at a time so the for loop isn't necessary here but nice in case we ever want to expand the code.
void updateMazeWallsByFoundNeighbors(MazeBranch masterBranch) {
    for (size_t neighborIndex = 0; neighborIndex < masterBranch.neighborCells.size(); neighborIndex++) {
        if (masterBranch.neighborCells[neighborIndex] -> posX > masterBranch.masterCell.posX) {
            //Found index is on the right of the master.
            mazeCells[translateXYToMazeIndex(masterBranch.masterCell.posX + 1, masterBranch.masterCell.posY)] -> wallLeft = false;
        }
        else if (masterBranch.neighborCells[neighborIndex] -> posX < masterBranch.masterCell.posX) {
            //Found index is on the left of the master.
            mazeCells[translateXYToMazeIndex(masterBranch.masterCell.posX, masterBranch.masterCell.posY)] -> wallLeft = false;
        }
        else if (masterBranch.neighborCells[neighborIndex] -> posY > masterBranch.masterCell.posY) {
            //Found index is below the master.
            mazeCells[translateXYToMazeIndex(masterBranch.masterCell.posX, masterBranch.masterCell.posY)] -> wallDown = false;
        }
        else if (masterBranch.neighborCells[neighborIndex] -> posY < masterBranch.masterCell.posY) {
            //Found index is above the master.
            mazeCells[translateXYToMazeIndex(masterBranch.masterCell.posX, masterBranch.masterCell.posY - 1)] -> wallDown = false;
        }
    }

}

//Return a list of cell objects.
std::vector<MazeCell*> getUndiscoveredNeighborsForPosition(int x, int y) {
    std::vector<MazeCell*> neighborCells;
    int xArray[2]{ x - 1, x + 1 };
    int yArray[2]{ y - 1, y + 1 };

    //I hardcoded the array's size to 2 here since arrays somehow lose track of their own size and cause errors? I suppose that's to do with the pointer decaying or pointing to a wrong list.
    //I am also casting mazeCells.size() to type "ptrdiff_t" because it was causing signed/unsigned mismatch warnings. This has further impact on performance as it makes an additional temporary variable in memory.
    //This loop checks the four adjacent spots next to the master
    for (int xIndex = 0; xIndex < 2; xIndex++) {
        //Todo: I feel like I need to catch an indexOutOfRangeException here but C++ doesn't let me? Is it not needed? -> I made sure it can't test an out of range index inside the if statement though it does make operation slower.
        ptrdiff_t listIndex = translateXYToMazeIndex(xArray[xIndex], y);
        if (xArray[xIndex] > -1 && xArray[xIndex] < mazeWidth && listIndex >= 0 && listIndex < static_cast<ptrdiff_t>(mazeCells.size()) && !mazeCells[translateXYToMazeIndex(xArray[xIndex], y)] -> isDiscovered) {
            //DON'T Set neighbor to discovered in maze list.
            //Add neighbor to list.
            neighborCells.emplace_back(new MazeCell(xArray[xIndex], y));
        }
    }

    for (int yIndex = 0; yIndex < 2; yIndex++) {
        //Todo: I feel like I need to catch an indexOutOfRangeException here but C++ doens't let me? Is it not needed? -> I made sure it can't test an out of range index inside the if statement though it does make operation slower.
        ptrdiff_t listIndex = translateXYToMazeIndex(x, yArray[yIndex]);
        if (listIndex >= 0 && listIndex < static_cast<ptrdiff_t>(mazeCells.size()) && !mazeCells[listIndex] -> isDiscovered) {
            //DON'T Set neighbor to discovered in maze list.
            //Add neighbor to list.
            neighborCells.emplace_back(new MazeCell(x, yArray[yIndex]));
        }
    }

    return neighborCells;
}

//Read first cell from private list of available cells and check neighbors.
bool calculateNextCell(MazeBranch subject) {
    //Set master to discovered in maze list.
    mazeCells[translateXYToMazeIndex(subject.masterCell.posX, subject.masterCell.posY)] -> isDiscovered = true;

    //Check for undiscovered neighbors and set them to branch.
    subject.neighborCells = getUndiscoveredNeighborsForPosition(subject.masterCell.posX, subject.masterCell.posY);

    //Turn all neighbor cells into branches and insert them into branch list in a random order. -> Not sure if GC is ok with me declaring this here and not globally, then reusing it... If there even is GC.
    std::vector<MazeBranch> tempBranches;

    for (size_t neighborIndex = 0; neighborIndex < subject.neighborCells.size(); neighborIndex++) {
        tempBranches.emplace_back(MazeBranch(*subject.neighborCells[neighborIndex]));
    }

    //Now "randomly™" insert the branches into active branch list.
    while (!tempBranches.empty()) {
        int chosenIndex = (rand() % tempBranches.size());

        //Check if neighbors are already discovered. This must happen again due to the recursive nature of the algorythm.
        if (!mazeCells[translateXYToMazeIndex(tempBranches[chosenIndex].masterCell.posX, tempBranches[chosenIndex].masterCell.posY)] -> isDiscovered) {
            std::vector<MazeCell*> tempNeighbor { &tempBranches[chosenIndex].masterCell };
            updateMazeWallsByFoundNeighbors(MazeBranch(subject.masterCell, tempNeighbor));

            //Branch added, recurse.
            calculateNextCell(tempBranches[chosenIndex]);
        }

        //Remove the branch from tempBranches now.
        tempBranches.erase(tempBranches.begin() + chosenIndex);
    }

    return true;
}

//Get a user configurable width and height from the console.
void startMazeGeneration(int width, int height) {

    //Simple nested for loop to generate a list of cells that matches the given width and height.
    for (int heightIndex = 0; heightIndex < height; heightIndex++) {
        for (int widthIndex = 0; widthIndex < width; widthIndex++) {
            mazeCells.emplace_back(new MazeCell(widthIndex, heightIndex));
        }
    }

    //Maze list is made, start iterating over list and generating a maze.
    //Manually execute the first cell check to get the recursive algorythm started.
    calculateNextCell(MazeBranch(MazeCell(0, 0)));

    //Place the starting and ending point.
    placeEntranceAndExit();

    //Maze generation is done, display maze in terminal.
    showGeneratedMaze(mazeCells);
}

int main()
{
    std::cout << "Welcome to Bram's maze generator!\nPlease insert the desired WIDTH of the maze:\n";
    std::cin >> mazeWidth;
    std::cout << "Thank you.\nNow please insert the desired HEIGHT of the maze:\n";
    std::cin >> mazeHeight;

    //Check if value is an int and otherwise repeat
    while (!std::cin.good()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nPlease only enter NUMBERS, not characters.\n";
        main();
        return 0;
    }

    //Generate maze here.
    if (mazeWidth > 0 && mazeHeight > 0) {
        std::cout << "Generating maze with provided parameters, please wait...\n";
        startMazeGeneration(mazeWidth, mazeHeight);
    }
    else {
        std::cout << "You must input NUMBERS above 0.\n";
    }

    std::cout << "Maze of size " << mazeWidth << "x" << mazeHeight << " generated!\nWould you like to generate another maze? Y/N\n";
    std::string repeatGen = "";
    std::cin >> repeatGen;

    //I wish C++ had a String.toLower method or something but unfortunately it doesn't. Pretty sure it wouldn't matter for speed in this case so we'll leave it like this.
    if (repeatGen._Equal("y") || repeatGen._Equal("Y")) {
        mazeCells.clear();
        main();
    }
}
