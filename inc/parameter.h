#ifndef _PARAMETER_H
#define _PARAMETER_H

#define CELL_DISTANCE       140
#define GUARD_RING_WIDTH    451
#define GUARD_RING_HEIGHT   240
#define POLY_TO_POLY        118
#define DUMMY_POLY_WIDTH    72
#define POLY_TO_WELL        59
#define OD_TO_WELL_TOP      120
#define OD_TO_WELL_BOTTOM   224
#define FIN_SPACE           48
#define FIN_HIGH            10
#define WELL_TO_GUARD_RING  140
#define WELL_TO_WELL        140
#define GR_TO_GR            400

#define K                   3       //Wong Liu Algorithm裡的參數K
#define RATIO               0.8     //Wong Liu Algorithm裡溫度T變化的比例
#define FROZEN_RATIO        0.1     //Wong Liu Algorithm裡，當溫度低於某值時，比例降低快速冷卻
#define SEED                173     //srand(SEED)
#define EPSILON             0.01     //Wong Liu Algorithm裡的參數epsilon
#define KN                  10
#define C                   100000  
#define TIME_LIMIT          1200
#define TIME_LIMIT2         600

#define RATIO_PERTURB_FHBTREE       10
#define RATIO_PERTURB_THBTREE       10
#define RATIO_PERTURB_HBTREE        40
#define RATIO_PERTURB_ASFBTREE      40
#define RATIO_PERTURB_ASFGUARDRING  10 

inline int PROBABILITY_FHBTREE      = RATIO_PERTURB_FHBTREE;
inline int PROBABILITY_THBTREE      = RATIO_PERTURB_THBTREE + PROBABILITY_FHBTREE;
inline int PROBABILITY_HBTREE       = RATIO_PERTURB_HBTREE + PROBABILITY_THBTREE;
inline int PROBABILITY_ASFBTREE     = RATIO_PERTURB_ASFBTREE + PROBABILITY_HBTREE;
inline int PROBABILITY_ASFGUARDRING = RATIO_PERTURB_ASFGUARDRING + PROBABILITY_ASFBTREE;

#endif