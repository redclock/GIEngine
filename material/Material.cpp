#include <Windows.h>
#include "Material.h"
#include "textparamdemarshal.h"
#include "gettextmanager.h"
#include "paramarrayinterface.h"
#include "paramvalueinterface.h"
#include "pathutil.h"
#include "scopedpointer.h"
#include "pathmanager.h"
#include "fileinput.h"
#include "paramarrayconstenumeratorinterface.h"
#include "paramobjectextractor.h"
#include "paramarrayextractor.h"

CMaterialList::CMaterialList()
{

}

CMaterialList::~CMaterialList()
{
    Clear();
}

void CMaterialList::Add(const CMaterialList::Item& item)
{
    Item* it = new Item;
    it->name = item.name;
    it->type = item.type;
    it->r = item.r;
    it->g = item.g;
    it->b = item.b;
    it->texture = item.texture;
    m_items.push_back(it);
}

void CMaterialList::Clear()
{
    for (size_t i = 0; i < m_items.size(); i++)
    {
        delete m_items[i];
    }
    m_items.clear();
}

int CMaterialList::FindIndex(const char* name) const
{
    for (size_t i = 0; i < m_items.size(); i++)
    {
        if (m_items[i]->name == name)
            return (int) i;
    }
    return -1;

}

CMaterialList::Item* CMaterialList::Find(const char* name) const
{
    int i = FindIndex(name);
    if (i >= 0)
        return m_items[i];
    else
        return NULL;
}

void CMaterialList::Remove(size_t index)
{
    delete m_items[index];
    m_items.erase(m_items.begin() + index);
}

void CMaterialList::Remove(const char* name)
{
    int i = FindIndex(name);
    if (i >= 0)
    {
        Remove(i);
    }
}

bool CMaterialList::LoadFromTHU( const char* filename )
{
    using namespace cgcad::common;
    cgcad::host::FileInput fileIn;
    if (!fileIn.initialize(filename))
    {
        printf("Failed to open input file %s\n", filename);
        return false;
    }

    printf("Parsing scene description file...\n ");
    TextParamDemarshal demarshal(NULL, size_t(8U), 100U);
    const ParamArrayInterface* paramsArray;
    if (!demarshal.demarshal(&fileIn)
        || (paramsArray = demarshal.getData()->getArray()) == NULL)
    {
        printf("%s\n", "\nFailed to parse script.");
        // print error messages
        for (size_t i = 0; i < demarshal.getErrorCount(); ++i)
        {
            if (demarshal.getErrorType(i)
                == TextParamDemarshal::ERROR_TYPE_ERROR)
            {
                printf("=== Error at line %d, column %d: %s\n", 
                    demarshal.getErrorRow(i), demarshal.getErrorColumn(i),
                    demarshal.getErrorText(i));
            }
        }
        return false;
    }
    // extracts elements in the order they appear in script file
    ParamArrayConstEnumeratorInterface* paramsEnum;
    for (paramsEnum = paramsArray->getEnumerator();
        !paramsEnum->atEnd();
        paramsEnum->moveNext())
    {
        const ParamObjectInterface* params =
            paramsEnum->getValue()->getObject();
        if (params == NULL)
        {
            printf("Enumerate error\n");
        }
        else
        {
            ParamObjectExtractor paramsE(params);
            std::string name;
            std::string className;
            paramsE.getString("name", &name);
            paramsE.getString("classname", &className);
            const ParamObjectInterface* texP = paramsE.getObject("colortexture");

            ParamObjectExtractor colorTextureE(texP);
            const ParamArrayInterface* colorArray = colorTextureE.getArray("color");
            double r = 1, g = 1, b = 1;
            if (colorArray != NULL)
            {
                ParamArrayExtractor colorArrayE(colorArray);
                colorArrayE.getReal(0, &r);
                colorArrayE.getReal(1, &g);
                colorArrayE.getReal(2, &b);
            }
            std::string texture;
            if (!colorTextureE.getString("filename", &texture))
                texture = "";
            //if (texture != "")
            //    printf("Name = %s, texture = %s color = [%g %g %g]\n", 
            //    name.c_str(), texture.c_str(),
            //    r, g, b);
            //BYTE r1 = (BYTE) (max(0, min(1, r)) * 255);
            //BYTE g1 = (BYTE) (max(0, min(1, g)) * 255);
            //BYTE b1 = (BYTE) (max(0, min(1, b)) * 255);
            BYTE mat = MAT_DIFFUSE;
            if (className == "metal")
            {
                mat = MAT_METAL;
            }
            if (className == "plastic")
            {
                mat = MAT_PLASTIC;
            }
            else if (className == "source")
            {
                mat = MAT_SOURCE;
            }
            else if (className == "glass")
            {
                mat = MAT_GLASS;
            }
            else if (className == "mirror")
            {
                mat = MAT_MIRROR;
            }

            Item* item = new Item;
            item->name = name;
            item->type = (MaterialType) mat;
            item->r = (float)r;
            item->g = (float)g;
            item->b = (float)b;
            item->texture = texture;
            m_items.push_back(item);
        }
    }

    paramsEnum->destroy();

    return true;
}

