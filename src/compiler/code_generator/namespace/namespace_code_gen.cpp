#include <unilang/compiler/code_generator/namespace/namespace_code_gen.hpp>

#include <unilang/compiler/code_generator/code_generator.hpp>

#include <unilang/compiler/ast/ast.hpp>

#include <unilang/compiler/log/log.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		namespace_code_generator::namespace_code_generator(	code_generator_errors & codeGeneratorErrors, 
															function_code_generator & functionCodeGenerator ) :
			m_codeGeneratorErrors	(codeGeneratorErrors),
			m_functionCodeGenerator	(functionCodeGenerator)
		{};
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool namespace_code_generator::operator()(ast::module const& x)
		{
			LOG_SCOPE_DEBUG;
			
			bool bSuccess (true);
			for(ast::meta_entity const & meta : x._metaEntities)
			{
				bSuccess &= (meta.apply_visitor(*this));
			}

			return bSuccess;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool namespace_code_generator::operator()(ast::namespace_declaration const& x)
		{
			LOG_SCOPE_DEBUG;
			
			m_vsNamespaceHierarchy.push_back(x._idfName._name);
			
			bool bSuccess (true);
			for(ast::meta_entity const & meta : x._metaEntities)
			{
				bSuccess &= (meta.apply_visitor(*this));
			}
			
			m_vsNamespaceHierarchy.pop_back();

			return bSuccess;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool namespace_code_generator::operator()(ast::function_declaration const & x)
		{
			return m_functionCodeGenerator(x) != nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool namespace_code_generator::operator()(ast::function_definition const & x)
		{
			return m_functionCodeGenerator(x) != nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::vector<std::string> namespace_code_generator::getCurrentNamespaceHierarchy() const
		{
			return m_vsNamespaceHierarchy;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::string namespace_code_generator::getCurrentMangledNamespaceName() const
		{
			return buildMangledNamespaceName(getCurrentNamespaceHierarchy());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::string namespace_code_generator::buildMangledNamespaceName( std::vector<std::string> const & vsNamespaceHierarchy )
		{
			std::string sMangledNamespaceName;
			for(std::string const & sNamespace : vsNamespaceHierarchy)
			{
				sMangledNamespaceName += sNamespace + "::";
			}
			return sMangledNamespaceName;
		}
	}
}