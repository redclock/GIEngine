#pragma once

#include "tokenreader.h"
#include <vector>
#include "Point3.h"
#include "MtlLoader.h"

using namespace cgcad::common;

class ObjLoader
{
protected:
    struct FaceTriple
    {
        int v, vt, vn;
    };
protected:
    std::string filepath;
    TokenReader* tokenReader_;
    std::vector<Point3*> vertices_;
    std::vector<Point3*> normals_;
    std::vector<Point3*> texcoords_;
    std::vector<FaceTriple> faceTriples_;

    std::vector<FaceTriple> vbuf_;
    std::vector<int> fbuf_;

    bool hasNormal_;
    bool hasTexcoord_;
    int minVertex_;
    int maxVertex_;
    std::string name_;
    std::string mtlname_;
    MtlLoader mtlLoader_;

private:
    static bool isEndType(TokenReader::TokenType type);
    bool skipToNextLine();
    TokenReader::TokenType getToken(std::string* token);
    void clear();

    void beginMesh();
    void endMesh();

    bool cmdV();
    bool cmdVt();
    bool cmdVn();
    bool cmdF();
    bool cmdF_one(TokenReader::TokenType& type, std::string& c);
    bool cmdG();

    bool cmdMtllib();
    bool cmdUsemtl();

protected:
    virtual void newMesh();

public:
    bool loadFromFile(const char* filename);

    ObjLoader(void);
    virtual ~ObjLoader(void);
};
