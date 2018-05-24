#pragma once

#include <array>
#include <vector>

class CMove {
public:
	int RowDifference = 0;
	int ColumnDifference = 0;

	CMove( int rowDifference, int columnDifference );
};

//----------------------------------------------------------------------------------------------------------------------

class CPuzzleSolver {

	using CPath = std::vector<CMove>;
	using CPositions = std::vector<int>;

	class CState {
	public:
		CPositions Positions{};
		int BlankRow{ -1 };
		int BlankColumn{ -1 };
		std::vector<__int8> MoveIndices{};
		int Distance{ 0 };
		int Estimation{ 0 };

		CState( const CPositions& positions, int blankRow, int blankColumn, const std::vector<__int8>& moveIndices, int distance, int estimation );

		bool operator==( const CState& other ) const;
		bool operator<( const CState& other ) const;
	};

	class CStateHasher {
	public:
		std::size_t operator()( const CState& other ) const;
	};

public:
	static CMove GetHint( const CPositions& initialPositions );
	static bool IsFinishPositions( const CPositions& positions, int tail = 0 );

private:
	static const std::array<CMove, 4> moves;
	
	static std::vector<__int8> solve( const CPositions& initialPositions );
	static int getBlankIndex( const CPositions& positions );
	static int findEstimation( const CPositions& permutation, int degree );
};
