#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture( Graphics& gfx,const Surface& s,UINT slot = 0 );
		void Bind( Graphics& gfx ) noexcept override;
	protected:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
