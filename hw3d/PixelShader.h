#pragma once
#include "Bindable.h"
#include <string>

class PixelShader : public Bindable
{
public:
	PixelShader( Graphics& gfx,const std::wstring& file );
	void Bind( Graphics& gfx ) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
};