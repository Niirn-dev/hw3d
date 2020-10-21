#pragma once
#include "Bindable.h"
#include <string>

class VertexShader : public Bindable
{
public:
	VertexShader( Graphics& gfx,const std::wstring& file );
	void Bind( Graphics& gfx ) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecode = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
};
