/************************************************************************/
/* Utilities			                                                */
/* 二维数组			       编写： 姚春晖                                */
/************************************************************************/

#pragma once
#include <assert.h>

//T为数组元素类型
template<class T>
class Array2D
{
private:
	//Buffer
	T* m_buf;
	int m_rows, m_cols;
	//Buf是否是外部管理
	bool m_attached;
public:
	int GetRows() const { return m_rows; };
	int GetCols() const { return m_cols; };

    //构造默认数组1x1
    Array2D(): m_rows(1), m_cols(1), m_attached(false)
    {
        m_buf = new T[m_rows * m_cols];
    }

    //构造数组
	Array2D(int row, int col): m_rows(row), m_cols(col), m_attached(false)
	{
		m_buf = new T[m_rows * m_cols];
	}

	//拷贝构造函数
	Array2D(const Array2D & arr): m_rows(arr.m_rows), m_cols(arr.m_cols), m_attached(arr.m_attached)
	{
		if (arr.m_attached)
		{
			m_buf = arr.m_buf;
		}else
		{
			m_buf = new T[m_rows * m_cols];
			memcpy(m_buf, arr.m_buf, sizeof(T) * m_cols * m_rows);
		}
		
	}

	//包装外部Buffer
	Array2D(int row, int col, T * buf): m_rows(row), m_cols(col), m_attached(true)
	{
		m_buf = buf;
	}

	~Array2D()
	{
		if (!m_attached)
			delete[] m_buf;
	}

	void ZeroArray()
	{
		memset(m_buf, 0, sizeof(T) * m_cols * m_rows);
	}

	//必须不能为包装的Buffer
	void Resize(int row, int col)
	{
		assert(!m_attached);
		int minr = min(row, m_rows);
		int minc = min(col, m_cols);
		T * temp = new T [(row) * (col)];
		T * p = temp;
		T * q = m_buf;
		ZeroMemory(temp, sizeof(T) * (row) * (col));
		for (int i = 0; i <= minr; i++)
		{
			memcpy(p, q, sizeof(T) * minc);
			p += col;
			q += m_cols;
		}
		delete [] m_buf;
		m_buf = temp;
		m_rows = row;
		m_cols = col;
	}

	T * operator [] (int i) 
	{
		assert(i >= 0 && i < m_rows);
		return m_buf + i * m_cols;
	}

	const T * operator [] (int i) const
	{
		assert(i >= 0 && i < m_rows);
		return m_buf + i * m_cols;
	}

	Array2D & operator = (const Array2D & arr)
	{
		if (this == &arr) return;
		if (!m_attached)
			delete [] m_buf;
		m_rows = arr.m_rows;
		m_cols = arr.m_cols;
		m_attached = arr.m_attached;

		if (m_attached)
		{
			m_buf = arr.m_buf;
		}else
		{
			m_buf = new T[m_rows * m_cols];
			memcpy(m_buf, arr.m_buf, sizeof(T) * m_cols * m_rows);
		}
		return *this;
	}

	T * GetBuf()
	{
		return m_buf;
	}

};
