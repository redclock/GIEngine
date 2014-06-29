#pragma once
#include <vector>
#include <stdlib.h>
#include "Array2D.h"

//�Ĳ���
class CQuadTree
{
private:
	//�ڵ�
	struct Node
	{
		//����λ��
		int r;
		int c;
		//��� Ҷ��Ϊ0 ��� 2^(depth+1)
		int depth;
		//�Ƿ���չ
		bool extented;
		//�ֲڶ�
		float d2;
		
		Node * parent;
		//�ӽڵ�
		//0 1
		//2 3
		Node * children[4]; 
		//�ھ�
		//  0
		//1 M 2
		//  3
		Node * neighbors[4];

		Node(int r, int c, Node * parent);
		~Node();
		
		void Extend();
		void ConnectNeighbor();

		void ReLOD(float eyer, float eyec);
		void Disable();
		bool IsDisabled() const;

	};
private:
	int m_depth;
	int m_length;
	Node * m_root; 
	void SmoothD2Node(Node * node, int nid, int cid);
	void SmoothD2();
	void CalcRoughness(Node* node, const Array2D<float>& heights, float lenScale);

	void AddIndex(DWORD * indices, int r, int c) const;
	int DrawNode(const Node * node, DWORD * indices) const;
	float eyer;
	float eyec;
public:
	void BuildTree();
	void ReLOD(float eyer, float eyec);
	void CalcRoughness(const Array2D<float>& heights, float lenScale);
	int GetLength() const { return m_length ;}

	int Draw(DWORD * indices) const;

	CQuadTree(int d);
	~CQuadTree();

};

bool LoadHeightMap(const char * filename, Array2D<float> & map);
