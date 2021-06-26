/*
  ==============================================================================

    MathUtil.h
    Created: 4 Jun 2021 1:24:22pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <regex>
#define SEMITONE_RATIO 1.05946309436f
class MathUtil
{
public:
    static float fLerp(float lower, float upper, float t)
    {
        return lower + ((upper - lower) * t);
    }
    static double dLerp(double lower, double upper, double t)
    {
        return lower + ((upper - lower) * t);
    }
    static float hypot(float dX, float dY)
    {
        return std::sqrt(std::pow(dX, 2.0f) + std::pow(dY, 2.0f));
    }
    static void fft(int N, float *ar, float *ai)
    /*
     in-place complex fft
     
     After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
     
     program adapted from FORTRAN
     by K. Steiglitz  (ken@princeton.edu)
     Computer Science Dept.
     Princeton University 08544          */
    {
        int i, j, k, L;            /* indexes */
        int M, TEMP, LE, LE1, ip;  /* M = log N */
        int NV2, NM1;
        double t;               /* temp */
        float Ur, Ui, Wr, Wi, Tr, Ti;
        float Ur_old;
        // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
        NV2 = N >> 1;
        NM1 = N - 1;
        TEMP = N; /* get M = log N */
        M = 0;
        while (TEMP >>= 1)
            ++M;
        /* shuffle */
        j = 1;
        for (i = 1; i <= NM1; i++)
        {
            if(i<j)
            {             /* swap a[i] and a[j] */
                t = ar[j-1];
                ar[j-1] = ar[i-1];
                ar[i-1] = t;
                t = ai[j-1];
                ai[j-1] = ai[i-1];
                ai[i-1] = t;
            }
            k = NV2;             /* bit-reversed counter */
            while(k < j)
            {
                j -= k;
                k /= 2;
            }
            j += k;
        }
        LE = 1.0f;
        for (L = 1; L <= M; L++) {            // stage L
            LE1 = LE;                         // (LE1 = LE/2)
            LE *= 2;                          // (LE = 2^L)
            Ur = 1.0f;
            Ui = 0.0f;
            Wr = cos(M_PI/(float)LE1);
            Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
            for (j = 1; j <= LE1; j++)
            {
                for (i = j; i <= N; i += LE)
                { // butterfly
                    ip = i+LE1;
                    Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                    Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                    ar[ip-1] = ar[i-1] - Tr;
                    ai[ip-1] = ai[i-1] - Ti;
                    ar[i-1]  = ar[i-1] + Tr;
                    ai[i-1]  = ai[i-1] + Ti;
                }
                Ur_old = Ur;
                Ur = Ur_old * Wr - Ui * Wi;
                Ui = Ur_old * Wi + Ui * Wr;
            }
        }
    }
    
};

struct stdu
{
   template <typename T>
    static T loadIfLockFree(std::atomic<T>& input)
    {
        if(input.is_lock_free())
            return input.load();
        return 0;
    }
    static std::vector<std::string> matchesAsVector(std::string body, std::regex reg)
    {
        std::vector<std::string> strings;
        auto results = std::smatch{};
        for (std::sregex_iterator it = std::sregex_iterator(body.begin(), body.end(), reg);
                 it != std::sregex_iterator(); it++)
        {
            std::smatch match;
            match = *it;
            auto str = match.str(0);
            strings.push_back(str);
        }
        return strings;
    }
    static std::pair<int, int> stringAsFraction(std::string input)
    {
        auto exp = std::regex("\\w+");
        auto values = matchesAsVector(input, exp);
        auto num = std::stoi(values[0]);
        auto denom = std::stoi(values[1]);
        return std::make_pair(num, denom);
    }
};
