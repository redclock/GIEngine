#include <Windows.h>
#include "QuadTree.h"
#include <math.h>

static const int ParamC = 10;

CQuadTree::Node::Node(int r, int c, Node * parent)
{
	this->r = r;
	this->c = c;
	this->parent = parent;
	if (parent != NULL) depth = parent->depth - 1;
	this->extented = false;
	memset(neighbors, 0, sizeof(children));

	memset(children, 0, sizeof(children));

}

CQuadTree::Node::~Node()
{
	for (int i = 0; i < 4; i++)
	{
		if (children[i]) delete children[i];
	}
}

bool CQuadTree::Node::IsDisabled() const
{
	return !(parent->extented);
}

void CQuadTree::Node::ReLOD(float eyer, float eyec)
{
	float dist = (abs(eyec - c) + abs(eyer - r));
	int d = 1 << depth;
	float c = 5;
	float f = dist / (d * ParamC * max(1, c * d2));
	if (depth > 0 && f < 1)
	{
		extented = true;
		children[0]->ReLOD(eyer, eyec);
		children[1]->ReLOD(eyer, eyec);
		children[2]->ReLOD(eyer, eyec);
		children[3]->ReLOD(eyer, eyec);
	}else
	{
		extented = false;
		if (depth > 0)
		{
			children[0]->Disable();
			children[1]->Disable();
			children[2]->Disable();
			children[3]->Disable();
		}
	}
}


void CQuadTree::Node::Extend()
{
	if (depth < 1) return;

	int n = 1<< (depth - 1);
	children[0] = new CQuadTree::Node(r - n, c - n, this);
	children[0]->Extend();
	children[1] = new CQuadTree::Node(r - n, c + n, this);
	children[1]->Extend();
	children[2] = new CQuadTree::Node(r + n, c - n, this);
	children[2]->Extend();
	children[3] = new CQuadTree::Node(r + n, c + n, this);
	children[3]->Extend();
}

void CQuadTree::Node::ConnectNeighbor()
{
	if (depth < 1) return;
	if (neighbors[0])
	{
		children[0]->neighbors[0] = neighbors[0]->children[2];
		children[1]->neighbors[0] = neighbors[0]->children[3];
	}
	if (neighbors[1])
	{
		children[0]->neighbors[1] = neighbors[1]->children[1];
		children[2]->neighbors[1] = neighbors[1]->children[3];
	}
	if (neighbors[2])
	{
		children[1]->neighbors[2] = neighbors[2]->children[0];
		children[3]->neighbors[2] = neighbors[2]->children[2];
	}
	if (neighbors[3])
	{
		children[2]->neighbors[3] = neighbors[3]->children[0];
		children[3]->neighbors[3] = neighbors[3]->children[1];
	}

	children[0]->neighbors[2] = children[1];
	children[0]->neighbors[3] = children[2];
	children[1]->neighbors[1] = children[0];
	children[1]->neighbors[3] = children[3];
	children[2]->neighbors[0] = children[0];
	children[2]->neighbors[2] = children[3];
	children[3]->neighbors[0] = children[1];
	children[3]->neighbors[1] = children[2];

	children[0]->ConnectNeighbor();
	children[1]->ConnectNeighbor();
	children[2]->ConnectNeighbor();
	children[3]->ConnectNeighbor();
}

void CQuadTree::Node::Disable()
{
	extented = false;
	if (depth > 0)
	{
		children[0]->Disable();
		children[1]->Disable();
		children[2]->Disable();
		children[3]->Disable();
	}
}

CQuadTree::CQuadTree(int d)
	: m_depth(d), m_length((1 << d) + 1), m_root(NULL)
		
{
}

CQuadTree::~CQuadTree()
{
	if (m_root) delete m_root;
}

void CQuadTree::BuildTree()
{
	if (m_root) return;

	m_root = new Node(m_length / 2, m_length / 2, NULL);
	m_root->depth = m_depth - 1;
	m_root->Extend();
	m_root->extented = true;

	m_root->ConnectNeighbor();
}

void CQuadTree::ReLOD(float eyer, float eyec)
{
	this->eyer = eyer;
	this->eyec = eyec;
	m_root->ReLOD(eyer, eyec);
}

inline void CQuadTree::AddIndex(DWORD * indices, int r, int c) const
{
	*indices = r * m_length + c;
}

