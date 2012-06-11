#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::function_call const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name in the global module table.
			llvm::Function *CalleeF = module->getFunction(x.idf.name);
			if (CalleeF == 0)
			{
				return ErrorV("Unknown function '"+x.idf.name+"' referenced!");
			}
  
			// If argument number mismatch error.
			if (CalleeF->arg_size() != x.arguments.size())
			{
				std::stringstream sstr;
				sstr << "Expected " << CalleeF->arg_size() << " arguments but " << x.arguments.size() << " are given.";
				return ErrorV("Incorrect number of arguments passed to '"+x.idf.name+"' ! "+sstr.str());
			}

			// argument type match?
			std::vector<llvm::Value*> ArgsV;
			auto itArg = CalleeF->arg_begin();
			BOOST_FOREACH(ast::expression const & ex, x.arguments)
			{
				ArgsV.push_back((*this)(ex));
				if(!ArgsV.back())
				{
					std::stringstream sstr;
					sstr << ex;
					return ErrorV("Invalid argument returned from '" +sstr.str()+ "'!");
				}

				if(ArgsV.back()->getType()!=(*itArg).getType())
				{
					std::string type_str;
					llvm::raw_string_ostream rso(type_str);
					rso << "Trying to call function '" << x.idf.name << "' with argument number " << (*itArg).getArgNo() << " with type '";
					ArgsV.back()->getType()->print(rso);
					rso << "' but function expects a value of type '";
					(*itArg).getType()->print(rso);
					rso << "'.";
					return ErrorV("Argument type mismatch! "+rso.str());
				}
				++itArg;
			}
  
			return builder.CreateCall(CalleeF, ArgsV, "calltmp");
		}
	}
}