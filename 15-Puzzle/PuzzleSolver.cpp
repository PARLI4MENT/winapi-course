#include "PuzzleSolver.h"

#include <cmath>
#include <set>
#include <queue>

CMove::CMove( int rowDifference, int columnDifference ) :
	RowDifference( rowDifference ),
	ColumnDifference( columnDifference )
{
}

//----------------------------------------------------------------------------------------------------------------------

CPuzzleSolver::CState::CState( CPositions positions, int blankRow, int blankColumn, const CPath& path ) :
	Positions( positions ),
	BlankRow( blankRow ),
	BlankColumn( blankColumn ),
	Path( path )
{
}

//----------------------------------------------------------------------------------------------------------------------

const std::array<CMove, 4> CPuzzleSolver::moves{ CMove{ -1, 0 }, CMove{ 0, -1 }, CMove{ 1, 0 }, CMove{ 0, 1 } };

CMove CPuzzleSolver::GetHint( const CPositions& initialPositions )
{
	return solve( initialPositions ).front();
}

CPuzzleSolver::CPath CPuzzleSolver::solve( const CPositions& initialPositions )
{
	int degree = std::floor( std::sqrt( initialPositions.size() ) + 0.5 );

	std::set<CState> visited{};
	std::queue<CState> states{};

	int blankIndex = getBlankIndex( initialPositions );
	int blankRow = blankIndex / degree;
	int blankColumn = blankIndex % degree;

	auto state = CState( initialPositions, blankRow, blankColumn, CPath{} );
	states.push( state );
	visited.insert( state );

	while( !states.empty() ) {
		state = states.front();
		if( isFinishPositions( state.Positions ) ) {
			break;
		}
		states.pop();

		for( auto& move : moves ) {
			int row = state.BlankRow + move.RowDifference;
			int column = state.BlankColumn + move.ColumnDifference;
			if( row < 0 || column < 0 || row > degree || column > degree ) {
				continue;
			}

			auto nextState = CState( state.Positions, row, column, state.Path );

			int blankIndex = state.BlankRow * degree + state.BlankColumn;
			int index = row * degree + column;
			std::swap( nextState.Positions[index], nextState.Positions[blankIndex] );

			state.Path.push_back( move );

			if( visited.find( nextState ) == visited.end() ) {
				states.push( nextState );
				visited.insert( nextState );
			}
		}
	}

	return state.Path;
}

int CPuzzleSolver::getBlankIndex( const CPositions& positions )
{
	return  std::find( positions.begin(), positions.end(), 0 ) - positions.begin();;
}

bool CPuzzleSolver::isFinishPositions( const CPositions& positions )
{
	for( int i = 0; i < positions.size() - 1; ++i ) {
		if( positions[i] != i + 1 ) {
			return false;
		}
	}
	return true;
}
