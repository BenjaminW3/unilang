#pragma once

#include "../export.hpp"

#include <memory>
#include <string>

// forward declarations
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
		//! \param m_ruleModule The generated llvm::Module.
		//! \return The ErrorCode returned from the program.
		//-----------------------------------------------------------------------------
		U_EXPORT int64_t execute_module( llvm::Module & m_ruleModule );
	}
}