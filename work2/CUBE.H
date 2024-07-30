// cube.h - Rubik's Cube class definition
//
// A cube model containing basic cube definitions and
// cube operations.  Applying a move maintains the
// proper cube permutation and orientation for each
// cubie. This class is designed to be subclassed in
// order to extend it to a specific type of cube
// useful for a particular solution method.
//
/*包含基本魔方状态定义和魔方操作的多维数据集模型。
应用移动可为每个方块保持正确的方块排列和方向。 
此类被设计为父类，以便将其扩展为对特定解决方案方法有用的特定类型的魔方。*/
#ifndef	_cube_h
#define	_cube_h

class Cube
{
public:
	// Cubie locations(魔方块位置)
	enum EdgeCubie {
		// Edge locations(边块位置)
		UF =  0, UL =  1, UB =  2, UR =  3,
		FU =  0, LU =  1, BU =  2, RU =  3,

		DF =  4, DL =  5, DB =  6, DR =  7,
		FD =  4, LD =  5, BD =  6, RD =  7,
		// Middle slice edges begin here(中间切片边块从此处开始)
		RF =  8, FL =  9, LB = 10, BR = 11,
		FR =  8, LF =  9, BL = 10, RB = 11,

		FirstEdgeCubie = UF,
		LastEdgeCubie = BR,
		FirstMiddleSliceEdgeCubie = RF,
		LastMiddleSliceEdgeCubie = BR,
		NumberOfEdgeCubies = LastEdgeCubie+1 };

	// Corner locations(角块位置)
	enum CornerCubie {
		URF = 0, UFL = 1, ULB = 2, UBR = 3,
		RFU = 0, FLU = 1, LBU = 2, BRU = 3,
		FUR = 0, LUF = 1, BUL = 2, RUB = 3,

		DFR = 4, DLF = 5, DBL = 6, DRB = 7,
		FRD = 4, LFD = 5, BLD = 6, RBD = 7,
		RDF = 4, FDL = 5, LDB = 6, BDR = 7,

		FirstCornerCubie = URF,
		LastCornerCubie = DRB,
		NumberOfCornerCubies = LastCornerCubie+1 };

	// Applies to all cubies(适用于所有方块)
	enum Cubie { InvalidCubie = LastEdgeCubie+1 };

	// Twists(拧(角块))
	enum Twists {
		NoQuark, Quark, AntiQuark,
		NumberOfTwists = AntiQuark+1 };
		
	// Flips(翻转(边块))
	enum Flips {
		NotFlipped, Flipped };

	// Quarter and half turn moves(四分之一圈和半圈)
	enum Move {
		R,  L,  U,  D,  F,  B,
		Ri, Li, Ui, Di, Fi, Bi,
		R2, L2, U2, D2, F2, B2,

		FirstMove = R,
		LastMove = B2,
		NumberOfClockwiseQuarterTurnMoves = B+1,
		NumberOfMoves = LastMove+1 };

	// Default constructor(默认构造函数)
	Cube(void);

	// Destructor(析构函数)
	virtual ~Cube();

	// Overloaded equality test operator(重载 == 操作符)
	int operator==(const Cube& cube);

	// Overloaded inequality test operator(重载 == 操作符)
	int operator!=(const Cube& cube);
	
	// Reset cube back to HOME position(将魔方状态重置回原位)
	virtual void BackToHome(void);

	// Set state from permutation and orientation vectors(根据排列和方向向量设置魔方状态)
	virtual void SetState(
		int* cornerPermutation, int* cornerOrientation,
		int* edgePermutation, int* edgeOrientation);
	
	// Apply move(应用动作)
	virtual void ApplyMove(int move);

	// Get inverse of move(逆向移动)
	inline static int InverseOfMove(int move)
		{ return InverseMoves[move]; }

	// Turns a quarter turn move to a half turn move (e.g. R and Ri become R2)(转四分之一圈到半圈（例如R和Ri变成R2）)
	inline static int QuarterTurnToHalfTurnMove(int move)
		{ return R2+move%(B+1); }

	// Get opposite face of a move(获取对立面)
	inline static int OpposingFace(int move)
		{ return OppositeFaces[move]; };
	
	// Get the name of a move(获取动作名称)
	inline static char NameOfMove(int move)
		{ return MoveNames[move]; }

	// Get the move from the move name(根据动作名称执行动作)
	static int MoveNameToMove(char moveName, int& move);

	// Dump cube state(销毁魔方状态)
	virtual void Dump(void);

protected:
	// Cube moves(魔方移动)
	void Move_R(void);
	void Move_L(void);
	void Move_U(void);
	void Move_D(void);
	void Move_F(void);
	void Move_B(void);
	void Move_Ri(void);
	void Move_Li(void);
	void Move_Ui(void);
	void Move_Di(void);
	void Move_Fi(void);
	void Move_Bi(void);
	void Move_R2(void);
	void Move_L2(void);
	void Move_U2(void);
	void Move_D2(void);
	void Move_F2(void);
	void Move_B2(void);

	// Cycle four edge cubies(循环四个边块)
	void FourCycle(
			EdgeCubie first,
			EdgeCubie second,
			EdgeCubie third,
			EdgeCubie fourth);

	// Cycle four corner cubies(循环四个角块)
	void FourCycle(
			CornerCubie first,
			CornerCubie second,
			CornerCubie third,
			CornerCubie fourth);

	// Cycle four vector elements(循环四个向量元素)
	void CycleFour(int* vector,
			int first,
			int second,
			int third,
			int fourth);

	// Flip an edge cubie(翻转边块)
	void Flip(EdgeCubie cubie);

	// Corner cubie twists(角块拧动)
	void ClockwiseTwist(CornerCubie cubie);
	void CounterClockwiseTwist(CornerCubie cubie);

	// The cubies(方块)
	int CornerCubiePermutations[NumberOfCornerCubies];
	int CornerCubieOrientations[NumberOfCornerCubies];

	int EdgeCubiePermutations[NumberOfEdgeCubies];
	int EdgeCubieOrientations[NumberOfEdgeCubies];

private:
	// Twist tables(拧动表)
	static int ClockwiseTwists[NumberOfTwists];
	static int CounterClockwiseTwists[NumberOfTwists];

	// Names(动作名字)
	static char MoveNames[NumberOfMoves];

	// Move inverse(反向动作)
	static Cube::Move InverseMoves[NumberOfMoves];
	// Opposing faces(对立面)
	static Cube::Move OppositeFaces[NumberOfMoves];
};

#endif	// _cube_h
