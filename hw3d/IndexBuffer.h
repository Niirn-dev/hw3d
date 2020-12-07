#pragma once
#include "Bindable.h"
#include <optional>

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer( Graphics& gfx,const std::string& tag,const std::vector<unsigned short>& indices );
		IndexBuffer( Graphics& gfx,const std::vector<unsigned short>& indices );
		void Bind( Graphics& gfx ) noexcept override;
		UINT GetCount() const noexcept;

		static std::shared_ptr<IndexBuffer> Resolve( Graphics& gfx,const std::string& tag,const std::vector<unsigned short>& indices ) noxnd;
		static std::string GenerateUID( const std::string& tag,const std::optional<std::vector<unsigned short>>& indices ) noexcept;
		std::string GetUID() const noexcept override;
	protected:
		UINT count;
		std::string tag;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}