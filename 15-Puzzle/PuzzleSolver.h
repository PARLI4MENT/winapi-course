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
		CPath Path{};

		CState( CPositions positions, int blankRow, int blankColumn, const CPath& path );

		bool operator==( const CState& other ) const;
	};

	class CStateHasher {
	public:
		std::size_t operator()( const CState& other ) const;
	};

public:
	static CMove GetHint( const CPositions& initialPositions );

private:
	static const std::array<CMove, 4> moves;
	
	static CPath solve( const CPositions& initialPositions );
	static int getBlankIndex( const CPositions& positions );
	static bool isFinishPositions( const CPositions& positions );
};
