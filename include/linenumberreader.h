//
// THU-Render
//
// Copyright 2007-2008 CG & CAD Institute, Tsinghua University.
// All rights reserved.
//

#ifndef __CGCAD_COMMON_LINE_NUMBER_READER_APPEARED__
#define __CGCAD_COMMON_LINE_NUMBER_READER_APPEARED__

#include <cstddef>
#include <fstream>
#include "linenumberadapter.h"

namespace cgcad
{

namespace common
{

/**
 * Reads the marshalled data, gets the line number and column number
 * for the data just read.
 */
class LineNumberReader : public LineNumberAdapter
{
// public constructors & destructor
public:
    /**
     * Initialize the @c LineNumberAdapter to encapsulate
     * the @c ParamDemarshalInputInterface.
     * @param input user defined @c ParamDemarshalInputInterface
     *     instance. If passes NULL, all the functions will
     *     occur an error.
     * @param tabStop user defined TAB size. If passes a negative
     *     number, all the functions will occur an error.
     */
    LineNumberReader(std::ifstream& file, size_t tabStop);
    virtual ~LineNumberReader(void);

// public member functions
public:
    virtual int read(void);
    virtual int peek(void);
    /**
     * Gets the line number of the character just read.
     * The line number starts with 1.
     * @return the line number of the character just read.
     */
    size_t getLineNumber(void) const;
    /**
     * Gets the column number of the character just read.
     * The column number starts with 0.
     * @return the column number of thet character just read.
     */
    size_t getColumnNumber(void) const;

// private data members
private:
    std::ifstream& input_;
    size_t tabStop_;
    size_t lineNo_;
    size_t columnNo_;
    bool inReturn_;
    int extraChar_;

};

} // namespace common

} // namespace cgcad

#endif // __CGCAD_COMMON_LINE_NUMBER_READER_APPEARED__
