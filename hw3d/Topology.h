#pragma once
#include "Bindable.h"

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		Topology( Graphics& gfx,D3D11_PRIMITIVE_TOPOLOGY type );
		void Bind( Graphics& gfx ) noexcept override;

		static std::shared_ptr<Topology> Resolve( Graphics& gfx,D3D11_PRIMITIVE_TOPOLOGY type ) noxnd;
		static std::string GenerateUID( D3D11_PRIMITIVE_TOPOLOGY type ) noexcept;
		std::string GetUID() const noexcept;
	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};
}