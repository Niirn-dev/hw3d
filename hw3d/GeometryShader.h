#pragma once

#include "Bindable.h"
#include <string>

class GeometryShader : public Bindable
{
public:
	GeometryShader( Graphics& gfx,const std::wstring& file );
	void Bind( Graphics& gfx ) noexcept override;

private:
	bool isEnabled = false;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pShader = nullptr;
};
