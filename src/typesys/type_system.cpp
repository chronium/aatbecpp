#include <atomic>
#include <typesys/type_system.hpp>

namespace aatbe::typesys {

static std::atomic<size_t> next_tid{};

TypeId TypeId::GetNext(TypeSystem *ts) { return {ts, next_tid.fetch_add(1)}; }

#define Resolve(T)                                                             \
  template <> T *TypeId::Resolve<T>() const {                                  \
    return type_system->GetType(*this)->As<T>();                               \
  }

Resolve(StructType);
Resolve(IntType);
Resolve(FloatType);
Resolve(BoolType);
Resolve(CharType);
Resolve(UnitType);
Resolve(ArrayType);
Resolve(PointerType);
Resolve(VoidType);
Resolve(UnknownType);

} // namespace aatbe::typesys
