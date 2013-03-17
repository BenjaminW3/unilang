#pragma once

// base classes
#include <boost/noncopyable.hpp>

// members
#include <vector>
#include <string>

namespace unilang
{
	// forward declarations
	namespace ast
	{
		struct module;
		struct namespace_declaration;
		struct function_declaration;
		struct function_definition;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class function_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class namespace_code_generator :	//public boost::static_visitor<bool>,
											virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			namespace_code_generator(	code_generator_errors & codeGeneratorErrors, 
										function_code_generator & functionCodeGenerator );

		public:
			//-----------------------------------------------------------------------------
			//! \return The namespaces from outermost to innermost.
			//-----------------------------------------------------------------------------
			std::vector<std::string> getCurrentNamespaceHierarchy() const;

			//-----------------------------------------------------------------------------
			//! \return The concatenation of namespaces.
			//-----------------------------------------------------------------------------
			std::string getCurrentMangledNamespaceName() const;

			//-----------------------------------------------------------------------------
			//! \param vsNamespaceHierarchy The namespaces from outermost to innermost.
			//! \return The concatenation of namespaces.
			//-----------------------------------------------------------------------------
			static std::string buildMangledNamespaceName( std::vector<std::string> const & vsNamespaceHierarchy );

			typedef bool result_type;

			bool operator()(ast::module const& x);
			bool operator()(ast::namespace_declaration const& x);

		public: // public for static_visitor
			bool operator()(ast::function_declaration const & x);
			bool operator()(ast::function_definition const & x);

		private:
			std::vector<std::string> m_vsNamespaceHierarchy;

			code_generator_errors & m_codeGeneratorErrors;
			function_code_generator & m_functionCodeGenerator;
		};
	}
}