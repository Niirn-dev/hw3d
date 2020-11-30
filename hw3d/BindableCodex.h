#pragma once
#include "Bindable.h"
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		template<class T,typename...Params>
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx,Params&&... p ) noxnd
		{
			static_assert( std::is_base_of_v<Bindable,T>,"Class T should be derived from Bindable" );
			return Get_().Resolve_<T>( gfx,std::forward<Params>( p )... );
		}
	private:
		template<class T,typename...Params>
		std::shared_ptr<Bindable> Resolve_( Graphics& gfx,Params&&... p ) noxnd
		{
			const auto key = T::GenerateUID( std::forward<Params>( p )... );
			const auto bindIt = binds.find( key );
			if ( bindIt != binds.end() )
			{
				return bindIt->second;
			}
			else
			{
				const auto bind = std::make_shared<T>( gfx,std::forward<Params>( p )... );
				binds[key] = bind;
				return bind;
			}
		}

		Codex() = default;
		static Codex& Get_() noxnd
		{
			static Codex codex;
			return codex;
		}

	private:
		std::unordered_map<std::string,std::shared_ptr<Bindable>> binds;
	};
}