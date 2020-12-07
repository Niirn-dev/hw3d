#pragma once
#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader( Graphics& gfx,const std::string& path );
		void Bind( Graphics& gfx ) noexcept override;

		static std::shared_ptr<PixelShader> Resolve( Graphics& gfx,const std::string& path ) noxnd;
		static std::string GenerateUID( const std::string& path ) noexcept;
		std::string GetUID() const noexcept;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}