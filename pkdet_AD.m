function [ pk, trends, labels ] = pkdet_AD(tacho, base, minDist, Fs, mm_lsb)
% ! только локальные максимумы с разрядом уровня
% структура: [индекс, амплитуда]
if nargin < 5
    mm_lsb = 1000;
end
if nargin<3
    minAmp = 0;
end
minDist = fix(minDist);
LvP = -Inf;
mxCnt = 0;
N=length(tacho);
extr = false(N,1);
type = zeros(N,1);
trends = {type, type, type};
labels = type;

lookMn = false;
mnCnt0 = 1; mnVal0 = base(1);
mnCnt1 = 1; mnVal1 = base(1);
mnPrevPos = 1;

minR = 0.1 * mm_lsb;
LvZ = 1 * mm_lsb; % средний уровень пика
LvR = tacho(1); % уровень слежения-разряда
kR = 1/400*(1000/Fs); % 300 % 15.02.2018


% LvN = 100;
% kN = 1/300;
% kaN = 2.5;


kZ = 0.2;
% LvP = LvZ/coefZ;
up=1; dn=1; modeZero = true;
pkUp = 1; pkDn = 1;
lookDn = false;
pk1 = struct;
pk = [];

for i=1:N
    
%     err = abs(tacho(i)) - LvN;
%     if err > LvN, err = LvN; end % мин-макс динамика
%     if err < -LvN, err = -LvN; end
%     LvN = LvN + kN*err;
    
    if mnVal0 >= base(i), mnVal0 = base(i); mnCnt0 = 0; end
    if mnVal1 >= base(i), mnVal1 = base(i); mnCnt1 = 0; end
    
    if modeZero
        if tacho(i) > LvZ*0.1
            up = i;
            modeZero = false;
        end
    else
        if tacho(i) < LvZ*0.1
            dn = i;
            modeZero = true;
            if lookDn, pkDn = dn; lookDn = false; end
        end
    end
    
    if tacho(i) > LvR % поиск максимума
        LvR = tacho(i);
        if LvP < LvR
            LvP = LvR;
            pkUp = up; lookDn = true;
            mxCnt = 0;
            mnCnt0 = 0; mnVal0 = base(i); % сброс поиска минимума
        end
        if lookMn
            mnPrevPos = i-mnCnt1; % mnPrevPos = mnCnt;
            lookMn = false;
            mnCnt1 = 0; mnVal1 = base(i); % сброс поиска минимума
        end
	else % медленный разряд
        LvR = LvR - LvR*kR;
        if LvR < minR, LvR = minR; end
    end
    
    trends{3}(i) = LvZ;
//-------------------------------------------------------------------------------------------------- 
    if mxCnt == minDist
        if LvP > LvZ && ~lookDn % если пик выше порога по амплитуде и была детекция вниз
            pk1.iup = pkUp;
            pk1.imx = i-mxCnt;
            pk1.idn = pkDn;
            pk1.iPrevMin = mnPrevPos;
            pk = [pk; pk1];
            
            mnCnt1 = mnCnt0;
            mnVal1 = mnVal0;
            
            lookMn = true;
        end
        
        err = (LvP/2 - LvZ);
        if err > LvZ, err = LvZ; end % мин-макс динамика
        if err < -LvZ, err = -LvZ; end
        if err > 0, dZ = err*kZ; % разная динамика вверх-вниз
        else, dZ = err*kZ*4; end
        
        LvZ = LvZ + dZ;
%         trends{3}(i) = LvP;
        LvP = 0; %x(i); %сброс
    end
    
    trends{1}(i) = LvR;
%     trends{2}(i) = LvZ;
    mnCnt0 = mnCnt0+1;
    mnCnt1 = mnCnt1+1;
    mxCnt = mxCnt+1;
end

trends{2} = extr;
% max.pos = find(extr(:,1) > 0);
% max.val = extr(max.pos);
% pk.pos = find(extr);
% pk.val = type(extr);
