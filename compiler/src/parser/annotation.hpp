#pragma once

#include <vector>
//#include <boost/variant/apply_visitor.hpp>
#include "../ast/ast_base.hpp"
#include <type_traits>

namespace unilang
{
	//#########################################################################
    //! The annotation handler links the AST to a map of iterator positions for the purpose of subsequent semantic error handling when the program is being compiled.
    //#########################################################################
    template <typename Iterator>
	struct annotation
    {
        template <typename, typename>
        struct result { typedef void type; };

        std::vector<Iterator>& m_iters;
		
		//-----------------------------------------------------------------------------
		//! Constructor.
		//-----------------------------------------------------------------------------
        annotation(std::vector<Iterator>& iters) : m_iters(iters) {}
		
		//-----------------------------------------------------------------------------
		//! Copy-constructor.
		//-----------------------------------------------------------------------------
        annotation( annotation const & an) : m_iters(an.m_iters){}
		//-----------------------------------------------------------------------------
		//! Assignment-operator.
		//-----------------------------------------------------------------------------
		annotation & operator=( const annotation & an ) { m_iters = an.m_iters; }

        /*void operator()(ast::operand& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_annotation_id(id), ast);
            ast.id = id;
        }

        void operator()(ast::primary_expr& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_annotation_id(id), ast);
            ast.id = id;
        }

        void operator()(ast::statement& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_annotation_id(id), ast);
            ast.id = id;
        }*/
		
		//-----------------------------------------------------------------------------
		//! Sets ID if it is a base of ast::ast_base.
		//-----------------------------------------------------------------------------
		template <typename T>
		typename std::enable_if<std::is_base_of<ast::ast_base, T>::value, void>::type  operator()(T& ast, Iterator pos) const
        {
            size_t id = m_iters.size();
            m_iters.push_back(pos);
            ast.id = id;
        }
		//-----------------------------------------------------------------------------
		//! Does nothing if it is not a base of ast::ast_base.
		//-----------------------------------------------------------------------------
		template <typename T>
		typename std::enable_if<!std::is_base_of<ast::ast_base, T>::value, void>::type  operator()(T& /*ast*/, Iterator /*pos*/) const
        {
			std::cout << "NOT ABLE TO SET ANNOTATION ID FOR: " << typeid(T).name();
        }
    };
}