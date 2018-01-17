/* Copyright (C) 2018 Arnaud Sipasseuth.

   This file is part of fplll. fplll is free software: you
   can redistribute it and/or modify it under the terms of the GNU Lesser
   General Public License as published by the Free Software Foundation,
   either version 2.1 of the License, or (at your option) any later version.

   fplll is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with fplll. If not, see <http://www.gnu.org/licenses/>. */

/* Template source file */

#include "hnf.h"
#include "util.h"

FPLLL_BEGIN_NAMESPACE

int in_hnf(ZZ_mat<mpz_t> &B, const vector<mpz_t> &w){

  vector<mpz_t> v = w;

  /* matrix bounds */
  int r = B.get_rows(), c = B.get_cols();
  /* test if the vector size is correct */
  if (v.size() != c) {
    cerr << "in_hnf error : matrix-vector sizes do not match\n";
    return -1;
  }
  /* matrix indexes (k = pivot)*/
  int i, j, j2, k;
  /* the limit "l" depends of matrix dimensions */
  int l = (c - r) * (c > r);

  /* main loop, decreases from last column to the limit */
  for (j = c - 1, k = r - 1; j >= l; j--, k--)
  {
    // checks if everything above diagonal zero
    for (i = k - 1; i >=0 ; i--) {
      if (B[i][j] != 0) {return 1;}
    }
    // checks if diagonal is non-negative
    if (B[k][j] < 0) {
      return 1;
    }
    // if "diagonal" entry is zero, we have to "shift" the diagonal position
    else if (B[k][j] == 0)
    {
      // pivot row position doesn't change, increment to compensate
      k++;
      // lower the limit as we skipped one column without increasing row
      if (l > 0) {l--;}
    }
    else
    {
      // checks if everything below is positive and below the diagonal
      for (i = k + 1; i < r ; i++) {
        if ( (B[i][j] < B[k][j]) && (B[i][j] > 0) ) {return 1;}
      }
      // reduces the vector by the row pivot vector for membership test
      q.fdiv_q(v[j], B[k][j]);
      for (j2 = j; j2 >= 0; j2--)
      {
        v[j2].submul(q, B[k][j2]);
      }
    }
  }

  // membership test : the vector after reduction should be a zero one.
  for ( j = 0; i < c; i++) {
    if (v[i] != 0) {return -1;}
  }

  return 0;
}

int hnf_xgcd_reduction(ZZ_mat<mpz_t> &B)
{
  /* matrix indexes (k = pivot)*/
  int i, j, j2, k;
  /* matrix bounds */
  int r = B.get_rows(), c = B.get_cols();
  /* ZT integers for operations */
  Z_NR<mpz_t> r1d, r2d, b, u, v, d, q;

  /* initializes the transformation matrix */
  // U.gen_identity(B.get_rows());

  /* the limit "l" depends of matrix dimensions */
  int l = (c - r) * (c > r);

  /* main loop, decreases from last column to the limit */
  for (j = c - 1, k = r - 1; j >= l; j--, k--)
  {
    /* iterates above the pivot */
    for (i = k - 1; i >= 0; i--)
    {
      /* skip zeroes (if any) */
      if (B[i + 1][j] == 0) {continue;}

      /* reduce row i + 1 with row i */
      d.xgcd(u, v, B[i][j], B[i + 1][j]);
      r2d.divexact(B[i + 1][j], d);
      r1d.divexact(B[i][j], d);
      /* only compute relevant values (rest should be guaranteed zero) */
      for (j2 = j; j2 >= 0; j2--)
      {
        /* precompute values */
        b.mul(u, B[i][j2]);
        b.addmul(v, B[i + 1][j2]);
        /* new vector i-1 value */
        B[i + 1][j2].mul(r1d, B[i + 1][j2]);
        B[i + 1][j2].submul(r2d, B[i][j2]);
        /* new vector i value */
        B[i][j2] = b;
      }
    }

    /* swap final row to its rightful pivot position */
    B.swap_rows(0, k);

    // change sign of the row vector if the diagonal entry is negative
    if (B[k][j] < 0)
    {
      for (j2 = j; j2 >= 0; j2--)
      {
        B[k][j2].neg(B[k][j2]);
      }
    }
    // if diagonal entry is zero just skip to the next step
    if (B[k][j] == 0)
    {
      // pivot row position doesn't change, increment to compensate
      k++;
      // lower the limit as we skipped one column without increasing row
      if (l > 0) {l--;}
    }
    else
    {
      /* reduce lower entries of column j with row k */
      for (i = k + 1; i < r ; i++)
      {
        q.fdiv_q(B[i][j], B[k][j]);
        for (j2 = j; j2 >= 0; j2--)
        {
          B[i][j2].submul(q, B[k][j2]);
        }
      }
    }
  }

  /* should always return 0 (can be changed in the future) */
  return 0;
}

FPLLL_END_NAMESPACE