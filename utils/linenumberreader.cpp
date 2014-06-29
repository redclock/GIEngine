//
// THU-Render
//
// Copyright 2007-2008 CG & CAD Institute, Tsinghua University.
// All rights reserved.
//

#include "linenumberreader.h"

// C++ headers
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <cassert>

namespace cgcad
{

namespace common
{

LineNumberReader::~LineNumberReader(void)
{
}

LineNumberReader::LineNumberReader(std::ifstream& file, 
                                     size_t tabStop)
    : LineNumberAdapter((ParamDemarshalInputInterface*)1, tabStop),
    input_(file), tabStop_(tabStop), lineNo_(1), columnNo_(0),
    inReturn_(false), extraChar_(-1)

{
    assert(tabStop_ > 0);
}

int LineNumberReader::read(void)
{
    int result = extraChar_;
    extraChar_ = -1;
    if (result == -1)
        result = input_.get();

    if (result >= 0)
    {
        switch (result)
        {
        case '\r':
            columnNo_ = 0;
            ++lineNo_;
            inReturn_ = true;
            break;
        case '\n':
            if (inReturn_)
                break;
            columnNo_ = 0;
            ++lineNo_;
            inReturn_ = false;
            break;
        case '\t':
            columnNo_ /= tabStop_;
            ++columnNo_;
            columnNo_ *= tabStop_;
            inReturn_ = false;
            break;
        default:
            ++columnNo_;
            inReturn_ = false;
        }
    }
    return result;
}

int LineNumberReader::peek(void)
{
    if (extraChar_ == -1)
        return extraChar_ = input_.get();
    return extraChar_;
}

size_t LineNumberReader::getLineNumber(void) const
{
    return lineNo_;
}

size_t LineNumberReader::getColumnNumber(void) const
{
    return columnNo_;
}

} // namespace common

} // namespace cgcad
