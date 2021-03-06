#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <set>

#include "defs.h"
#include "global.h"
#include "aca.h"

using namespace std;

int * gF = NULL;
int * gP = NULL;
int gMatches = 0;
int gMax_alloc_matches = ALLOC_SIZE;

unsigned int WTM ( double z, string alphabet, vector < unsigned int > * Occ )
{
	unsigned int sigma		= alphabet.size();
	unsigned int m			= xy.pattern.size();
	unsigned int n			= xy.text.size();
	unsigned char * x		= new unsigned char [m + 1];
	unsigned char * y		= new unsigned char [n + 1];
	unsigned int num_frag	= 1;
	unsigned int num_Occ	= 0;

	cout << "pattern length: " << m << "\t text length: " << n << endl;

	for ( unsigned int i = 0; i < m; i++ )
	{
		x[i] = alphabet[ xy.pattern[i] ];
	}
	x[m] = '\0';
	
	for ( unsigned int i = 0; i < n; i++ )
	{  
		unsigned int ch = xy.text[i];
		if ( ch < sigma )
		{
			y[i] = alphabet[ch];
		}
		else
		{
			y[i] = '$';
		}
	}
	y[n] = '\0';

	unsigned int l = 0;
	for ( unsigned int i = 0; i < m; i++ )
	{
		if ( y[i] == '$' )
			l += 1;
	}

	unsigned int ll = l;
	for ( unsigned int i = m; i < n; i++ )
	{
		if ( y[i] == '$' && y[ i - m ] != '$' )
		{
			ll += 1;
		}
		if ( y[i] != '$' && y[ i - m ] == '$' ) 
		{
			ll -= 1;
		}
		
		if ( ll > l )
			l = ll;
	}	
	num_frag = l + 1;

	Factor * F	= new Factor [num_frag];
	int	*	ind	= new int [num_frag];		//the starting position of fragments
	int	*	mf	= new int [num_frag];		//the length of fragments

	for ( unsigned int i = 0; i < num_frag; i++ )
	{
		fragments ( 0, i, num_frag, m, mf, ind );
		F[i].length	= mf[i];
		F[i].start	= ind[i];
		F[i].end	= ind[i] + mf[i] - 1;
		F[i].prob	= 1;
	}

	/* Check whether there exist duplicated fragments */
	unsigned char ** seqs;
	int	 *	dups;

	dups = new int [num_frag];
	unsigned int uniq;
	uniq = extract_dups_single_str ( x, m, num_frag, mf, ind, dups );

	int * d_occ = new int [num_frag];
	int * l_occ = new int [num_frag];

	for ( unsigned int i = 0; i < num_frag; i++ )
	{
		d_occ[i] = -1;
		l_occ[i] = -1;
	}
 
	/* In case there exist duplicated fragments */
	if ( uniq < num_frag ) 
	{
		seqs = new unsigned char * [num_frag];
		for ( unsigned int i = 0; i < num_frag; i++ )
		{
			/* Add the fragment once */
			if ( dups[i] < 0 )
			{
				seqs[i] = new unsigned char [ mf[i] + 1 ];
				memmove ( seqs[i], x + ind[i], mf[i] );
				seqs[i][ mf[i] ] = '\0';
			}
			else
			{
				/* add nothing since it is already added */
				seqs[i] = new unsigned char [1];
				seqs[i][0] = '\0';
				if ( l_occ[dups[i]] == -1 )
					d_occ[dups[i]] = i;
				else
					d_occ[ l_occ[ dups[i] ] ] = i;
				l_occ[dups[i]] = i;
			}
		}
	}
	else
	{
		/* all all the fragments since there exist no duplicated fragments */
		seqs = new unsigned char * [num_frag];
		for ( unsigned int i = 0; i < num_frag; i++ )
		{
			seqs[i] = new unsigned char [ mf[i] + 1 ];
			memmove ( seqs[i], x + ind[i], mf[i] );
			seqs[i][mf[i]] = '\0';
		}
	}

	int * frag_id	= new int [ALLOC_SIZE];
	int * frag_occ	= new int [ALLOC_SIZE];
	int matches;

#if 1 
	/* Aho Corasick */
	gF = frag_id;
	gP = frag_occ;
	matches = gMatches;

	filtering ( ( char * ) y, n, ( char ** ) seqs, num_frag );

	frag_id	 = gF;
	frag_occ = gP;
	matches = gMatches;
#endif

	for ( unsigned int i = 0; i < matches; i++ )
	{
		int id = frag_id[i];
		do 
		{
			unsigned int ystart = frag_occ[i] - F[id].start;
			if ( ( frag_occ[i] >= F[id].start ) && ( n - frag_occ[i] >= m - F[id].start ) )
			{
				unsigned int flag = 1;
				double prob_Occ = 1;

				for ( unsigned int ii = frag_occ[i]; ii < frag_occ[i] + F[id].length - 1; ii++ )
				{
					prob_Occ *= xy.prob[ii];
				}

				if ( prob_Occ >= 1/z )
				{
					unsigned int j = 0;
					while ( j < m && ystart + j < n )
					{
						if ( xy.text[ystart + j] < sigma )
						{
							if ( xy.text[ystart + j] != xy.pattern[j] )
							{
								flag = 0;
								break;
							}
							else
							{
								prob_Occ *= xy.prob[ystart + j];
								if ( prob_Occ < 1/z )
								{
									flag = 0;
									break;
								}
								else
								{
									j++;
								}
							}
						}
						else
						{
							unsigned int row = xy.text[ystart + j] - sigma;
							unsigned int col = xy.pattern[j];
							prob_Occ *= xy.bptable[row][col];
							if ( prob_Occ < 1/z )
							{
								flag = 0;
								break;
							}
							else
							{
								j++;
							}
						}
						if ( j == F[id].start )
						{
							j = F[id].end + 1;
						}
					}

					if ( flag )
					{
						Occ->push_back( ystart );
					}
				}
			}

			id = d_occ[id];

		} while ( id != -1 );
	}

#if 1
	/* remove the duplicates */
	set < unsigned int > unique_Occ ( Occ->begin(), Occ->end() );
	Occ->assign ( unique_Occ.begin(), unique_Occ.end() );
#endif
	num_Occ = Occ->size();

	delete[] F;
	delete[] ind;
	delete[] mf;
	delete[] frag_id;
	delete[] frag_occ;
	delete[] dups;
	delete[] d_occ;
	delete[] l_occ;

	for ( unsigned int i = 0; i < num_frag; ++i )
	{
		delete[] seqs[i];
	}
	delete[] seqs;

	delete[] x;
	delete[] y;

#if 1
	gF = NULL;
	gP = NULL;
	gMatches = 0;
	gMax_alloc_matches = ALLOC_SIZE;
#endif

	return num_Occ;
}



