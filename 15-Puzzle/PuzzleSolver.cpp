#include "PuzzleSolver.h"

#include <cmath>
#include <unordered_set>
#include <set>
#include <queue>

CMove::CMove( int rowDifference, int columnDifference ) :
	RowDifference( rowDifference ),
	ColumnDifference( columnDifference )
{
}

//----------------------------------------------------------------------------------------------------------------------

CPuzzleSolver::CState::CState( const CPositions& positions, int blankRow, int blankColumn, const std::vector<__int8>& moveIndices, int distance, int estimation ) :
	Positions( positions ),
	BlankRow( blankRow ),
	BlankColumn( blankColumn ),
	MoveIndices( moveIndices ),
	Distance( distance ),
	Estimation( estimation )
{
}

bool CPuzzleSolver::CState::operator==( const CState& other ) const
{
	for( int i = 0; i < Positions.size(); ++i ) {
		if( Positions[i] != other.Positions[i] ) {
			return false;
		}
	}
	return true;
}

bool CPuzzleSolver::CState::operator<( const CState & other ) const
{
	if( Distance + Estimation < other.Distance + other.Estimation ) {
		return true;
	} else if( Distance + Estimation == other.Distance + other.Estimation ) {
		for( int i = 0; i < Positions.size(); ++i ) {
			if( Positions[i] < other.Positions[i] ) {
				return true;
			} else if( Positions[i] > other.Positions[i] ) {
				return false;;
			}
		}
	}
	return false;
}

std::size_t CPuzzleSolver::CStateHasher::operator()( const CState& state ) const
{
	std::size_t hash = 0;
	for( auto& position : state.Positions ) {
		hash = ( 23 * hash + position ) % 18446744073689561299ULL;
	}
	return hash;
}

//----------------------------------------------------------------------------------------------------------------------

const std::array<CMove, 4> CPuzzleSolver::moves{ CMove{ -1, 0 }, CMove{ 0, -1 }, CMove{ 1, 0 }, CMove{ 0, 1 } };

CMove CPuzzleSolver::GetHint( const CPositions& initialPositions )
{
	return moves[solve( initialPositions ).front()];
}

bool CPuzzleSolver::IsFinishPositions( const CPositions& positions, int tail )
{
	for( int i = 0; i < positions.size() - tail; ++i ) {
		if( positions[i] != i + 1 ) {
			return false;
		}
	}
	return true;
}

std::vector<__int8> CPuzzleSolver::solve( const CPositions& initialPositions )
{
	int degree = std::floor( std::sqrt( initialPositions.size() ) + 0.5 );

	std::unordered_set<CState, CStateHasher> visited{};
	std::set<CState> states{};

	int blankIndex = getBlankIndex( initialPositions );
	int blankRow = blankIndex / degree;
	int blankColumn = blankIndex % degree;

	auto state = CState( initialPositions, blankRow, blankColumn, std::vector<__int8>{}, 0, findEstimation( initialPositions, degree ) );
	states.insert( state );
	visited.insert( state );

	while( !states.empty() ) {
		state = *states.begin();
		states.erase(states.begin());

		for( int i = 0; i < moves.size(); ++i ) {
			auto move = moves[i];
			int row = state.BlankRow + move.RowDifference;
			int column = state.BlankColumn + move.ColumnDifference;
			if( row < 0 || column < 0 || row >= degree || column >= degree ) {
				continue;
			}

			auto nextState = CState( state.Positions, row, column, state.MoveIndices, state.Distance + 1, 0 );

			int blankIndex = state.BlankRow * degree + state.BlankColumn;
			int index = row * degree + column;

			std::swap( nextState.Positions[index], nextState.Positions[blankIndex] );

			nextState.MoveIndices.push_back( i );
			nextState.Estimation = findEstimation( nextState.Positions, degree );

			if( visited.find( nextState ) == visited.end() ) {
				states.insert( nextState );
				visited.insert( nextState );

				if( IsFinishPositions( nextState.Positions, 1 ) ) {
					return nextState.MoveIndices;
				}
			}
		}
	}
}

int CPuzzleSolver::getBlankIndex( const CPositions& positions )
{
	return  std::find( positions.begin(), positions.end(), 0 ) - positions.begin();
}

int CPuzzleSolver::findEstimation( const CPositions& permutation, int degree )
{
	int sum = 0;
	for( int i = 0; i < permutation.size(); ++i ) {
		int value = permutation[i];
		int expectedRow = value != 0 ? value / degree : degree - 1;
		int expectedColumn = value != 0 ? value % degree : degree - 1;
		int row = i / degree;
		int column = i % degree;
		sum += std::abs( row - expectedRow ) + std::abs( column - expectedColumn );
	}
	return sum;
}
