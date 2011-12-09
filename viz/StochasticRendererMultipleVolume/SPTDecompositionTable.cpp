//
//  SPTDecompositionTable.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include "SPTDecompositionTable.h"

namespace kvs
{

namespace SPTDecompositionTable
{

const int DecompositionInfo[81][5] =
{
// class 1(+++- or +---): total  8 patterns (4C1 x 2)
// class 2(++--        ): total  6 patterns (4C2)
// class 3(++-0 or +--0): total 24 patterns (4C1 x 3C1 x 2)
// class 4(+-00        ): total 12 patterns (4C2 x 2C1)
// 321
// | 230
// | | 103
// | | | 012
// | | | |
/* - - - - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - - - 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - - - + */   { 1, 3, 0, 1, 2 }, // class 1, 3-(0,1,2)
/* - - 0 - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - - 0 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - - 0 + */   { 3, 2, 3, 1, 0 }, // class 3, 2-(3-(1,0))
/* - - + - */   { 1, 2, 1, 0, 3 }, // class 1, 2-(1,0,3)
/* - - + 0 */   { 3, 3, 2, 0, 1 }, // class 3, 3-(2-(0,1))
/* - - + + */   { 2, 0, 1, 2, 3 }, // class 2, (0,1)-(2,3)
/* - 0 - - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - 0 - 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - 0 - + */   { 3, 1, 3, 0, 2 }, // class 3, 1-(3-(0,2))
/* - 0 0 - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - 0 0 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* - 0 0 + */   { 4, 2, 1, 0, 3 }, // class 4, 1-2-(0,3)
/* - 0 + - */   { 3, 1, 2, 3, 0 }, // class 3, 1-(2,(3,0))
/* - 0 + 0 */   { 4, 3, 1, 0, 2 }, // class 4, 3-1-(0,2)
/* - 0 + + */   { 3, 1, 0, 3, 2 }, // class 3, 1-(0,(3,2))
/* - + - - */   { 1, 1, 2, 3, 0 }, // class 1, 1-(2,3,0)
/* - + - 0 */   { 3, 3, 1, 2, 0 }, // class 3, 3-(1-(2,0))
/* - + - + */   { 2, 0, 2, 1, 3 }, // class 2, (0,2)-(1,3)
/* - + 0 - */   { 3, 2, 1, 0, 3 }, // class 3, 2-(1,(0,3))
/* - + 0 0 */   { 4, 2, 3, 0, 1 }, // class 4, 2-3-(0,1)
/* - + 0 + */   { 3, 2, 0, 3, 1 }, // class 3, 2-(0,(3,1))
/* - + + - */   { 2, 3, 0, 1, 2 }, // class 2, (3,0)-(1,2)
/* - + + 0 */   { 3, 3, 0, 1, 2 }, // class 3, 3-(0,(1,2))
/* - + + + */   { 1, 0, 3, 2, 1 }, // class 1, 0-(3,2,1)
// 321
// | 230
// | | 103
// | | | 012
// | | | |
/* 0 - - - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 - - 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 - - + */   { 3, 0, 3, 2, 1 }, // class 3, 0-(3,(2,1))
/* 0 - 0 - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 - 0 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 - 0 + */   { 4, 2, 0, 1, 3 }, // class 4, 2-0-(1,3)
/* 0 - + - */   { 3, 0, 2, 1, 3 }, // class 3, 0-(2-(1,3))
/* 0 - + 0 */   { 4, 0, 3, 1, 2 }, // class 4, 0-3-(1,2)
/* 0 - + + */   { 3, 0, 1, 3, 2 }, // class 3, 0-(1-(3,2))
/* 0 0 - - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 - 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 - + */   { 4, 0, 1, 2, 3 }, // class 4, 0-1-(2,3)
/* 0 0 0 - */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 0 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 0 + */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 + - */   { 4, 1, 0, 3, 2 }, // class 4, 1-0-(3,2)
/* 0 0 + 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 0 + + */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 + - - */   { 3, 0, 1, 3, 2 }, // class 3, 0-(1-(3,2))
/* 0 + - 0 */   { 4, 3, 0, 2, 1 }, // class 4, 3-0-(2,1)
/* 0 + - + */   { 3, 0, 2, 1, 3 }, // class 3, 0-(2-(1,3))
/* 0 + 0 - */   { 4, 0, 2, 3, 1 }, // class 4, 0-2-(3,1)
/* 0 + 0 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 + 0 + */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 + + - */   { 3, 0, 3, 2, 1 }, // class 3, 0-(3-(2,1))
/* 0 + + 0 */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* 0 + + + */                              { 0, 0, 1, 2, 3 }, // this pattern is not exist.
// 321
// | 230
// | | 103
// | | | 012
// | | | |
/* + - - - */   { 1, 0, 3, 2, 1 }, // class 1, 0-(3,2,1)
/* + - - 0 */   { 3, 3, 0, 1, 2 }, // class 3, 3-(0-(1,2))
/* + - - + */   { 2, 1, 2, 3, 0 }, // class 2, (1,2)-(3,0)
/* + - 0 - */   { 3, 2, 0, 3, 1 }, // class 3, 2-(0-(3,1))
/* + - 0 0 */   { 4, 3, 2, 1, 0 }, // class 4, 3-2-(1,0)
/* + - 0 + */   { 3, 2, 1, 0, 3 }, // class 3, 2-(1-(0,3))
/* + - + - */   { 2, 1, 3, 0, 2 }, // class 2, (1,3)-(0,2)
/* + - + 0 */   { 3, 3, 1, 2, 0 }, // class 3, 3-(1-(2,0))
/* + - + + */   { 1, 1, 2, 3, 0 }, // class 1, 1-(2,3,0)
/* + 0 - - */   { 3, 1, 0, 2, 3 }, // class 3, 1-(0-(2,3))
/* + 0 - 0 */   { 4, 1, 3, 2, 0 }, // class 4, 1-3-(2,0)
/* + 0 - + */   { 3, 1, 2, 3, 0 }, // class 3, 1-(2-(3,0))
/* + 0 0 - */   { 4, 2, 1, 3, 0 }, // class 4, 2-1-(3,0)
/* + 0 0 0 */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + 0 0 + */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + 0 + - */   { 3, 1, 3, 0, 2 }, // class 3, 1-(3-(0,2))
/* + 0 + 0 */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + 0 + + */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + + - - */   { 2, 2, 3, 0, 1 }, // class 2, (2,3)-(0,1)
/* + + - 0 */   { 3, 3, 2, 0, 1 }, // class 3, 3-(2-(0,1))
/* + + - + */   { 1, 2, 1, 0, 3 }, // class 1, 2-(1,0,3)
/* + + 0 - */   { 3, 2, 3, 1, 0 }, // class 3, 2-(3-(1,0))
/* + + 0 0 */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + + 0 + */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + + + - */   { 1, 3, 0, 1, 2 }, // class 1, 3-(0,1,2)
/* + + + 0 */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
/* + + + + */                               { 0, 0, 1, 2, 3 }, // this pattern is not exist.
};

} // end of namespace SPTDecompositionTable

} // end of namespace kvs

