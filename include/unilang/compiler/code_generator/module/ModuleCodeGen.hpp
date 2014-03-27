#pragma once

// base classes
#include <boost/noncopyable.hpp>

// members
#include <vector>
#include <string>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SModule;
		struct SNamespaceDeclaration;
		struct SFunctionDeclaration;
		struct SFunctionDefinition;
	}

	namespace code_generator
	{
		class CErrorCodeGen;
		class CFunctionCodeGen;
	}
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class CModuleCodeGen :
			//public boost::static_visitor<bool>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CModuleCodeGen(
				CErrorCodeGen & codeGeneratorErrors, 
				CFunctionCodeGen & functionCodeGenerator );

		public:
			//-------------------------------------------------------------------------
			//! \return The namespaces from outermost to innermost.
			//-------------------------------------------------------------------------
			std::vector<std::string> getCurrentNamespaceHierarchy() const;

			//-------------------------------------------------------------------------
			//! \return The concatenation of namespaces.
			//-------------------------------------------------------------------------
			std::string getCurrentMangledNamespaceName() const;

			//-------------------------------------------------------------------------
			//! \param vsNamespaceHierarchy The namespaces from outermost to innermost.
			//! \return The concatenation of namespaces.
			//-------------------------------------------------------------------------
			static std::string buildMangledNamespaceName( std::vector<std::string> const & vsNamespaceHierarchy );

			typedef bool result_type;

			bool operator()(ast::SModule const & x);
			bool operator()(ast::SNamespaceDeclaration const & x);

		public: // public for static_visitor
			bool operator()(ast::SFunctionDeclaration const & x);
			bool operator()(ast::SFunctionDefinition const & x);

		private:
			std::vector<std::string> m_vsNamespaceHierarchy;

			CErrorCodeGen & m_codeGeneratorErrors;
			CFunctionCodeGen & m_functionCodeGenerator;
		};
	}
}