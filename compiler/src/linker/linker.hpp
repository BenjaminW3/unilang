#pragma once

#include "../export.hpp"

#include <memory>
#include <string>
#include <vector>

// forward declarations
namespace llvm
{
	class Module;
}

namespace unilang
{
	//-----------------------------------------------------------------------------
	//! The namespace defining the module linker.
	//-----------------------------------------------------------------------------
	namespace linker
	{
		//-----------------------------------------------------------------------------
		//! Loads the module from the given bitcode file.
		//!
		//! \param sBitCodeFilePath The path to the bitcode file to be loaded.
		//! \return The loaded module. Null if there has been an error.
		//-----------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> load_module_from_bitcode_file( std::string const & sBitCodeFilePath );

		//-----------------------------------------------------------------------------
		//! Loads the modules from the given bitcode files.
		//!
		//! \param vsBitcodeFilePaths The paths to the bitcode files to be loaded.
		//! \return The loaded module. Null if there has been an error.
		//-----------------------------------------------------------------------------
		U_EXPORT std::vector<std::shared_ptr<llvm::Module>> load_modules_from_bitcode_files( std::vector<std::string> const & vsBitcodeFilePaths );
		
		//-----------------------------------------------------------------------------
		//! Writes the module bitcode to the given file.
		//!
		//! \param spModule The module to be written.
		//! \param sBitCodeFilePath The path to the bitcode file being written.
		//-----------------------------------------------------------------------------
		U_EXPORT void write_module_to_bitcode_file( llvm::Module const & module, std::string const & sBitCodeFilePath );

		//-----------------------------------------------------------------------------
		//! Links the given modules together.
		//!
		//! \param vModules A list of all modules to be linked together.
		//! \return A new module containing all resolved parts.
		//-----------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> link_modules( std::vector<std::shared_ptr<llvm::Module>> vModules );
		
		//-----------------------------------------------------------------------------
		//! Links the given bitcode files together.
		//!
		//! \param vsBitcodeFilePaths A list of all paths to bitcode files to be linked together.
		//! \return A new module containing all resolved parts.
		//-----------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> link_bitcode_files_to_module( std::vector<std::string> const & vsBitcodeFilePaths );
	}
}