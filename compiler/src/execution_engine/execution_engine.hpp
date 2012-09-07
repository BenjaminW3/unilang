#pragma once

#include <memory>

// predefinitions
namespace llvm
{
	class Module;
}

namespace unilang
{
	namespace execution_engine
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		int execute_module( std::shared_ptr<llvm::Module> module );
	}
}