#include "ConfigFile.h"
#include <fstream>
#include <ctype.h>
#include <sstream>

ConfigFile::ConfigFile(void)
{
}

ConfigFile::~ConfigFile(void)
{
}

static std::string trim(std::string& s, const std::string& drop = " \t\f\v\n\r")
{
    // trim right
    s.erase(s.find_last_not_of(drop)+1);
    // trim left
    return s.erase(0,s.find_first_not_of(drop));
}

static bool isInt(const std::string& s)
{
    if (s.length() == 0)
        return false;
    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] != '-' && s[i] != '+' 
            && !isspace(s[i]) && !isdigit(s[i]))
            return false;
    }
    return true;
}

template <class T>
bool fromString(T &t, const std::string &s)
{
    std::istringstream iss(s);
    return !(iss >> t).fail();
}

template <class T>
std::string toString(T &t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

static bool isFloat(const std::string& s)
{
    if (s.length() == 0)
        return false;
    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] != '-' && s[i] != '+' 
            && s[i] != '.' && s[i] != 'e' 
            && !isspace(s[i]) && !isdigit(s[i]))
            return false;
    }
    return true;
}


bool ConfigFile::Load(const char* filename)
{
    std::ifstream fin(filename);
    if (!fin.good())
        return false;
    m_options.clear();
    char line[2000];
    while (!fin.eof())
    {
        fin.getline(line, 2000);
        std::string l = line;
        size_t i = l.find('=');
        if (i == std::string.npos)
            continue;
        std::string key = trim(l.substr(0, i));
        std::string value = trim(l.substr(i + 1, l.length()));
        m_options[key] = value;
    }
    FileName = filename;
    return true;
}

bool ConfigFile::Save(const char* filename) const
{
    std::ofstream fout(filename);
    if (!fout.good())
        return false;
    for (std::map<std::string, std::string>::const_iterator itr = m_options.begin();
        itr != m_options.end(); itr++)
    {
        fout << itr->first << "=" << itr->second << std::endl;
    }
    return true;
}

std::string ConfigFile::Get(const std::string& key, 
                            const std::string& defaultValue) const
{
    std::map<std::string, std::string>::const_iterator itr 
        = m_options.find(key);
    if (itr == m_options.end())
        return defaultValue;
    else
    {
        return itr->second;
    }
}

void ConfigFile::Set(const std::string& key, const std::string& value)
{
    m_options[key] = value;
}

void ConfigFile::Set(const std::string& key, int value)
{
    m_options[key] = toString(value);
}

void ConfigFile::Set(const std::string& key, float value)
{
    m_options[key] = toString(value);
}

void ConfigFile::Set(const std::string& key, const Point3& value)
{
    m_options[key + ".x"] = toString(value.x);
    m_options[key + ".y"] = toString(value.y);
    m_options[key + ".z"] = toString(value.z);
}
void ConfigFile::Clear()
{
    m_options.clear();
}

int ConfigFile::GetInt(const std::string& key, int defaultValue) const
{
    std::string s = Get(key);
    int result;
    if (!fromString(result, s))
        result = defaultValue;
    return result;
}

float ConfigFile::GetFloat(const std::string& key, float defaultValue) const
{
    std::string s = Get(key);
    float result;
    if (!fromString(result, s))
        result = defaultValue;
    return result;
}

Point3 ConfigFile::GetPoint(const std::string& key, const Point3& defaultValue) const
{
    Point3 p = Point3(
        GetFloat(key + ".x", defaultValue.x),
        GetFloat(key + ".y", defaultValue.y),
        GetFloat(key + ".z", defaultValue.z)
        );
    return p;
}