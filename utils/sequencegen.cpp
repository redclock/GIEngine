//
// seqencegen.cpp
// Last Updated:		05.01.07
// 
// Mark Colbert & Jaroslav Krivanek
// colbert@cs.ucf.edu
//
// Copyright (c) 2007.
//
// The following code is freely distributed "as is" and comes with 
// no guarantees or required support by the authors.  Any use of 
// the code for commercial purposes requires explicit written consent 
// by the authors.
//

// The following uses the PDSampler code based provided by Daniel Dunbar
// and the radiacal inverse code by Matt Pharr and Greg Humphreys 
// to generate a variety of different quasi-random number sequences.

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "sequencegen.h"

using namespace std;

static const float minrnd=0.000001f;
static const float maxrnd=1-minrnd;

// Inversion code courtesy of Matt Pharr and Greg Humphries
inline double FoldedRadicalInverse(int n, int base) {
	double val = 0;
	double invBase = 1.f/base, invBi = invBase;
	int modOffset = 0;
	while (val + base * invBi != val) {
		// Compute next digit of folded radical inverse
		int digit = ((n+modOffset) % base);
		val += digit * invBi;
		n /= base;
		invBi *= invBase;
		++modOffset;
	}
	return val;
}

inline double RadicalInverse(int n, int base) {
	double val = 0;
	double invBase = 1. / base, invBi = invBase;
	while (n > 0) {
		// Compute next digit of radical inverse
		int d_i = (n % base);
		val += d_i * invBi;
		n /= base;
		invBi *= invBase;
	}
	return val;
}



CSequenceGenerator::CSequenceGenerator()
{
    randnum = NULL;
    randcos = NULL;
    randsin = NULL;
    randlog = NULL;
    sampleCount = 0;
}

CSequenceGenerator::~CSequenceGenerator()
{
    clearBuffers();
}

void CSequenceGenerator::clearBuffers()
{
    delete [] randnum;
    randnum = NULL;
    randcos = NULL;
    randsin = NULL;
    randlog = NULL;
}

void CSequenceGenerator::allocBuffers(int count)
{
    randnum = new float[count * 4];
    randcos = &randnum[count];
    randsin = &randcos[count];
    randlog = &randsin[count];
    sampleCount = count;
}

void CSequenceGenerator::genHaltonSequence(int n)
{
    for (int i=0; i < n; i++) {
        randnum[i] = (float) RadicalInverse(i,2);
        if(randnum[i]<minrnd) randnum[i]=minrnd;
        if(randnum[i]>maxrnd) randnum[i]=maxrnd;
        randlog[i] = logf(randnum[i]);

        double e2 = 2.*M_PI*RadicalInverse(i,3);
        randcos[i] = (float) cos(e2);
        randsin[i] = (float) sin(e2);		
    }
}

void CSequenceGenerator::genFoldedHaltonSequence(int n)
{
    for (int i=0; i < n; i++) {
        randnum[i] = (float) FoldedRadicalInverse(i,2);
        if(randnum[i]<minrnd) randnum[i]=minrnd;
        if(randnum[i]>maxrnd) randnum[i]=maxrnd;
        randlog[i] = logf(randnum[i]);

        double e2 = 2.*M_PI*FoldedRadicalInverse(i,3);
        randcos[i] = (float) cos(e2);
        randsin[i] = (float) sin(e2);		
    }
}

void CSequenceGenerator::genHammersleySequence(int n)
{
    for (int i=0; i < n; i++) {
        randnum[i] = ((float) i)/((float) n);
        if(randnum[i]<minrnd) randnum[i]=minrnd;
        if(randnum[i]>maxrnd) randnum[i]=maxrnd;
        randlog[i] = logf(randnum[i]);

        double e2 = 2.*M_PI*RadicalInverse(i,2);
        randcos[i] = (float) cos(e2);
        randsin[i] = (float) sin(e2);		
    }

}

void CSequenceGenerator::genFoldedHammersleySequence(int n)
{
    for (int i=0; i < n; i++) {
        randnum[i] = ((float) i)/((float) n);
        if(randnum[i]<minrnd) randnum[i]=minrnd;
        if(randnum[i]>maxrnd) randnum[i]=maxrnd;
        randlog[i] = logf(randnum[i]);

        double e2 = 2.*M_PI*FoldedRadicalInverse(i,2);
        randcos[i] = (float) cos(e2);
        randsin[i] = (float) sin(e2);		
    }
}

void CSequenceGenerator::genSequence(int n, SequenceType type)
{
    if (randnum)
        clearBuffers();
    allocBuffers(n);
    switch (type) 
    {
        case HALTON:			genHaltonSequence(n);			break;
        case FOLDED_HALTON:		genFoldedHaltonSequence(n);		break;
        case HAMMERSLEY:		genHammersleySequence(n);		break;
        case FOLDED_HAMMERSLEY:	genFoldedHammersleySequence(n);	break;
    }
}