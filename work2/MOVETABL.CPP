// movetabl.cpp - MoveTable class implementation

#include "movetabl.h"	// MoveTable
#include <iostream>
#include <fstream>
#include <iomanip>


MoveTable::MoveTable(Cube& cube, int tableSize, int phase2)
	:TheCube(cube),
	 TableSize(tableSize),
	 Phase2(phase2)
{
	// Allocate table storage
	Table = (int (*)[Cube::Move::NumberOfClockwiseQuarterTurnMoves]) // the cast
	        (new int[TableSize*Cube::Move::NumberOfClockwiseQuarterTurnMoves]); // the allocation
}

MoveTable::~MoveTable()
{
	// Deallocate table storage
	delete [] Table;
}

void MoveTable::Initialize(char* fileName)
{
	std::ifstream infile(fileName, std::ios::in|std::ios::binary);
	if (!infile)	// If the move mapping table file is absent...(如果缺少移动映射表文件...)
	{
		// Generate the table and save it to a file(生成移动映射表并将其保存到文件)
		std::cout << "Generating" << std::endl;
		Generate();
		std::cout << "Saving" << std::endl;
		Save(fileName);
		std::cout << "Done Saving" << std::endl;
	}
	else	// The move mapping table file exists(移动映射表文件存在)
	{
		// Load the existing file(加载现有文件)
		//std::cout << "Loading" << std::endl;
		Load(infile);
	}
}

// Generate the table(产生表格)
void MoveTable::Generate(void)
{
	int ordinal;
	int move, move2;

	// Insure the cubies are in their proper slice(确保方块在适当的切片中)
	TheCube.BackToHome();

	// Initialize each table entry(初始化每个表条目)
	for (ordinal = 0; ordinal < TableSize; ordinal++)
	{
		// Establish the proper cube state for the current ordinal(为当前序数建立适当的立方体状态)
		OrdinalToCubeState(ordinal);

		// Initialize the possible moves for each entry(初始化每个条目的可能动作)
		for (move = Cube::Move::R; move <= Cube::Move::B; move++)
		{
			// Apply this move(应用此动作)

			// Phase 1 is the group spanned by <U,D,R,L,F,B>(阶段1是被<U，D，R，L，F，B>涵盖的组)
			// Phase 2 is the group spanned by <U,D,R2,L2,F2,B2>(阶段2是被<U，D，R2，L2，F2，B2>涵盖的组)
			move2 = move;
			if (Phase2 && move != Cube::Move::U && move != Cube::Move::D)
				move2 = Cube::QuarterTurnToHalfTurnMove(move);

			TheCube.ApplyMove(move2);

			// Compute a new ordinal from the new cube state(根据新的魔方状态计算新的序数)
			Table[ordinal][move] = OrdinalFromCubeState();
			// Unapply this move(取消此动作)
			TheCube.ApplyMove(Cube::InverseOfMove(move2));
		}
	}
}

int* MoveTable::operator[](int index)
{
	return Table[index];
}

void MoveTable::Save(char* fileName)
{
	std::ofstream outfile(fileName, std::ios::out|std::ios::binary);
	int index;
	for (index = 0; index < TableSize; index++)
		outfile.write((char*)&Table[index],
			Cube::Move::NumberOfClockwiseQuarterTurnMoves*sizeof(int));
}

void MoveTable::Load(std::ifstream& infile)
{
	int index;
	for (index = 0; index < TableSize; index++)
		infile.read((char*)&Table[index],
			Cube::Move::NumberOfClockwiseQuarterTurnMoves*sizeof(int));
}

// Output the move table in human readable form
void MoveTable::Dump(void)
{
	int ordinal;
	int move, move2;
	// For each table entry...
	for (ordinal = 0; ordinal < TableSize; ordinal++)
	{
		std::cout << std::setw(8) << ordinal << ": ";
		// For each possible move...
		for (move = Cube::Move::R; move <= Cube::Move::B; move++)
		{
			move2 = move;
			if (Phase2)
			{
				if (move != Cube::Move::U && move != Cube::Move::D)
					move2 = Cube::QuarterTurnToHalfTurnMove(move);
			}
			std::cout << Cube::NameOfMove(move2) << ":"
			     << std::setw(8) << Table[ordinal][move] << " ";
		}
		std::cout << std::endl;
	}
}