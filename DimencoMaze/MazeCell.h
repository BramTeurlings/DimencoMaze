#pragma once
#include "CellType.h"
#ifndef MazeCell_H
#define MazeCell_H

class MazeCell
{
	public:
		MazeCell();
		MazeCell(int x, int y);

		int posX;
		int posY;

		CellType cellType;

		bool wallDown;
		bool wallLeft;

		bool isDiscovered;
};

#endif

