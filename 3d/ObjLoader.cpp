#include "ObjLoader.h"
#include <fstream>
#include <locale>
#include <string>
#include <d3dx9.h>
#include "linenumberreader.h"
#include "ObjFaceMap.h"
#include "MyUtils.h"

using namespace std;

ObjLoader::ObjLoader(void) : tokenReader_(NULL),
    hasNormal_(false), hasTexcoord_(false),
    minVertex_(-1), maxVertex_(-1), name_("")
{
}

ObjLoader::~ObjLoader(void)
{
    if (tokenReader_)
        delete tokenReader_;
    clear();
}

bool ObjLoader::isEndType(TokenReader::TokenType type)
{
    return type == TokenReader::TOKEN_EOF 
        || type == TokenReader::TOKEN_EOS
        || type == TokenReader::TOKEN_ERROR;
}


void ObjLoader::clear()
{
    for (size_t i = 0; i < vertices_.size(); ++i)
        delete vertices_[i];
    vertices_.clear();
    for (size_t i = 0; i < normals_.size(); ++i)
        delete normals_[i];
    normals_.clear();
    for (size_t i = 0; i < texcoords_.size(); ++i)
        delete texcoords_[i];
    texcoords_.clear();

}

bool ObjLoader::loadFromFile(const char* filename)
{
    clear();
    std::locale::global(std::locale(""));
    ifstream f;
    f.open(filename);
    bool result = true;
    if (!f.good())
    {
        f.close();
        result = false;
        printf("File can't open: %s.\n", filename); 
    }
    else
    {
        filepath = getFilePath(filename);
        LineNumberReader reader(f, 4);
        tokenReader_ = new TokenReader(&reader, "\\/\n", " \t\r", "#", NULL);
        std::string cmd;
        TokenReader::TokenType type = getToken(&cmd);
        while (!isEndType(type))
        {
            //printf("%s\n", cmd.c_str());
            if (cmd == "v")
            {
                if (!cmdV())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "vt")
            {
                if (!cmdVt())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "vn")
            {
                if (!cmdVn())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "f")
            {
                if (!cmdF())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "g")
            {
                if (!cmdG())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "mtllib")
            {
                if (!cmdMtllib())
                {
                    type = TokenReader::TOKEN_ERROR;
                }
            }
            else if (cmd == "usemtl")
            {
                if (!cmdUsemtl())
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
    }
    printf("Vertex# %d, Normal# %d, Texcoord# %d\n", 
        vertices_.size(), normals_.size(), texcoords_.size());
    std::locale::global(std::locale("C")); 
    return result;
}

bool ObjLoader::skipToNextLine()
{
    TokenReader::TokenType type;
    std::string c;
    type = getToken(&c);
    while (!isEndType(type) && c != "\n")
        type = getToken(&c);
    return type != TokenReader::TOKEN_ERROR;
}


TokenReader::TokenType ObjLoader::getToken(std::string* token)
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

bool ObjLoader::cmdV()
{
    TokenReader::TokenType type;
    std::string c;
    float x, y, z;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    x = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    y = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    z = (float)atof(c.c_str());

    Point3* p = new Point3(x, y, z);
    skipToNextLine();
    vertices_.push_back(p);
    return type != TokenReader::TOKEN_ERROR;
}

bool ObjLoader::cmdVt()
{
    TokenReader::TokenType type;
    std::string c;
    float x, y, z;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    x = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        y = 0;
    else
        y = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        z = 0;
    else
        z = (float)atof(c.c_str());

    Point3* p = new Point3(x, y, z);
    skipToNextLine();
    texcoords_.push_back(p);
    return type != TokenReader::TOKEN_ERROR;
}

bool ObjLoader::cmdVn()
{
    TokenReader::TokenType type;
    std::string c;
    float x, y, z;

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    x = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    y = (float)atof(c.c_str());

    type = getToken(&c);
    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    z = (float)atof(c.c_str());

    Point3* p = new Point3(x, y, z);
    *p = p->Normalize();
    skipToNextLine();
    normals_.push_back(p);
    return type != TokenReader::TOKEN_ERROR;
}

bool ObjLoader::cmdF()
{
    TokenReader::TokenType type;
    std::string c;
    int pn = 0;
    type = getToken(&c);
    FaceTriple head;
    while (! isEndType(type) && c != "\n")
    {
        if (pn >= 3)
        {
            faceTriples_.push_back(head);
            faceTriples_.push_back(faceTriples_[faceTriples_.size() - 2]);
        }
        if (!cmdF_one(type, c))
            return false;
        if (pn == 0)
            head = faceTriples_[faceTriples_.size() - 1];
        pn++;
    }
    //printf("face : %d\n", pn);
    if (pn < 3)
        return false;
    if (! isEndType(type) && c != "\n")
        skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool ObjLoader::cmdF_one(TokenReader::TokenType& type, std::string& c)
{
    int v = 0, vt = 0, vn = 0;

    if (type != TokenReader::TOKEN_NORMAL || c == "\n")
        return false;
    v = atoi(c.c_str());
    if (maxVertex_ == -1 || v > maxVertex_)
        maxVertex_ = v;
    if (minVertex_ == -1 || v < minVertex_)
        minVertex_ = v;
    type = getToken(&c);
    if (isEndType(type) || c == "\n")
        return true;
    if (c == "/")
    {
        type = getToken(&c);
        if (c != "/")
        {
            if (type != TokenReader::TOKEN_NORMAL || c == "\n")
                return false;
            vt = atoi(c.c_str());
            hasTexcoord_ = true;
            type = getToken(&c);
        }
        if (c == "/")
        {
            type = getToken(&c);
            if (type != TokenReader::TOKEN_NORMAL || c == "\n")
                return false;
            vn = atoi(c.c_str());
            hasNormal_ = true;
            type = getToken(&c);
        }
    }
    FaceTriple t = {v, vt, vn};
    faceTriples_.push_back(t);
    return true;
}

bool ObjLoader::cmdG()
{
    TokenReader::TokenType type;
    std::string c;
    int pn = 0;
    type = getToken(&c);
    endMesh();
    if (type == TokenReader::TOKEN_NORMAL)
    {
        name_ = c;
        beginMesh();
    }
    else
    {
        name_ = "";
    }
    if (! isEndType(type) && c != "\n")
        skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

void ObjLoader::beginMesh()
{
    hasNormal_ = false;
    hasTexcoord_ = false;
    minVertex_ = -1;
    maxVertex_ = -1;
    //printf("Begin object: %s %s\n", name_.c_str(), mtlname_.c_str());
}

void ObjLoader::endMesh()
{
    if (faceTriples_.size() == 0)
        return;
    printf("New object: %s * %s\n", name_.c_str(), mtlname_.c_str());
    ObjFaceMap faceMap(minVertex_, maxVertex_);
    int index = 0;
    for (size_t i = 0; i < faceTriples_.size(); ++i)
    {
        int id = faceMap.lookup(faceTriples_[i].v, faceTriples_[i].vt, faceTriples_[i].vn);
        if (id < 0)
        {
            faceMap.insert(faceTriples_[i].v, 
                faceTriples_[i].vt, faceTriples_[i].vn, index);
            vbuf_.push_back(faceTriples_[i]);
            id = index;
            ++index;
        }
        fbuf_.push_back(id);
    }
    newMesh();
    faceTriples_.clear();
    vbuf_.clear();
    fbuf_.clear();
}

void ObjLoader::newMesh()
{

}

bool ObjLoader::cmdMtllib()
{
    TokenReader::TokenType type;
    std::string pathname;
    std::string c;
    int pn = 0;
    type = getToken(&c);
    while (type == TokenReader::TOKEN_NORMAL || c == "/" || c == "\\")
    {
        pathname += c;
        type = getToken(&c);
    }
    mtlLoader_.appendFile(pathname.c_str(), filepath.c_str());
    if (! isEndType(type) && c != "\n")
        skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;
}

bool ObjLoader::cmdUsemtl()
{
    TokenReader::TokenType type;
    std::string c;
    int pn = 0;
    type = getToken(&c);
    endMesh();
    if (type == TokenReader::TOKEN_NORMAL)
    {
        mtlname_ = c;
        beginMesh();
    }
    else
    {
        mtlname_ = "";
    }
    if (! isEndType(type) && c != "\n")
        skipToNextLine();
    return type != TokenReader::TOKEN_ERROR;

}