int CQuadTree::DrawNode(const Node * node, DWORD * indices) const
{
	int w = (1 << node->depth);
	int count = 0;
	if (node->extented == false || node->depth == 0)
	{
		if (node->neighbors[0] && node->neighbors[0]->IsDisabled())
		{
			AddIndex(indices++, node->r - w, node->c + w);
			AddIndex(indices++, node->r - w, node->c - w);
			AddIndex(indices++, node->r, node->c);
			count += 3;
		}else
		{
			AddIndex(indices++, node->r - w, node->c);
			AddIndex(indices++, node->r - w, node->c - w);
			AddIndex(indices++, node->r, node->c);

			AddIndex(indices++, node->r - w, node->c + w);
			AddIndex(indices++, node->r - w, node->c);
			AddIndex(indices++, node->r, node->c);
			count += 6;
		}

		if (node->neighbors[1] && node->neighbors[1]->IsDisabled())
		{
			AddIndex(indices++, node->r - w, node->c -w);
			AddIndex(indices++, node->r + w, node->c - w);
			AddIndex(indices++, node->r, node->c);
			count += 3;
		}else
		{
			AddIndex(indices++, node->r, node->c - w);
			AddIndex(indices++, node->r + w, node->c - w);
			AddIndex(indices++, node->r, node->c);

			AddIndex(indices++, node->r - w, node->c - w);
			AddIndex(indices++, node->r, node->c - w);
			AddIndex(indices++, node->r, node->c);
			count += 6;
		}

		if (node->neighbors[2] && node->neighbors[2]->IsDisabled())
		{
			AddIndex(indices++, node->r + w, node->c + w);
			AddIndex(indices++, node->r - w, node->c + w);
			AddIndex(indices++, node->r, node->c);
			count += 3;
		}else
		{
			AddIndex(indices++, node->r, node->c + w);
			AddIndex(indices++, node->r - w, node->c + w);
			AddIndex(indices++, node->r, node->c);

			AddIndex(indices++, node->r + w, node->c + w);
			AddIndex(indices++, node->r, node->c + w);
			AddIndex(indices++, node->r, node->c);
			count += 6;
		}

		if (node->neighbors[3] && node->neighbors[3]->IsDisabled())
		{
			AddIndex(indices++, node->r + w, node->c - w);
			AddIndex(indices++, node->r + w, node->c + w);
			AddIndex(indices++, node->r, node->c);
			count += 3;
		}else
		{
			AddIndex(indices++, node->r + w, node->c);
			AddIndex(indices++, node->r + w, node->c + w);
			AddIndex(indices++, node->r, node->c);

			AddIndex(indices++, node->r + w, node->c - w);
			AddIndex(indices++, node->r + w, node->c);
			AddIndex(indices++, node->r, node->c);
			count += 6;
		}
	}else
	{
		count += DrawNode(node->children[0], indices + count);
		count += DrawNode(node->children[1], indices + count);
		count += DrawNode(node->children[2], indices + count);
		count += DrawNode(node->children[3], indices + count);
	}

	return count;
}

void CQuadTree::CalcRoughness(Node* node, const Array2D<float>& heights, float lenScale)
{
	int n = 1<< node->depth;
	float dh;
	dh = abs(heights[node->r - n][node->c - n] + heights[node->r + n][node->c - n] - 2 * heights[node->r][node->c - n]);
	node->d2 = dh;
	dh = abs(heights[node->r - n][node->c + n] + heights[node->r + n][node->c + n] - 2 * heights[node->r][node->c + n]);
	if (node->d2 < dh) node->d2 = dh;
	dh = abs(heights[node->r - n][node->c - n] + heights[node->r - n][node->c + n] - 2 * heights[node->r - n][node->c]);
	if (node->d2 < dh) node->d2 = dh;
	dh = abs(heights[node->r + n][node->c - n] + heights[node->r + n][node->c + n] - 2 * heights[node->r + n][node->c]);
	if (node->d2 < dh) node->d2 = dh;
	dh = abs(heights[node->r - n][node->c - n] + heights[node->r + n][node->c + n] - 2 * heights[node->r][node->c]);
	if (node->d2 < dh) node->d2 = dh;
	dh = abs(heights[node->r - n][node->c + n] + heights[node->r + n][node->c - n] - 2 * heights[node->r][node->c]);
	if (node->d2 < dh) node->d2 = dh;
	node->d2 /= n * 4 * lenScale;
	
	if (node->depth > 0)
	{
		CalcRoughness(node->children[0], heights, lenScale);
		CalcRoughness(node->children[1], heights, lenScale);
		CalcRoughness(node->children[2], heights, lenScale);
		CalcRoughness(node->children[3], heights, lenScale);
	}

}

void CQuadTree::CalcRoughness(const Array2D<float>& heights, float lenScale)
{
	CalcRoughness(m_root, heights, lenScale); 
	SmoothD2();
}

int CQuadTree::Draw(DWORD * indices) const
{
	return DrawNode(m_root, indices);
}

void CQuadTree::SmoothD2Node(Node * node, int nid, int cid)
{
	float K = ParamC /(2.0f * (ParamC - 1));

	if (node->neighbors[nid])
	{
		float dd = node->neighbors[nid]->children[cid]->d2 * K;
		if (dd > node->d2) node->d2 = dd;
	}
}

void CQuadTree::SmoothD2()
{
	Node * leaf = m_root;
	if (m_depth < 2) return; 
	for (int i = 0; i < m_depth - 2; i++)
	{
		leaf = leaf->children[0];
	}

	while(leaf)
	{
		Node * line = leaf;
		while (line)
		{
			Node * node = line;
			while (node)
			{
				SmoothD2Node(node, 0, 2);
				SmoothD2Node(node, 0, 3);
				SmoothD2Node(node, 1, 1);
				SmoothD2Node(node, 1, 3);
				SmoothD2Node(node, 2, 0);
				SmoothD2Node(node, 2, 2);
				SmoothD2Node(node, 3, 0);
				SmoothD2Node(node, 3, 1);
				//->
				node = node->neighbors[2];
			}
			//|
			//V
			line = line->neighbors[3];
		}
		leaf = leaf->parent;
	}
}