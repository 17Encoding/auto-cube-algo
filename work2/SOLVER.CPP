// solver.cpp - Cube Solver class implementation

#include "solver.h"		// Solver

#include <iostream>
#include <iomanip>

Solver::Solver(void)
	// Phase 1 move mapping tables(第一阶段移动映射表)
	: twistMoveTable(cube),
	  flipMoveTable(cube),
	  choiceMoveTable(cube),
	// Phase 2 move mapping tables(第二阶段移动映射表)
	  cornerPermutationMoveTable(cube),
	  nonMiddleSliceEdgePermutationMoveTable(cube),
	  middleSliceEdgePermutationMoveTable(cube),
	
	// Phase 1 pruning tables(第一阶段修剪表)
	  TwistAndFlipPruningTable(
		twistMoveTable, flipMoveTable,
		cube.Twist(), cube.Flip()),
	  TwistAndChoicePruningTable(
		twistMoveTable, choiceMoveTable,
		cube.Twist(), cube.Choice()),
	  FlipAndChoicePruningTable(
		flipMoveTable, choiceMoveTable,
		cube.Flip(), cube.Choice()),
	// Phase 2 pruning tables(第二阶段修剪表)
	  CornerAndSlicePruningTable(
		cornerPermutationMoveTable, middleSliceEdgePermutationMoveTable,
		cube.CornerPermutation(), cube.MiddleSliceEdgePermutation()),
	  EdgeAndSlicePruningTable(
		nonMiddleSliceEdgePermutationMoveTable, middleSliceEdgePermutationMoveTable,
		cube.NonMiddleSliceEdgePermutation(), cube.MiddleSliceEdgePermutation())
{
	minSolutionLength = Huge;	// Any solution disovered will look better than this one!(发现的任何解决方案都会比这个更好！)
}

Solver::~Solver()
{
}

void Solver::InitializeTables(void)
{
	// Phase 1 move mapping tables(第一阶段移动映射表)

	//std::cout << "Initializing TwistMoveTable" << std::endl;
	twistMoveTable.Initialize((char*)"Twist.mtb");
	//std::cout << "Size = " << twistMoveTable.SizeOf() << std::endl;

	//std::cout << "Initializing FlipMoveTable" << std::endl;
	flipMoveTable.Initialize((char*)"Flip.mtb");
 	//std::cout << "Size = " << flipMoveTable.SizeOf() << std::endl;

	//std::cout << "Initializing ChoiceMoveTable" << std::endl;
	choiceMoveTable.Initialize((char*)"Choice.mtb");
	//std::cout << "Size = " << choiceMoveTable.SizeOf() << std::endl;

	// Phase 2 move mapping tables(第二阶段移动映射表)

	//std::cout << "Initializing CornerPermutationMoveTable" << std::endl;
	cornerPermutationMoveTable.Initialize((char*)"CrnrPerm.mtb");
	//std::cout << "Size = " << cornerPermutationMoveTable.SizeOf() << std::endl;

	//std::cout << "Initializing NonMiddleSliceEdgePermutationMoveTable" << std::endl;
	nonMiddleSliceEdgePermutationMoveTable.Initialize((char*)"EdgePerm.mtb");
	//std::cout << "Size = " << nonMiddleSliceEdgePermutationMoveTable.SizeOf() << std::endl;

	//std::cout << "Initializing MiddleSliceEdgePermutationMoveTable" << std::endl;
	middleSliceEdgePermutationMoveTable.Initialize((char*)"SlicPerm.mtb");
	//std::cout << "Size = " << middleSliceEdgePermutationMoveTable.SizeOf() << std::endl;

	// Phase 1 pruning tables(第一阶段修剪表)
	
	//std::cout << "Initializing TwistAndFlipPruningTable" << std::endl;
	TwistAndFlipPruningTable.Initialize((char*)"TwstFlip.ptb");
	//std::cout << "Size = " << TwistAndFlipPruningTable.SizeOf() << std::endl;
	
	//std::cout << "Initializing TwistAndChoicePruningTable" << std::endl;
	TwistAndChoicePruningTable.Initialize((char*)"TwstChce.ptb");
	//std::cout << "Size = " << TwistAndChoicePruningTable.SizeOf() << std::endl;

	//std::cout << "Initializing FlipAndChoicePruningTable" << std::endl;
	FlipAndChoicePruningTable.Initialize((char*)"FlipChce.ptb");
	//std::cout << "Size = " << FlipAndChoicePruningTable.SizeOf() << std::endl;

	// Phase 2 pruning tables(第二阶段修剪表)

	// Obviously a CornerAndEdgePruningTable doesn't make sense as it's size
	//   would be extremely large (i.e. 8!*8!)

	//std::cout << "Initializing CornerAndSlicePruningTable" << std::endl;
	CornerAndSlicePruningTable.Initialize((char*)"CrnrSlic.ptb");
	//std::cout << "Size = " << CornerAndSlicePruningTable.SizeOf() << std::endl;
	
	//std::cout << "Initializing EdgeAndSlicePruningTable" << std::endl;
	EdgeAndSlicePruningTable.Initialize((char*)"EdgeSlic.ptb");
	//std::cout << "Size = " << EdgeAndSlicePruningTable.SizeOf() << std::endl;
}

