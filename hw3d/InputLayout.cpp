#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	using namespace std::string_literals;

	InputLayout::InputLayout( Graphics& gfx,
		const Dvtx::VertexLayout& layout,
		ID3DBlob* pVertexShaderBytecode )
		:
		vertexLayout( layout )
	{
		INFOMAN( gfx );

		const auto d3dlayout = layout.GetD3DLayout();
		GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
			d3dlayout.data(),(UINT)d3dlayout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&pInputLayout
		) );
	}

	void InputLayout::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->IASetInputLayout( pInputLayout.Get() );
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve( Graphics& gfx,const Dvtx::VertexLayout& layout,ID3DBlob* pVertexShaderBlob ) noxnd
	{
		return Codex::Resolve<InputLayout>( gfx,layout,pVertexShaderBlob );
	}
	std::string InputLayout::GenerateUID( const Dvtx::VertexLayout& layout,ID3DBlob* pVertexBlob ) noexcept
	{
		return typeid( InputLayout ).name() + "#"s + layout.GetCode();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID( vertexLayout );
	}
}
