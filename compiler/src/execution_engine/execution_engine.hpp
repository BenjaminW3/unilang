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
		//! \param module The generated llvm::Module.
		//! \return The ErrorCode returned from the program.
		//-----------------------------------------------------------------------------
		U_EXPORT int64_t execute_module( std::shared_ptr<llvm::Module> module );

		//-----------------------------------------------------------------------------
		//! Executes the module inside the given bitcode file.
		//!
		//! \param sBitCodeFilePath The path to the bitcode file to be loaded.
		//! \return The ErrorCode returned from the program.
		//-----------------------------------------------------------------------------
		U_EXPORT int64_t execute_bitcode_file( std::string const & sBitCodeFilePath );
	}
}