int Solver::Solve(KociembaCube& scrambledCube, int& solutionLength, char* solution)
{
	int iteration = 1;
	int result = NOT_FOUND;

	// Make a copy of the scrambled cube for use later on(制作一份scrambledCube的副本，以备后用)
	cube = scrambledCube;

	// Establish initial cost estimate to goal state(建立初始成本估算以达到目标状态)
	threshold1 = Phase1Cost(cube.Twist(), cube.Flip(), cube.Choice());

	nodes1 = 1;		// Count root node here(在这里计算根节点)
	solutionLength1 = 0;

	do
	{
		//std::cout << "threshold(" << iteration
		//	 << ") = " << threshold1 << std::endl;

		newThreshold1 = Huge;	// Any cost will be less than this(任何费用都将少于此)

		// Perform the phase 1 recursive IDA* search(执行阶段1递归IDA *搜索)
		result = Search1(cube.Twist(), cube.Flip(), cube.Choice(), 0, solutionLength, solution);

		// Establish a new threshold for a deeper search(为更深入的搜索建立新的门槛)
		threshold1 = newThreshold1;
		
		// Count interative deepenings(计算迭代加深)
		iteration++;
	} while (result == NOT_FOUND);

	//std::cout << "Phase 1 nodes = " << nodes1 << std::endl;
	return result;
}

int Solver::Search1(int twist, int flip, int choice, int depth, int& solutionLength, char* solution)
{
	int cost, totalCost;
	int move;
	int power;
	int twist2, flip2, choice2;
	int result;

	// Compute cost estimate to phase 1 goal state(将费用估算值计算为第一阶段的目标状态)
	cost = Phase1Cost(twist, flip, choice);	// h

	if (cost == 0)	// Phase 1 solution found...(寻找阶段1解决方案...)
	{
		solutionLength1 = depth;	// Save phase 1 solution length(保存第一阶段的解决方案长度)

		// We need an appropriately initialized cube in order
		//   to begin phase 2.  First, create a new cube that
		//   is a copy of the initial scrambled cube.  Then we
		//   apply the phase 1 move sequence to that cube.  The
		//   phase 2 search can then determine the initial
		//   phase 2 coordinates (corner, edge, and slice
		//   permutation) from this cube.
		//
		//   Note: No attempt is made to merge moves of the same
		//   face adjacent to the phase 1 & phase 2 boundary since
		//   the shorter sequence will quickly be found.
		
		KociembaCube phase2Cube = cube;
		for (int i = 0; i < solutionLength1; i++)
		{
			for (power = 0; power < solutionPowers1[i]; power++)
				phase2Cube.ApplyMove(solutionMoves1[i]);
		}
		// Invoke Phase 2(调用阶段2)
		result = Solve2(phase2Cube, solutionLength, solution);
		if (result != NOT_FOUND)
			return result;
	}

	// See if node should be expanded(查看是否应扩展节点)
	totalCost = depth + cost;	// g + h

	if (totalCost <= threshold1)	// Expand node(扩展节点)
	{
		// If this happens, we should have found the
		//   optimal solution at this point, so we
		//   can exit indicating such.  Note: the first
		//   complete solution found in phase1 is optimal
		//   due to it being an addmissible IDA* search.
		if (depth >= minSolutionLength-1)
			return OPTIMUM_FOUND;
		
		for (move = Cube::Move::R; move <= Cube::Move::B; move++)
		{
			if (Disallowed(move, solutionMoves1, depth)) continue;
				
			twist2  = twist;
			flip2   = flip;
			choice2 = choice;

			solutionMoves1[depth] = move;
			for (power = 1; power < 4; power++)
			{
				solutionPowers1[depth] = power;
				twist2  = twistMoveTable[twist2][move];
				flip2   = flipMoveTable[flip2][move];
				choice2 = choiceMoveTable[choice2][move];
				nodes1++;
				// Apply the move(套用动作)
				if (result = Search1(twist2, flip2, choice2, depth+1, solutionLength, solution))
						return result;
			}
		}
	}
	else	// Maintain minimum cost exceeding threshold(维持最低成本超过阈值)
	{
		if (totalCost < newThreshold1)
			newThreshold1 = totalCost;
	}
	return NOT_FOUND;
}

