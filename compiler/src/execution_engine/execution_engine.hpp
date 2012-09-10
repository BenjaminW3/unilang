#pragma once

#include <memory>

// predefinitions
namespace llvm
{
	class Module;
}

namespace unilang
{
	//-----------------------------------------------------------------------------
	//! The namespace defining just-in-time execution_engine.
	//-----------------------------------------------------------------------------
	namespace execution_engine
	{
		//-----------------------------------------------------------------------------
		//! Executes the given llvm::Module.
		//!
		//! \param module The generated llvm::Module.
		//! \return The ErrorCode returned from the program.
		//-----------------------------------------------------------------------------
		int execute_module( std::shared_ptr<llvm::Module> module );
	}
}