#ifndef MM_ModelHandlerH
#define MM_ModelHandlerH

namespace MM {

/*!
	List of all creating models handled by the material generator.
*/
class ModelHandler
{
public:

	// ***KEYWORDLIST-START***

	enum ModelIdentifier {
		MI_GAUSS,				// Keyword: GAUSS
		MI_SERIAL_PARALLEL		// Keyword: SERIAL_PARALLEL
	};

	// ***KEYWORDLIST-END***


	ModelHandler();
};

} // namespace MM

#endif // MM_ModelHandlerH
