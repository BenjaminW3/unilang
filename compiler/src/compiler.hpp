#pragma once

#include "export.hpp"

#include <string>
#include <memory>

// predefinitions
//-----------------------------------------------------------------------------
//! The llvm namespace.
//-----------------------------------------------------------------------------
namespace llvm
{
	class Module;
}

//-----------------------------------------------------------------------------
//! The namespace defining the unilang language.
//-----------------------------------------------------------------------------
namespace unilang
{
	//-----------------------------------------------------------------------------
	//! The namespace defining the compiler.
	//! 
	//! To compile a file call compile_file and use the returned module to execute it just-in-time with the execution_engine.
	//-----------------------------------------------------------------------------
	namespace compiler
	{
		//-----------------------------------------------------------------------------
		//! Defines the verbosity of the debug output.
		//-----------------------------------------------------------------------------
		enum EDebugOutputOptions
		{
			Standard	= 0,
			SourceCode	= 1,
			Unoptimized	= 2,
			Optimized	= 4,
			All			= SourceCode | Unoptimized | Optimized,
		};

		//-----------------------------------------------------------------------------
		//! Compiles the given file.
		//! The returned module will be optimized.
		//!
		//! \param sSourceCodeFilePath The Path to the source code file.
		//! \param output The verbosity of the debug output.
		//! \return The llvm::Module being created.
		//-----------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> compile_file( std::string const & sSourceCodeFilePath, EDebugOutputOptions const output = EDebugOutputOptions::Standard );
	}
}