#pragma once
#include "tokenreader.h"
#include <vector>
#include "Point3.h"
#include <map>

using namespace cgcad::common;

struct MtlMaterialInfo
{
    std::string name;
    std::string texture;
    Point3 Kd;
    Point3 Ks;
    float Ns;
};

typedef std::map<std::string, MtlMaterialInfo> MtlMap;

class MtlLoader
{
private:
    TokenReader* tokenReader_;

    static bool isEndType(TokenReader::TokenType type);
    bool skipToNextLine();
    TokenReader::TokenType getToken(std::string* token);
    
    bool cmdNewMtl();
    bool cmdKd();
    bool cmdKs();
    bool cmdMapKd();
    bool cmdNs();

    MtlMaterialInfo curMat_;
public:
    MtlMap materials;

    MtlLoader(void);
    ~MtlLoader(void);

    bool appendFile(const char* filename, const char* extraPath);
    bool contains(const char* name) const;
};