int Solver::Solve2(KociembaCube& cube, int& solutionLength, char* solution)
{
	int iteration = 1;
	int result = NOT_FOUND;

	// Establish initial cost estimate to goal state(建立初始成本估算以达到目标状态)
	threshold2 = Phase2Cost(
		cube.CornerPermutation(),
		cube.NonMiddleSliceEdgePermutation(),
		cube.MiddleSliceEdgePermutation());

	nodes2 = 1;		// Count root node here(在这里计算根节点)
	solutionLength2 = 0;

	do
	{
		//std::cout << "threshold2(" << iteration
		//	<< ") = " << threshold2 << std::endl;

		newThreshold2 = Huge;	// Any cost will be less than this(任何费用都将少于此)

		// Perform the phase 2 recursive IDA* search(执行阶段2递归IDA *搜索)
		result = Search2(
			cube.CornerPermutation(),
			cube.NonMiddleSliceEdgePermutation(),
			cube.MiddleSliceEdgePermutation(), 0, solutionLength, solution);

		// Establish a new threshold for a deeper search(为更深入的搜索建立新的门槛)
		threshold2 = newThreshold2;
		
		// Count interative deepenings(计算迭代加深)
		iteration++;
	} while (result == NOT_FOUND);

//	cout << "Phase 2 nodes = " << nodes2 << endl;
	return result;
}

int Solver::Search2(
	int cornerPermutation,
	int nonMiddleSliceEdgePermutation,
	int middleSliceEdgePermutation,
	int depth,
	int& solutionLength,
	char* solution)
{
	int cost, totalCost;
	int move;
	int power, powerLimit;
	int	cornerPermutation2;
	int nonMiddleSliceEdgePermutation2;
	int middleSliceEdgePermutation2;
	int result;

	// Compute cost estimate to goal state(计算成本估算至目标状态)
	cost = Phase2Cost(
		cornerPermutation,
		nonMiddleSliceEdgePermutation,
		middleSliceEdgePermutation);	// h

	if (cost == 0)	// Solution found...(找到解决方案...)
	{
		solutionLength2 = depth;	// Save phase 2 solution length(保存第二阶段的解决方案长度)
		if (solutionLength1 + solutionLength2 < minSolutionLength)
			minSolutionLength = solutionLength1 + solutionLength2;
		solutionLength=PrintSolution(solution);
		return FOUND;
	}

	// See if node should be expanded(查看是否应扩展节点)
	totalCost = depth + cost;	// g + h

	if (totalCost <= threshold2)	// Expand node(扩展节点)
	{
		// No point in continuing to search for solutions of equal or greater
		//   length than the current best solution
		if (solutionLength1 + depth >= minSolutionLength-1) return ABORT;

		for (move = Cube::Move::R; move <= Cube::Move::B; move++)
		{
			if (Disallowed(move, solutionMoves2, depth)) continue;

			cornerPermutation2 = cornerPermutation;
			nonMiddleSliceEdgePermutation2 = nonMiddleSliceEdgePermutation;
			middleSliceEdgePermutation2 = middleSliceEdgePermutation;

			solutionMoves2[depth] = move;
			powerLimit = 4;
			if (move != Cube::Move::U && move != Cube::Move::D) powerLimit=2;

			for (power = 1; power < powerLimit; power++)
			{
				cornerPermutation2 =
					cornerPermutationMoveTable[cornerPermutation2][move];
				nonMiddleSliceEdgePermutation2 =
					nonMiddleSliceEdgePermutationMoveTable[nonMiddleSliceEdgePermutation2][move];
				middleSliceEdgePermutation2 =
					middleSliceEdgePermutationMoveTable[middleSliceEdgePermutation2][move];

				solutionPowers2[depth] = power;

				nodes2++;
				// Apply the move
				if (result = Search2(
						cornerPermutation2,
						nonMiddleSliceEdgePermutation2,
						middleSliceEdgePermutation2, depth+1, solutionLength, solution))
						return result;
			}
		}
	}
	else	// Maintain minimum cost exceeding threshold
	{
		if (totalCost < newThreshold2)
			newThreshold2 = totalCost;
	}
	return NOT_FOUND;
}

