#include "LuaScriptWrapper.h"

LuaObject LuaScriptWrapper::LuaMatrixTranslate(float x, float y, float z)
{
    D3DXMATRIX mat;
    D3DXMatrixTranslation(&mat, x, y, z);
    return MatrixToLua(mat);
}


LuaObject LuaScriptWrapper::LuaMatrixScale(float x, float y, float z)
{
    D3DXMATRIX mat;
    D3DXMatrixScaling(&mat, x, y, z);
    return MatrixToLua(mat);
}

LuaObject LuaScriptWrapper::LuaMatrixRotate(LuaObject flag, float angle)
{
    D3DXMATRIX mat;
    if (flag.IsString())
    {
        if (*flag.GetString() == 'x')
            D3DXMatrixRotationX(&mat, angle);
        else if (*flag.GetString() == 'y')
            D3DXMatrixRotationY(&mat, angle);
        else if (*flag.GetString() == 'z')
            D3DXMatrixRotationZ(&mat, angle);
    }
    else if (flag.IsTable())
    {
        Point3 p = LuaToPoint3(flag);
        D3DXVECTOR3 axis(p.x, p.y, p.z);
        D3DXMatrixRotationAxis(&mat, &axis, angle);
    }
    return MatrixToLua(mat);
}

LuaObject LuaScriptWrapper::LuaMatrixMultiply(LuaObject mat1, LuaObject mat2)
{
    D3DXMATRIX m1 = LuaToMatrix(mat1);
    D3DXMATRIX m2 = LuaToMatrix(mat2);
    D3DXMATRIX m3 = m1 * m2;
    return MatrixToLua(m3);
}