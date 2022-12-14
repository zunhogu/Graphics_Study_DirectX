#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass(){
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other){}


ColorShaderClass::~ColorShaderClass(){}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	//정점 셰이더와 픽셀 셰이더를 초기화한다.
	result = InitializeShader(device, hwnd, L"../Engine/color.vs", L"../Engine/color.ps");
	if (!result) {
		return false;
	}
	return true;
}

void ColorShaderClass::Shutdown() {
	// 정점 셰이더와 픽셀 셰이더 및 그와 관련된 것들을 반환한다.
	ShutdownShader();

	return;
}

// Render 함수에서는 SetShaderParameters 함수를 사용해 셰이더에서 사용될 인자들을 설정한다.
// 인자들이 설정되면 RenderShader 함수를 호출해 활성화된 정점버퍼들을 가지고 셰이더를 이용해서 그리기를 수행한다.
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {

	bool result;

	// 렌더링에 사용할 셰이더의 인자를 입력한다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	// 셰이더의 인자를 다 받아왔다면 셰이더 이용하여 활성화된 버퍼를 그린다.
	RenderShader(deviceContext, indexCount);

	return true;
}


// 가장 중요한 InitializeShader 함수를 사용하여 Shader를 초기화시킨다.
// 이 함수에서는 실제 셰이더 파일을 불러오고 DIrectX와 GPU에서 사용 가능하도록 해준다.
// 또한 레이아웃을 세팅하고 어덯게 정점버퍼의 데이터가 GPU에서 사용되는지 볼 수 있다.
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {

	// 셰이더 초기화 순서는 셰이더 컴파일 -> 셰이더 생성 -> 셰이더에서 사용할 정점 성분 구조체를 description을 서술 한다 -> 구조체를 입력 레이아웃에 집어넣어는다.
	// 입력 레이아웃(입력 배치)에 넣게되면 Direct3D에게 정점 성분 정보를 알려준다.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// 이 함수에서 사용하는 포인터들을 null로 설정한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0",0, 0,&vertexShaderBuffer, &errorMessage);
	if (FAILED(result)){
		// 셰이더가 컴파일에 실패하면 에러 메세지를 기록한다.
		if (errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메세지가 없다면 셰이더 파일을 찾지 못한다.
		else{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0",0, 0 ,&pixelShaderBuffer, &errorMessage);
	if (FAILED(result)){
		// 셰이더 컴파일이 실패하면 에러 메세지를 기록한다.
		if (errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 에러 메세지가 없다면 단순히 셰이더 파일을 찾지 못한다.
		else{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 정점 셰이더와 픽셀 셰이더가 버퍼로 잘 컴파일되면 이를 이용하여 셰이더 객체를 만들 수 있다.
	// 여기서 나온 포인터를 정점 셰이더와 픽셀 셰이더의 인터페이스로서 사용한다.

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)){
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성한다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)){
		return false;
	}

	// 정점 입력 레이아웃 description을 작성한다.
	// 이 설정은 ModelClass와 셰이더에 있는 VertexType와 일치해야 한다.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)){
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 셰이더를 사용하기 위한 마지막 단계는 상수 버퍼를  설정하는 것이다.
	// 현재 정점 셰이더에서 하나의 상수버퍼를 가지고 있기 때문에 그것을 세팅하여 셰이더에 대한 인터페이스에서 사용할 수 있다.
	// 단 매 프레임마다 상수버퍼를 업데이트하기 때문에 버퍼의 사용은 동적일 필요가 있다.
	// 바로 BindFlags 플래그로 상수버퍼를 이 버퍼로 한다는것을 설정한다.
	// CPUAccessFlags도 용도에 맞춰야 하기 대문에 D3D11_CPU_ACCESS_WRITE로 설정한다.
	// descrition이 채워지고 나면 상수 버퍼의 인터페이스를 만들고 SetShaderParameters 함수를 이용하여 셰이더의 내부 변수들에 접근할 수 있다.

	// 정점 셰이더에 있는 행렬 상수 버퍼의 description을 작성한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)){
		return false;
	}

	return true;
}

//ShutdownShader 함수에서는 InitializeShader 함수에서 생성했던 4개의 인터페이스들을 정리한다.
void ColorShaderClass::ShutdownShader(){
	// 상수 버퍼를 해제한다.
	if (m_matrixBuffer){
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃을 해제한다.
	if (m_layout){
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더를 해제한다.
	if (m_pixelShader){
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점 셰이더를 해제한다.
	if (m_vertexShader){
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//OutputShaderErrorMessage 함수에서는 정점 / 픽셀 셰이더의 컴파일 도중 오류로 인해 만들어진 에러 메세지를 출력하는 일을 한다.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져온다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메세지의 길이를 가져온다.
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메세지를 기록한다.
	fout.open("shader-error.txt");

	// 에러 메세지를 쓴다.
	for (i = 0; i < bufferSize; i++){
		fout << compileErrors[i];
	}

	// 파일을 닫는다.
	fout.close();

	// 에러 메세지를 반환한다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려준다.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


// SetShaderVariables 함수는 셰이더의 전역변수를 쉽게 다룰 수 있게 하기 위해 만들어졌다.
// 이 함수에 사용된 행렬들은 GraphicsClass에서 만들어진 것이다.

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix){
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)){
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

// 셰이더 전역변수 설정이 마무리 되고 나면, RenderShader 함수에서는 입력 레이아웃을 입력 어셈블러에 연결하는 것이다. 이로써 GPU 정점 버퍼의 자료구조를 알게 된다.
// 또 정점버퍼를 그리기 위한 정점셰이더와 픽셀 셰이더를 설정하는 것이다. 셰이더가 설정되고 나면 Direct3D 장치 컨텍스트에서 DirectX11 DrawIndexed 함수를 이용해 삼각형을 그려낸다.
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

