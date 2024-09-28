#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Logger.h"

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <WICTextureLoader.h>
using namespace DirectX;
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
protected:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct Vertex {
		float x, y, z;
	};

	struct Texture {
		float u, v;
	};

	struct Normal {
		float nx, ny, nz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct Face {
		int v1, v2, v3;
		int t1, t2, t3;
		int n1, n2, n3;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, std::vector<ID3D11ShaderResourceView*>);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(int);
	bool ChangeTexture(ID3D11Device*, ID3D11DeviceContext*, std::wstring filename, int index);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, vector<string> filename);
	void ReleaseTextures();

	bool LoadModel(char*);
	bool LoadObjModel(char*);
	bool LoadTxtModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	std::vector<ID3D11ShaderResourceView*> m_Textures;
	ModelType* m_model;
};

#endif