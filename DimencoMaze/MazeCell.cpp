#include "MazeCell.h"

MazeCell::MazeCell() {
	MazeCell(0, 0);
}

MazeCell::MazeCell(int x, int y) {

	posX = x;
	posY = y;

	cellType = CellType::NONE;

	wallDown = true;
	wallLeft = true;

	isDiscovered = false;
}
