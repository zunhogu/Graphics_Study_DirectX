// TextureClass는 텍스처 자원을 불러오고 해제하는 작업을 캡슐화한다.
// 모든 텍스처에 대해서 이 클래스가 만들어져 있어야한다.

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <directxtex.h>

class TextureClass {
private:
	// 텍스처 자원을 담을 포인터
	ID3D11ShaderResourceView* m_texture;
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	// 아래 두함수는 주어진 파일의 이름으로 텍스처를 불러오고 더이상 사용되지 않는 텍스처는 해제한다.
	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	DirectX::ScratchImage LoadTextureFromFile(WCHAR*);

	// GetTexture함수는 셰이더에게 불러온 텍스처를 반환한다.
	ID3D11ShaderResourceView* GetTexture();
};

#endif