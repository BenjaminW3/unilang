#pragma once

// http://stackoverflow.com/questions/538134/exporting-functions-from-a-dll-with-dllexport
// http://msdn.microsoft.com/en-us/library/a90k134d(v=vs.80).aspx
// http://msdn.microsoft.com/de-de/library/3y1sfaz2.aspx

#ifdef _WIN32
#	ifdef UNILANG_EXPORTS_EXTERN_C
#			define U_EXPORT extern "C" __declspec(dllexport)
#	else
#		ifdef UNILANG_EXPORTS
#			define U_EXPORT __declspec(dllexport)
#		else
#			define U_EXPORT __declspec(dllimport)
#		endif
#	endif
#else
#    define U_EXPORT
#endif