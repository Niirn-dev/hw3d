#pragma once
#include "Bindable.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		static std::shared_ptr<Bindable> Resolve( const std::string& key ) noxnd
		{
			return Get_().Resolve_( key );
		}
		static void Store( std::shared_ptr<Bindable> bind ) noxnd
		{
			Get_().Store_( std::move( bind ) );
		}
	private:
		std::shared_ptr<Bindable> Resolve_( const std::string& key ) noxnd
		{
			auto bindIt = binds.find( key );
			if ( bindIt != binds.end() )
			{
				return bindIt->second;
			}
			else
			{
				return {};
			}
		}
		void Store_( std::shared_ptr<Bindable> bind ) noxnd
		{
			binds[bind->GetUID()] = std::move( bind );
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