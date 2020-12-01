#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture( Graphics& gfx,const std::string& path,UINT slot = 0 );
		void Bind( Graphics& gfx ) noexcept override;

		static std::shared_ptr<Bindable> Resolve( Graphics& gfx,const std::string& path,UINT slot = 0 ) noxnd;
		static std::string GenerateUID( const std::string& path,UINT slot ) noexcept;
		std::string GetUID() const noexcept;
	protected:
		UINT slot;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
