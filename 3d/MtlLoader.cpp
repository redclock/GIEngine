#include "MtlLoader.h"
#include "linenumberreader.h"
#include <fstream>

using namespace std;
using namespace cgcad::common;

MtlLoader::MtlLoader(void)
{
    MtlMaterialInfo m;
    m.name = "";
    m.Kd = Point3(1, 1, 1);
    m.Ks = Point3(1, 1, 1);
    m.Ns = 0;
    m.texture = "";
    materials[""] = m;
}

MtlLoader::~MtlLoader(void)
{
}

bool MtlLoader::isEndType(TokenReader::TokenType type)
{
    return type == TokenReader::TOKEN_EOF 
        || type == TokenReader::TOKEN_EOS
        || type == TokenReader::TOKEN_ERROR;
}

bool MtlLoader::skipToNextLine()
{
    TokenReader::TokenType type;
    std::string c;
    type = getToken(&c);
    while (!isEndType(type) && c != "\n")
        type = getToken(&c);
    return type != TokenReader::TOKEN_ERROR;
}

TokenReader::TokenType MtlLoader::getToken(std::string* token)
{
    TokenReader::TokenType type;
    type = tokenReader_->read(token);
    // "\" ±íÊ¾ÕÛÐÐ
    while (!isEndType(type) && *token == "\\")
    {
        skipToNextLine();
        type = tokenReader_->read(token);
    }
    return type;
}

bool MtlLoader::appendFile(const char* filename, const char* extraPath)
{
    std::string fn = filename;
    std::locale::global(std::locale(""));
    ifstream f;
    f.open(fn.c_str());
    bool result = true;
    if (!f.good())
    {
        f.close();
        f.clear();
        fn = std::string(extraPath) + "/" + fn;
        f.open(fn.c_str());
    }

    if (!f.good())
    {
        result = false;
        printf("File can't open: %s.\n", fn.c_str()); 
    }
    else
    {
        printf("MTL file found: %s", fn.c_str());
        LineNumberReader reader(f, 4);
        tokenReader_ = new TokenReader(&reader, "\\/\n", " \t\r", "#", NULL);
        std::string cmd;
        TokenReader::TokenType type = getToken(&cmd);
        while (!isEndType(type))
        {
            //printf("%s\n", cmd.c_str());
            if (cmd == "newmtl")
            {
                if (!cmdNewMtl())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "Kd")
            {
                if (!cmdKd())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "Ks")
            {
                if (!cmdKs())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "Ns")
            {
                if (!cmdNs())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "map_Kd")
            {
                if (!cmdMapKd())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "\n")
            {
                // Skip
            }
            else
            {
                if (!skipToNextLine())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            if (type == TokenReader::TOKEN_ERROR)
                break;
            type = getToken(&cmd);
        }
        if (type == TokenReader::TOKEN_ERROR)
        {
            printf("Read error at Row %d Col %d.\n", 
                reader.getLineNumber(), reader.getColumnNumber());
        }
        else
        {
            if (curMat_.name != "")
            {
                materials[curMat_.name] = curMat_;
            }
        }
    }

    std::locale::global(std::locale("C")); 
    return result;
}

bool MtlLoader::contains(const char* name) const
{
    return materials.find(name) != materials.end();
}

bool MtlLoader::cmdNewMtl()
{
    TokenReader::TokenType type;
    std::string c;
    int pn = 0;
    type = getToken(&c);
    if (curMat_.name != "")
    {
        materials[curMat_.name] = curMat_;
    }
    curMat_.texture = "";
    if (type == TokenReader::TOKEN_NORMAL)
    {
        curMat_.name = c;
        printf("New material: %s.\n", c.c_str());
    }
    else
    {
        curMat_.name = "";
    }
    if (! isEndType(type) && c != "\n")
        skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool MtlLoader::cmdKd()
{
    TokenReader::TokenType type;
    std::string c;
    float r, g, b;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    r = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    g = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    b = (float)atof(c.c_str());

    curMat_.Kd = Point3(r, g, b);
    skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool MtlLoader::cmdKs()
{
    TokenReader::TokenType type;
    std::string c;
    float r, g, b;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    r = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    g = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    b = (float)atof(c.c_str());

    curMat_.Ks = Point3(r, g, b);
    skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool MtlLoader::cmdNs()
{
    TokenReader::TokenType type;
    std::string c;
    float r;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    r = (float)atof(c.c_str());

    curMat_.Ns = r;
    skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool MtlLoader::cmdMapKd()
{
    TokenReader::TokenType type;
    std::string c, res;
    int pn = 0;
    type = getToken(&c);
    while (!isEndType(type) && c != "\n")
    {
        res += c;
        type = getToken(&c);
    }

    curMat_.texture = res;
    printf("Texture: %s\n", res.c_str());
    return type != TokenReader::TOKEN_ERROR;
}