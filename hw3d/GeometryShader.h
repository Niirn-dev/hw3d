#pragma once

#include "Bindable.h"
#include <string>
#include <optional>

class GeometryShader : public Bindable
{
public:
	GeometryShader( Graphics& gfx,const std::optional<std::wstring>& file = {} );
	void Bind( Graphics& gfx ) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pShader = nullptr;
};
