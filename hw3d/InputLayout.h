#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout( Graphics& gfx,
			const Dvtx::VertexLayout& layout,
			ID3DBlob* pVertexShaderBytecode );
		void Bind( Graphics& gfx ) noexcept override;

		static std::shared_ptr<InputLayout> Resolve( Graphics& gfx,const Dvtx::VertexLayout& layout,ID3DBlob* pVertexShaderBlob ) noxnd;
		static std::string GenerateUID( const Dvtx::VertexLayout& layout,ID3DBlob* pVertexBlob = nullptr ) noexcept;
		std::string GetUID() const noexcept override;
	protected:
		const Dvtx::VertexLayout& vertexLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}