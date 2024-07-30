// solver.h - Cube Solver class definition
//
// This class performs a two phase IDA* search for a solution
// to the scrambled cube.
//此类执行两阶段的IDA* 搜索以寻找混乱的立方体的解决方案。

// Phase 1 searches the group spanned by <U,D,R,L,F,B> until
// a configuration is discovered where the three coordinates
// of twist, flip, and choice are "correct" with respect to a
// solved cube.  This means that no edge cubie is twisted, no
// corner cubie is flipped, and the four middle slice edge
// cubies are in the middle slice (but not necessarily in their
// correct permutation within that slice).  At this point, we
// have found a member of an element of the phase two group.
//
// Phase 2 uses the resulting phase 1 configuration as the
// starting point for a search of the group spanned by
// <U,D,R2,L2,F2,B2>, the goal being to reach <I>, the identity
// (i.e. the solved configuration).  Note that this group
// preserves the three coordinates of the phase 1 search since
// in phase 2 it is impossible to alter the twist, flip, or
// choice aspects of the cube.  The U, D moves do not alter corner
// or edge parity and do not affect the choice of the middle slice
// edge cubies.  The same is true for the R2, L2, F2, B2 moves.
// This can be verified by considering the effect these moves
// have on cube parity (see cube.cpp for details on the parity
// frame of reference used).
//
// In search parlance, the pruning tables (a.k.a. "pattern
// databases) constitute an "admissible" heuristic.  This
// means that they always underestimate the distance (i.e.
// number of moves required) to reach the goal state.  It
// can be proven that any search, such as IDA*, that examines
// nodes at progressively increasing cost values and employs
// an admissible heuristic is "optimal".  This means that the
// first solution found by the search is guaranteed to be of
// minimal distance required to reach the target, or goal,
// state.
// 
// Since the search is split into two sequential IDA* search
// phases, the optimality condition above does not always
// hold.  However, since we allow the phase 1 search to
// iteratively deepen, if let run long enough, it will
// eventually deepen to the point where it is capable of
// finding a complete solution.  At this point, we know we
// have an optimal solution as we have degenerated to a
// a single IDA* search of the cube space, but this takes
// a very long time to occur.  The main strength of the
// two phase search is that it finds a near optimal solution
// very quickly and outputs successively better solutions
// until it eventually finds one that is optimal.  In most
// cases though, the search is terminated early (due to lack
// of patience) once an "adequate" solution is found.
//
// For more information concerning IDA* and admissibility,
// see the paper "Depth-First Iterative-Deepening: An Optimal
// Admissable Tree Search" by Richard E. Korf.  This paper
// appears in volume 25 of "Artificial Intelligence" 1985,
// pp. 97-109.  Also, there are many texts on AI (Artificial
// Intelligence) or books on search techniques that cover
// these topics in depth.
//
#ifndef	_solver_h
#define	_solver_h

#include "kocicube.h"	// KociembaCube
#include "kocimovt.h"	// Kociemba's move mapping tables
#include "pruningt.h"	// PruningTable

class Solver
{
public:
	Solver(void);
	~Solver();

	// Initializes both the move mapping and pruning tables required
	//   by the search(初始化搜索所需的移动映射和修剪表)
	void InitializeTables(void);

	// Perform the two phase search(执行两阶段搜索)
	int Solve(KociembaCube& scrambledCube, int& solutionLength, char* solution);
	// Solver return codes(求解器返回码)
	enum {
		NOT_FOUND,	// A solution was not found(找不到解决方案)
		FOUND,		// A solution was found(找到一个解决方案)
		OPTIMUM_FOUND,	// An optimal solution was found(找到了最佳解决方案)
		ABORT };	// The search was aborted(搜索被中止)
				//   （即第2阶段未产生改进的解决方案）

	// Output the solution(输出解决方案)
	//   Note: you may not need to call this as the best
	//   solution, found so far, is output during the search
	//   (注意：您可能不需要将其称为迄今为止找到的最佳解决方案，它是在搜索过程中输出的)
	int PrintSolution(char* solution);

private:

