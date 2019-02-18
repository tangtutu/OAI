#include "PHY/defs_UE.h"
#include "PHY/defs_common.h"
const int trp8[TRP8_MAX+1][8] = {{1,0,0,0,0,0,0,0},
				 {0,1,0,0,0,0,0,0},
				 {0,0,1,0,0,0,0,0},
				 {0,0,0,1,0,0,0,0},
				 {0,0,0,0,1,0,0,0},
				 {0,0,0,0,0,1,0,0},
				 {0,0,0,0,0,0,1,0},
				 {0,0,0,0,0,0,0,1},
				 {1,1,0,0,0,0,0,0},
				 {1,0,1,0,0,0,0,0},
				 {0,1,1,0,0,0,0,0},
				 {1,0,0,1,0,0,0,0},
				 {0,1,0,1,0,0,0,0},
				 {0,0,1,1,0,0,0,0},
				 {1,0,0,0,1,0,0,0},
				 {0,1,0,0,1,0,0,0},
				 {0,0,1,0,1,0,0,0},
				 {0,0,0,1,1,0,0,0},
				 {1,0,0,0,0,1,0,0},
				 {0,1,0,0,0,1,0,0},
				 {0,0,1,0,0,1,0,0},
				 {0,0,0,1,0,1,0,0},
				 {0,0,0,0,1,1,0,0},
				 {1,0,0,0,0,0,1,0},
				 {0,1,0,0,0,0,1,0},
				 {0,0,1,0,0,0,1,0},
				 {0,0,0,1,0,0,1,0},
				 {0,0,0,0,1,0,1,0},
				 {0,0,0,0,0,1,1,0},
				 {1,0,0,0,0,0,0,1},
				 {0,1,0,0,0,0,0,1},
				 {0,0,1,0,0,0,0,1},
				 {0,0,0,1,0,0,0,1},
				 {0,0,0,0,1,0,0,1},
				 {0,0,0,0,0,1,0,1},
				 {0,0,0,0,0,0,1,1},
				 {1,1,1,1,0,0,0,0},
				 {1,1,1,0,1,0,0,0},
				 {1,1,0,1,1,0,0,0},
				 {1,0,1,1,1,0,0,0},
				 {0,1,1,1,1,0,0,0},
				 {1,1,1,0,0,1,0,0},
				 {1,1,0,1,0,1,0,0},
				 {1,0,1,1,0,1,0,0},
				 {0,1,1,1,0,1,0,0},
				 {1,1,0,0,1,1,0,0},
				 {1,0,1,0,1,1,0,0},
				 {0,1,1,0,1,1,0,0},
				 {1,0,0,1,1,1,0,0},
				 {0,1,0,1,1,1,0,0},
				 {0,0,1,1,1,1,0,0},
				 {1,1,1,0,0,0,1,0},
				 {1,1,0,1,0,0,1,0},
				 {1,0,1,1,0,0,1,0},
				 {0,1,1,1,0,0,1,0},
				 {1,1,0,0,1,0,1,0},
				 {1,0,1,0,1,0,1,0},
				 {0,1,1,0,1,0,1,0},
				 {1,0,0,1,1,0,1,0},
				 {0,1,0,1,1,0,1,0},
				 {0,0,1,1,1,0,1,0},
				 {1,1,0,0,0,1,1,0},
				 {1,0,1,0,0,1,1,0},
				 {0,1,1,0,0,1,1,0},
				 {1,0,0,1,0,1,1,0},
				 {0,1,0,1,0,1,1,0},
				 {0,0,1,1,0,1,1,0},
				 {1,0,0,0,1,1,1,0},
				 {0,1,0,0,1,1,1,0},
				 {0,0,1,0,1,1,1,0},
				 {0,0,0,1,1,1,1,0},
				 {1,1,1,0,0,0,0,1},
				 {1,1,0,1,0,0,0,1},
				 {1,0,1,1,0,0,0,1},
				 {0,1,1,1,0,0,0,1},
				 {1,1,0,0,1,0,0,1},
				 {1,0,1,0,1,0,0,1},
				 {0,1,1,0,1,0,0,1},
				 {1,0,0,1,1,0,0,1},
				 {0,1,0,1,1,0,0,1},
				 {0,0,1,1,1,0,0,1},
				 {1,1,0,0,0,1,0,1},
				 {1,0,1,0,0,1,0,1},
				 {0,1,1,0,0,1,0,1},
				 {1,0,0,1,0,1,0,1},
				 {0,1,0,1,0,1,0,1},
				 {0,0,1,1,0,1,0,1},
				 {1,0,0,0,1,1,0,1},
				 {0,1,0,0,1,1,0,1},
				 {0,0,1,0,1,1,0,1},
				 {0,0,0,1,1,1,0,1},
				 {1,1,0,0,0,0,1,1},
				 {1,0,1,0,0,0,1,1},
				 {0,1,1,0,0,0,1,1},
				 {1,0,0,1,0,0,1,1},
				 {0,1,0,1,0,0,1,1},
				 {0,0,1,1,0,0,1,1},
				 {1,0,0,0,1,0,1,1},
				 {0,1,0,0,1,0,1,1},
				 {0,0,1,0,1,0,1,1},
				 {0,0,0,1,1,0,1,1},
				 {1,0,0,0,0,1,1,1},
				 {0,1,0,0,0,1,1,1},
				 {0,0,1,0,0,1,1,1},
				 {0,0,0,1,0,1,1,1},
				 {0,0,0,0,1,1,1,1},
				 {1,1,1,1,1,1,1,1}};