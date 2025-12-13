#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <string>

class Shader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> cb;
public:
	enum ShaderType {
		Regular,
		PostProcess
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetPixelBuffer();
	//Microsoft::WRL::ComPtr<ID3D11Buffer> GetCB();

	void Setup();
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout();
	void LoadPixelShader(std::string fileName);
	void LoadVertexShader(std::string fileName, ShaderType type);
	void SetInputLayout(ID3DBlob* vertexShaderBlob, ShaderType type);
	void CreatePixelBuffer();

};

