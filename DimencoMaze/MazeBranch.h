#pragma once
#include "MazeCell.h"
#include <vector>
#ifndef MazeBranch_H
#define MazeBranch_H

class MazeBranch
{
	public:
		MazeBranch();
		MazeBranch(MazeCell masterCell);
		MazeBranch(MazeCell masterCell, std::vector<MazeCell*> neighborCells);

		MazeCell masterCell;
		std::vector<MazeCell*> neighborCells;
};

#endif

