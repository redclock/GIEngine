//
// seqencegen.h
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

// Modified by yaoch


#pragma once

enum SequenceType 
{
    HALTON = 0, FOLDED_HALTON = 1, HAMMERSLEY = 2, FOLDED_HAMMERSLEY = 3, 
    POSSION_DISK = 4, BEST_CANDIDATE = 5, PENROSE = 6
};

class CSequenceGenerator
{
public:
    float *randnum, *randcos, *randsin, *randlog;
    int sampleCount;

private:
    void clearBuffers();
    void allocBuffers(int count);
    void genHaltonSequence(int n);
    void genFoldedHaltonSequence(int n);
    void genHammersleySequence(int n);
    void genFoldedHammersleySequence(int n);

public:
    CSequenceGenerator();
    ~CSequenceGenerator();
        
    /// Generates a quasi-random number sequence
    void genSequence(int n, SequenceType type);
};