	enum { Huge = 10000 };	// An absurdly large number(非常大的数)

	// Initiatates the second phase of the search(启动第二阶段搜索)
	int Solve2(KociembaCube& cube, int& solutionLength, char* solution);

	// Phase 1 & 2 recursive IDA* search routines(第一和第二阶段递归IDA *搜索例程)
	int Search1(int twist, int flip, int choice, int depth, int& solutionLength, char* solution);
	int Search2(
		int cornerPermutation,
		int nonMiddleSliceEdgePermutation,
		int middleSliceEdgePermutation,
		int depth,
		int& solutionLength,
		char* solution);

	// Phase 1 & 2 cost heuristics(第一阶段和第二阶段的成本启发法)
	int Phase1Cost(int twist, int flip, int choice);
	int Phase2Cost(
		int cornerPermutation,
		int nonMiddleSliceEdgePermutation,
		int middleSliceEdgePermutation);

	// Predicate to determine if a move is redundant (leads to
	//   (a node that is explored elsewhere) and should therefore
	//   be disallowed.
	// 判断移动是否多余（导致移动到其他位置的节点），因此不应使用。
	inline int Disallowed(int move, int* solutionMoves, int depth);

	// Translates moves from a (face, power) representation to a
	//   single move string representation (e.g. R,3 becomes R').
	//   Also if the move was applied during phase 2 and is either
	//   R,L,F, or B, then a power of 2 is assumed.  This is done
	//   since the phase 2 move mapping tables are in terms of half
	//   turn moves for R,L,F, and B and the power used is 1, not 2.
	//   In this way, we do not have to burden the phase 2 search with
	//   determining the correct power for display purposes only.
	//   I hope that's clear.
	//
	int TranslateMove(int move, int power, int phase2);
	

	// Search variables for the two phase IDA* search(两阶段IDA *搜索的搜索变量)
	int nodes1, nodes2;				// Number of nodes expanded(扩展的节点数)
	int threshold1, threshold2;                     // Current heuristic threshold (cutoff)[当前启发式阈值（截止）]
	int newThreshold1, newThreshold2;		// New threshold as determined by current search pass(根据当前搜索次数确定的新阈值)

	int solutionMoves1[32], solutionMoves2[32];	// List of applied moves(应用的动作列表)
	int solutionPowers1[32], solutionPowers2[32];	// List of powers associated with each move(与每个动作相关的权力列表)
	int solutionLength1, solutionLength2;		// Length of each solution(每个解决方案的长度)
	int minSolutionLength;				// Minimum solution length found so far(到目前为止找到的最小解决方案长度)

	// A cube used for two purposes:
	//   1- Initially used by the solver for initializing the move mapping tables
	//   2- Contains a copy of the scrambled cube that is used at the phase 1/phase 2
	///     transition to compute the initial phase 2 coordinates.
	/// 用于两个目的的魔方：
	/// 1-最初由求解器用于初始化移动映射表
	/// 2-包含打乱的魔方的副本，该副本在阶段1 /阶段2转换中用于计算初始阶段2坐标。
	KociembaCube cube;

	// Phase 1 move mapping tables
	TwistMoveTable twistMoveTable;
	FlipMoveTable flipMoveTable;
	ChoiceMoveTable choiceMoveTable;
	// Phase 2 move mapping tables
	CornerPermutationMoveTable cornerPermutationMoveTable;
	NonMiddleSliceEdgePermutationMoveTable nonMiddleSliceEdgePermutationMoveTable;
	MiddleSliceEdgePermutationMoveTable middleSliceEdgePermutationMoveTable;
	
	// Phase 1 pruning tables
	PruningTable TwistAndFlipPruningTable;
	PruningTable TwistAndChoicePruningTable;
	PruningTable FlipAndChoicePruningTable;
	// Phase 2 pruning tables
	PruningTable CornerAndSlicePruningTable;
	PruningTable EdgeAndSlicePruningTable;
};

#endif	// _solver_h
