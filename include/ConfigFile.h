#pragma once
#include <map>
#include <string>
#include "Point3.h"

class ConfigFile
{
private:
    std::map<std::string, std::string> m_options;
public:
    std::string FileName;
    bool Load(const char* filename);
    bool Save(const char* filename) const;

    std::string Get(const std::string& key, const std::string& defaultValue = "") const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    Point3 GetPoint(const std::string& key, const Point3& defaultValue = Point3(0, 0, 0)) const;
    
    void Set(const std::string& key, const std::string& value);
    void Set(const std::string& key, int value);
    void Set(const std::string& key, float value);
    void Set(const std::string& key, const Point3& value);
    
    void Clear();
    
    ConfigFile(void);
    ~ConfigFile(void);
};
