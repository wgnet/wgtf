#ifndef QT_MODEL_HELPERS_HPP
#define QT_MODEL_HELPERS_HPP


#include <memory>
#include <vector>


namespace wgt
{
class IModelExtension;

namespace QtModelHelpers
{

typedef std::vector< IModelExtension * > Extensions;

int encodeRole( size_t roleId, const Extensions& extensions );
int calculateFirst( const size_t start );
int calculateLast( const size_t start, const size_t count );

} // namespace QtModelHelpers
} // end namespace wgt
#endif // QT_MODEL_HELPERS_HPP
