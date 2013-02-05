#pragma once

#include "export.hpp"

#include <string>
#include <memory>

//-----------------------------------------------------------------------------
//! \mainpage unilang
//! 
//! \section intro_sec Introduction
//! \verbinclude README.txt
//! 
//! \section install_sec The Language
//!
//! \subsection lang1 General
//! During the design of the language one essential rule was that whitespace should never have any semantic.
//! This means that there can not be types with multiple words like <tt>unsigned long long</tt> in c++.
//! Separation of identifiers and other things must be done with seperation characters like ':', ';', ',' or '|'.
//! There are also no braceless forms of if, for, etc. Due to this rule the <tt>{}</tt> have to be given explicitly.
//! 
//! Each statement line has to be delimited by a <tt>";"</tt>.
//!
//! \subsection lang2 Types
//! Unilang defines a few fundamental types. There is only the differentation between floating point and integer types of different precision.
//! Integer types implemented are i64, i32 and i1 commonly known as boolean type.
//! Floating point types supported are f64 and f32.
//!
//! There is no implicit type conversion. Each conversion has to be made explicitly. 
//! Numeric types can be converted into each other but be aware of the precision loss while casting between floating point numbers and the risc of truncation when casting between integers.
//! Integer upcasts from all integer types with more then one bit are always done by using sign extension. Bools are always unsigned so that true == 1 and false == 0.
//! Integer downcasts are done by trunction so there is no implicit mapping from integer values bigger then one to true in the case of a boolean cast!
//! E.g. <tt>bBool:i1{12};</tt> will evaluate to <tt>false</tt>!
//! Condition expressions must have boolean type. If you want C semantics you have to add "!= 0" explicitly.
//! 
//! \subsection lang3 Variables
//! All data Objects are zero initialised on creation.
//! Every variable is const by default. This means that they are only named values unable to change the underlying data.
//! E.g.: <tt>my64BitInteger:i64;</tt> defines a constant 64 Bit "0" value. 
//! To make it changeable add the qualifier symbol '~' in front of the typename.
//! E.g.: <tt>my64BitInteger:~i64;</tt>. Now you are able to change the calue in following statements. 
//! To give a special value as construction parameter you can use the initialiser-list syntax. E.g.: <tt>my64BitInteger:i64{42};</tt> defines a constant integer.
//! 
//! \subsection lang4 Functions
//! Functions can be defined like <tt>increment:(i:i64)->(ret:i64{++i});</tt>. This is an inline definition because we did not even need a function body at all.
//! We could also have written the function like: <tt>increment:(i:i64)->(ret:~i64){ret=i+1};</tt>
//!
//! \subsection lang5 Executable modules
//! The main module needs to have a function with the signature <tt>entrypoint:()->(i64{})</tt> 
//! This means it has to have a single i64 errorcode return value. Return 0 if there was no error.
//! 
// $ - The stringify operator
// if you write $ in front of a variable or expression its content is converted to a string. This can be used for Debug purposes.
//!
//-----------------------------------------------------------------------------

// forward declarations
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