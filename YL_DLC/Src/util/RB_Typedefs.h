//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Typedefs.h
//! \ingroup	util
//! \brief		Predefined data types for all Rainbow modules (processor independent)
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2017/08/24 12:07:23MESZ $
// $Revision: 1.84 $
//
//==================================================================================================

#ifndef _RB_Typedefs__h
#define _RB_Typedefs__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include <string.h> // required fo memset()

//#include "RB_Sysdefs.h"
#include <stdint.h>
#include <stdbool.h>

//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================
#define   RB_DECL_FUNC
#define   RB_DECL_TYPE
    
    //! Float 32 bit
typedef float RB_DECL_TYPE float32;

//! Float 64 bit
typedef double RB_DECL_TYPE float64;

    
//#ifndef RB_RAMFUNCTION
//	#define RB_RAMFUNCTION
//#endif
//
//#define RB_ENDIANESS_LITTLE		0
//#define RB_ENDIANESS_BIG		1

//! Definition of endianess
//typedef enum {
//	RB_TYPEDEFS_ENDIANESS_LITTLE = RB_ENDIANESS_LITTLE,
//	RB_TYPEDEFS_ENDIANESS_BIG    = RB_ENDIANESS_BIG
//} RB_DECL_TYPE RB_TYPEDEFS_tEndianess;
//
//// Get endianness of MCU by reading RB_SYSDEFS_ENDIANESS_LITTLE/BIG
//#if defined(RB_SYSDEFS_ENDIANESS_LITTLE)
//	#define RB_ENDIANNESS RB_ENDIANESS_LITTLE
//#elif defined(RB_SYSDEFS_ENDIANESS_BIG)
//	#define RB_ENDIANNESS RB_ENDIANESS_BIG
//#else
//	#error endianness must be defined in RB_Sysdefs.h for this MCU
//#endif

//! Number of elements of array arrayVar[].
//! Attention: The argument 'arrayVar' must be an array variable;
//!            it must not be a pointer variable pointing to the array.
//! The term 'SIZE' in the macro name is chosen because
//! - 'SIZEOF' typically refers to unit [char] or [octet].
//! - 'LENGTH' does often not denote the amount of allocated memory.
//! Using '0[arrayVar]', which is equivalent to 'arrayVar[0]' in C,
//! makes the compiler complain instead of giving a bad result
//! if arrayVar happens to be a C++ type with overloaded subscript operator '[]'.
//lint -emacro(409, RB_ARRAY_SIZE) (Warning -- Expecting a pointer or array) suppressed for 0[(arrayVar)]
//lint -emacro(866, RB_ARRAY_SIZE) (Info -- Unusual use of 'int+ptr' in argument to sizeof) suppressed for 0[(arrayVar)]
#define RB_ARRAY_SIZE(arrayVar) ((sizeof(arrayVar))/(sizeof(0[(arrayVar)])))

//! Set all array elements of an array to 0.
//! Attention: The argument 'arrayVar' must be an array variable;
//!            it must not be a pointer variable pointing to the array.
#define RB_RESET_ARRAY(arrayVar) memset((arrayVar), 0, sizeof(arrayVar))

//! Set all members of a struct to 0.
//! The argument 'struct_' must be a struct variable
//! or a dereferenced pointer pointing to a struct variable.
//! Therefore, the argument name 'struct_' is chosen instead of 'structVar'.
#define RB_RESET_STRUCT(struct_) memset(&(struct_), 0, sizeof(struct_))

//! Return minimum of x and y
#define RB_MIN(x,y) ((x) < (y) ? (x) : (y))

//! Return maximum of x and y
#define RB_MAX(x,y) ((x) > (y) ? (x) : (y))

//! Used to prevent compiler warnings on unused arguments of functions
#ifndef __cplusplus
	#define RB_UNUSED(x) (/*lint --e(920) */(void)(x))				// C-style static cast
#else
	#define RB_UNUSED(x) (/*lint --e(920) */static_cast<void>(x))	// C++ static cast
#endif

//! Returns bit mask for a single bit from its bit number
#define RB_BIT(bit) (1uL << (bit))

//! Backward compatibility with RB 2.6.7-1 and before
#define RB_BIT_U32(bit) (1uL << (bit))

//! Get the number of octets (8 bit entities) used by a type or variable
//! Note: On ARM7, ARM9 and CortexM, a byte contains 8 bits.
//!       Generally, a byte can contain more than 8 bits (e.g. on TMS320 architecture)
#define RB_OCTET_SIZEOF(x) ((sizeof(x) * CHAR_BIT + 7) / 8)

//! Standard callback function, void FUNCTION(void)
typedef RB_DECL_TYPE void (*RB_tCallback)(void);

//! Attribute that marks functions as thread safe
//! Function declarations that are decorated with this attribute MUST BE IMPLEMENTED THREAD SAFE
//! Thread safe means that the global state of the system always must stay in a logically valid
//! state during the execution of the function since another threads could access the global state
//! meanwhile. Functions not accessing any global variables are thread safe. If functions are
//! accessing global variables, the access to the global variables must be protected by critical
//! sections (to prevent task switches).
//!
//! Contrary to "thread safe", the related expression "reentrant" originates from single-threaded
//! programming environments where the flow of control could be interrupted by a hardware interrupt
//! and transferred to an interrupt service routine (ISR). Any subroutine used by the ISR that could
//! potentially have been executing when the interrupt was triggered should be reentrant. A function
//! can be thread-safe and still not reentrant. For example, a function could be wrapped all around
//! with a mutex (which avoids problems in multithreading environments), but if that function is
//! used in an interrupt service routine, it could starve waiting for the first execution to
//! release the mutex.
#define RB_ATTR_THREAD_SAFE

//! Stringification of expanded macro argument
//! Note: If the stringizing operator '#' is applied to a macro argument
//!       then the argument is not macro-expanded first.
//!       In order to expand the macro argument, two levels of macros are needed.
#define RB_STRINGIFY(x)				RB_STRINGIFY_NOEXPAND(x)

//! Stringification of macro argument without macro expansion
#define RB_STRINGIFY_NOEXPAND(x)	#x

//! Concatenation of two macro-expanded arguments leaving no blank spaces between them
//! Note: If the token on either side of the token pasting operator '##' comes
//!       from a macro argument then the argument is not macro-expanded first.
//!       In order to expand the macro arguments, two levels of macros are needed.
#define RB_GLUE(x,y)				RB_GLUE_NOEXPAND(x,y)

//! Concatenation of two non-macro-expanded arguments leaving no blank spaces between them
#define RB_GLUE_NOEXPAND(x,y)		x ## y


#ifdef __cplusplus
}
#endif

#endif // _RB_Typedefs__h
