#ifndef ENGINEOBJECT_HPP
#define ENGINEOBJECT_HPP

#include <stddef.h>
#include <stdint.h>
#include <string.h>

template<typename T>
class EngineObject
{
public:
	typedef T cdataType;
	// Maybe think of another solution later...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
	static const ptrdiff_t cdataOffset = offsetof(EngineObject<T>, cdata);
#pragma GCC diagnostic pop
protected:
	T cdata;
public:
	explicit EngineObject()
	{
		// Initialize with explicit zeroes :)
		memset(&this->cdata, 0, sizeof(T));
	}

	virtual ~EngineObject() = default;

	T const & api() const { return this->cdata; }
	T & api() { return this->cdata; }
};

template<typename T>
static inline T * promote(typename T::cdataType * value)
{
	if(value != nullptr) {
		uintptr_t ptr = reinterpret_cast<uintptr_t>(value);
		ptr	-= T::cdataOffset;
		EngineObject<typename T::cdataType> * obj = reinterpret_cast<EngineObject<typename T::cdataType>*>(ptr);
		return static_cast<T*>(obj);
	} else {
		return nullptr;
	}
}

template<typename T>
static inline T const * promote(typename T::cdataType const * value)
{
	if(value != nullptr) {
		uintptr_t ptr = reinterpret_cast<uintptr_t>(value);
		ptr	-= T::cdataOffset;
		EngineObject<typename T::cdataType> const * obj = reinterpret_cast<EngineObject<typename T::cdataType> const *>(ptr);
		return static_cast<T const *>(obj);
	} else {
		return nullptr;
	}
}

template<typename T>
static inline T * demote(EngineObject<T> * value)
{
	if(value != nullptr) {
		uintptr_t t = reinterpret_cast<uintptr_t>(value);
		t += EngineObject<T>::cdataOffset;
		return reinterpret_cast<T*>(t);
	} else {
		return nullptr;
	}
}

#endif // ENGINEOBJECT_HPP
