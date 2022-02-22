#include "MazeBranch.h"
#include "MazeCell.h"
#include <list>
#include <vector>


MazeBranch::MazeBranch() {

}

MazeBranch::MazeBranch(MazeCell masterCell) {
	this->masterCell = masterCell;
}

MazeBranch::MazeBranch(MazeCell masterCell, std::vector<MazeCell*> neighborCells) {
	this->masterCell = masterCell;
	this->neighborCells = neighborCells;
}