int Solver::Phase1Cost(int twist, int flip, int choice)
{
	// Combining admissible heuristics by taking their maximum
	//   produces an improved admissible heuristic.(通过采用最大允许值来组合可允许的启发式算法会产生改进的可允许的启发式算法)
	int cost = TwistAndFlipPruningTable.GetValue(twist*flipMoveTable.SizeOf()+flip);
	int cost2 = TwistAndChoicePruningTable.GetValue(twist*choiceMoveTable.SizeOf()+choice);
	if (cost2 > cost) cost = cost2;
	cost2 = FlipAndChoicePruningTable.GetValue(flip*choiceMoveTable.SizeOf()+choice);
	if (cost2 > cost) cost = cost2;
	return cost;
}

int Solver::Phase2Cost(
		int cornerPermutation,
		int nonMiddleSliceEdgePermutation,
		int middleSliceEdgePermutation)
{
	// Combining admissible heuristics by taking their maximum
	//   produces an improved admissible heuristic.
	int cost = CornerAndSlicePruningTable.GetValue(
		cornerPermutation*middleSliceEdgePermutationMoveTable.SizeOf()+middleSliceEdgePermutation);
	int cost2 = EdgeAndSlicePruningTable.GetValue(
		nonMiddleSliceEdgePermutation*middleSliceEdgePermutationMoveTable.SizeOf()+middleSliceEdgePermutation);
	if (cost2 > cost) cost = cost2;
	return cost;
}

int Solver::Disallowed(int move, int* solutionMoves, int depth)
{
	if (depth > 0)
	{
		// Disallow successive moves of a single face (RR2 is same as R')
		if (solutionMoves[depth-1] == move)
			return 1;

		//   Disallow a move of an opposite face if the current face
		//     moved is B,L, or D. (BF, LR, DU are same as FB,RL,UD)
		if ((move == Cube::Move::F) && solutionMoves[depth-1] == Cube::Move::B)
			return 1;
		if ((move == Cube::Move::R) && solutionMoves[depth-1] == Cube::Move::L)
			return 1;
		if ((move == Cube::Move::U) && solutionMoves[depth-1] == Cube::Move::D)
			return 1;

		// Disallow 3 or more consecutive moves of opposite faces
		//   (UDU is same as DU2 and U2D)
		if ((depth > 1) && solutionMoves[depth-2] == move &&
			solutionMoves[depth-1] == Cube::OpposingFace(move))
			return 1;
	}
	return 0;	// This move is allowed
}

int Solver::PrintSolution(char* solution)
{
	int len=0;
	for (int i = 0; i < solutionLength1; i++)
		solution[len++] = Cube::NameOfMove(TranslateMove(solutionMoves1[i], solutionPowers1[i], 0));
	//std::cout << ". ";
	for (int i = 0; i < solutionLength2; i++)
		solution[len++] = Cube::NameOfMove(TranslateMove(solutionMoves2[i], solutionPowers2[i], 1));
	//std::cout << "(" << solutionLength1 + solutionLength2 << ")" << std::endl;
	return solutionLength1 + solutionLength2;
}

int Solver::TranslateMove(int move, int power, int phase2)
{
	int translatedMove = move;

	if (phase2 && move != Cube::Move::U && move != Cube::Move::D)
		power = 2;
		
	if (power == 2)
		translatedMove = Cube::QuarterTurnToHalfTurnMove(move);
	else if (power == 3)
		translatedMove = Cube::InverseOfMove(move);
	else ;

	return translatedMove;
}
