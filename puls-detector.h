#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

// #define INF 999999

struct puls {
    int32_t     iup;
    int32_t     imx;
    int32_t     idn;
    int32_t     iPrevMin;


    puls() {
        iup         = 0;
        imx         = 0;
        idn         = 0;
        iPrevMin    = 0;
    }
};

class PulsDetector {
public:
    int32_t     i;
    int32_t     minDist;

    int32_t     LvP;
    int32_t     mxCnt;

    bool        lookMn;
    int32_t     mnCnt0, mnCnt1;
    int32_t     mnVal0, mnVal1;
    int32_t     mnPrevPos;

    float       minR;
    int32_t     LvZ, LvR;
    int32_t     kR;

    float       kZ;
    bool        modeZero;
    int32_t     up, pkUp;
    int32_t     dn, pkDn;
    bool        lookDn;

    //struct pk1
    puls        pk;

	PulsDetector(int32_t Fs, int32_t MinDistance) {
		pk          = puls();
        minDist     = MinDistance;
        LvP         = INT32_MIN;
        mxCnt       = 0;

        lookMn      = false;
        mnCnt0      = 1;
        mnCnt1      = 1;
        mnPrevPos   = 0;

        minR        = 0.1; //*mmlsb
        LvZ         = 1;   //*mmlsb
        kR          = 1/400*(1000/Fs);

        kZ          = 0.2;
        up          = 1;
        dn          = 1;
        modeZero    = true;
        pkUp        = 1;
        pkDn        = 1;
        lookDn      = false;
	}

    bool exe(int tacho, int base) {
        bool res = false;

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
                pk.iup         = pkUp;
                pk.imx         = i - mxCnt;
                pk.idn         = pkDn;
                pk.iPrevMin    = mnPrevPos;

                mnCnt1 = mnCnt0;
                mnVal1 = mnVal0;
            
                lookMn  = true;
                res     = true;
            }

            int32_t dZ;
            int32_t err = LvP/2 - LvZ;
            if (err >  LvZ) { err =  LvZ; }
            if (err < -LvZ) { err = -LvZ; }
            if (err > 0)    { dZ  = err * kZ; }
            else            { dZ  = err * kZ * 4; }

            LvZ = LvZ + dZ;
            LvP = 0;
        }

        i++;
        return res;
	}
};
