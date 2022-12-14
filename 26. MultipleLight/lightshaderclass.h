// ColorShaderClass는 GPU 상에 존재하는 3D 모델들을 그리는 데 사용하는 HLSL 셰이더를 호출하는 것이다.

#ifndef _TEXTURESHADERCLASS_H_ 
#define _TEXTURESHADERCLASS_H_

#include <d3d11.h> 
#include <DirectXMath.h>
//#include <d3dx11async.h> 
#include  <d3dcompiler.h>
#include <fstream> 
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

const int NUM_LIGHTS = 4;

class LightShaderClass {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightColorBufferType { 
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	}; 
	struct LightPositionBufferType { 
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	//아래 함수들은 셰이더의 초기화와 마무리를 제어한다.
	// Render 함수는 셰이더에 사용되는 변수들을 설정하고 셰이더를 이용해 준비된 모델의 정점들을 그려낸다.

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[]);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*); 
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[]);
	void RenderShader(ID3D11DeviceContext*, int);
private: 
	ID3D11VertexShader* m_vertexShader; 
	ID3D11PixelShader* m_pixelShader; 
	ID3D11InputLayout* m_layout; 
	// 전역변수 값을 초기화하기 위한 상수버퍼
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightColorBuffer; 
	ID3D11Buffer* m_lightPositionBuffer;
	// 텍스처 샘플러 객체를 만들기 위한 객체를 선언
	ID3D11SamplerState* m_sampleState;
};

#endif