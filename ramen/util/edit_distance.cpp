// Copyright (c) 2011 Esteban Tovagliari.
// 
// Based on Levenshtein Distance Algorithm:
// public domain C++ implementation by Anders Sewerin Johansen.

#include<ramen/util/edit_distance.hpp>

#include<algorithm>

#include<ramen/assert.hpp>

namespace ramen
{
namespace util
{

edit_distance_t::edit_distance_t() : rows_( 0), cols_( 0) {}

int edit_distance_t::operator()( const std::string& src, const std::string& dst)
{
	int n = src.length();
	int m = dst.length();
		
	if( n == 0)
	  return m;

	if( m == 0)
	  return n;
		
	if( rows_ < n + 1 || cols_ < m + 1)
		realloc_matrix( n + 1, m + 1);
		
	for( int i = 0; i <= n; i++)
	  matrix( i, 0) = i;
	  
	for( int j = 0; j <= m; j++)
	  matrix( 0, j) = j;
		
	for( int i = 1; i <= n; i++)
	{
		char s_i = src[i-1];
			
		for( int j = 1; j <= m; j++)
		{	  
			char t_j = dst[j-1];
			int cost;
				
			if (s_i == t_j)
				cost = 0;
			else
				cost = 1;
								
			int above = matrix( i - 1, j);
			int left = matrix( i, j - 1);
			int diag = matrix( i - 1, j - 1);
			int cell = std::min( above + 1, std::min( left + 1, diag + cost));
							
			if( i > 2 && j > 2)
			{
				int trans = matrix( i - 2, j - 2) + 1;
				
				if( src[i-2] != t_j)
					++trans;
				
				if( s_i != dst[j-2])
					++trans;
			
				if( cell > trans)
					cell = trans;
			}
				
			matrix( i, j) = cell;
		}
	}
	  
	return matrix( n, m);
}
	
void edit_distance_t::realloc_matrix( int rows, int cols)
{
	matrix_.reset( new int[rows * cols]);
	rows_ = rows;
	cols_ = cols;
}
	
} // util
} // ramen

/*
int distance(const std::string src, const std::string dst)
{

  // Step 1

  const int n = src.length();
  const int m = dst.length();
  if (n == 0) {
    return m;
  }
  if (m == 0) {
    return n;
  }

  // Good form to declare a TYPEDEF

  typedef std::vector< std::vector<int> > Tmatrix; 

  Tmatrix matrix(n+1);

  // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
  // allow for allocation on declaration of 2.nd dimension of vec of vec

  for (int i = 0; i <= n; i++) {
    matrix[i].resize(m+1);
  }

  // Step 2

  for (int i = 0; i <= n; i++) {
    matrix[i][0]=i;
  }

  for (int j = 0; j <= m; j++) {
    matrix[0][j]=j;
  }

  // Step 3

  for (int i = 1; i <= n; i++) {

    const char s_i = src[i-1];

    // Step 4

    for (int j = 1; j <= m; j++) {

      const char t_j = dst[j-1];

      // Step 5

      int cost;
      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }

      // Step 6

      const int above = matrix( i-1, j);
      const int left = matrix( i, j - 1);
      const int diag = matrix[i-1][j-1];
      const int cell = min( above + 1, min(left + 1, diag + cost));

      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)

      if (i>2 && j>2) {
        int trans=matrix[i-2][j-2]+1;
        if (src[i-2]!=t_j) trans++;
        if (s_i!=dst[j-2]) trans++;
        if (cell>trans) cell=trans;
      }

      matrix[i][j]=cell;
    }
  }

  // Step 7

  return matrix[n][m];
}
*/

