#pragma once

#include <unilang/compiler/export.hpp>

#include <memory>
#include <string>

// forward declarations
namespace llvm
{
	class Module;
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining just-in-time execution_engine.
	//-------------------------------------------------------------------------
	namespace execution_engine
	{
		//-------------------------------------------------------------------------
		//! Executes the given llvm::Module.
		//!
		//! \param module The llvm::Module to execute.
		//! \return The ErrorCode returned from the program.
		//-------------------------------------------------------------------------
		U_EXPORT int64_t execute_module(llvm::Module const & module);
	}
}