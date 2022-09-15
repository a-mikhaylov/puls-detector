#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

// #define INF 999999

struct puls {
    int32_t index;
    int32_t A;
};

class PulsDetector {
public:
    int32_t     i;
    int32_t     minDist;

    int32_t     LvP;
    int32_t     mxCnt;

    bool        lookMn;
    int32_t     mnCnt0;
    int32_t     mnCnt1;
    int32_t     mnVal0;
    int32_t     mnVal1;
    int32_t     mnPrevPos;

    float       minR;
    int32_t     LvZ;
    int32_t     LvR;
    int32_t     kR;

    float       kZ;
    bool        modeZero;
    int32_t     up, pkUp;
    int32_t     dn, pkDn;
    bool        lookDn;

	PulsDetector() {
		
	}

    bool exe(int tacho, int base) {
		if (mnVal0 >= base) { mnVal0 = base; mnCnt0 = 0; }
        if (mnVal1 >= base) { mnVal1 = base; mnCnt1 = 0; }
        
        if (modeZero) {
            if (tacho > LvZ * 0.1) {
                up          = i;
                modeZero    = false;
            }
        } else {
            if (tacho < LvZ * 0.1) {
                dn          = i;
                modeZero    = true;

                if (lookDn) {
                    pkDn    = dn;
                    lookDn  = false;
                }
            }
        }

        if (tacho > LvR) {
            LvR = tacho;

            if (LvP < LvR) {
                LvP     = LvR;
                pkUp    = up;
                lookDn  = true;
                mxCnt   = 0;
                mnCnt0  = 0;
                mnVal1  = base;
            }

            if (lookMn) {
                mnPrevPos   = i - mnCnt1;
                lookMn      = false;
                mnCnt1      = 0;
                mnVal1      = base;          
            }
        } else {
            LvR = LvR - LvR*kR;
            if (LvR < minR) { LvR = minR; }
        }

        if (mxCnt == minDist) {
            if (LvP > LvZ && !lookDn) {
                
            }
        }

        i++;
        return false;
	}
};
