#ifndef MM_ConstantsH
#define MM_ConstantsH

namespace MM {

/*! Maximum lenght of a Keyword string. */
inline unsigned int		MAX_KW_LEN() { return 24; }

/*! Constant for user defined material IDs. */
inline unsigned int		USER_MAT_ID() { return (0x1 << 24); }

} // namespace MM

/*! \file MM_Constants.h
	\brief Contains functions that return constants which are used throughout the material library.
*/

#endif  // MM_ConstantsH

