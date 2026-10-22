#pragma once
#include <jni.h>
#include "AlwaysFalse.h"
#include "FixedString.h"

namespace EasyJni
{

template <typename T>
struct JNISignature
{
	static consteval auto Value()
	{
		static_assert(always_false<T>, "JNISignature is not defined for this type.");
	}
};

template <typename T>
struct JNIArraySignature
{
	static consteval auto Value()
	{
		return FixedString("[") + JNISignature<T>::Value();
	}
};

template <typename Ret, typename... Args>
struct JNIMethodSignature
{
	static consteval auto Value()
	{
		if constexpr (sizeof...(Args) == 0) return FixedString("()") + JNISignature<Ret>::Value();
		else
			return FixedString("(") + (JNISignature<Args>::Value() + ...) + FixedString(")") +
				   JNISignature<Ret>::Value();
	}
};

#define REGISTER_JNI_OBJECT_SIGNATURE(TYPE, SIG)                                                                       \
	template <>                                                                                                        \
	struct EasyJni::JNISignature<TYPE>                                                                                 \
	{                                                                                                                  \
		static consteval auto Value()                                                                                  \
		{                                                                                                              \
			return EasyJni::FixedString("L") + EasyJni::FixedString(SIG) + EasyJni::FixedString(";");                  \
		}                                                                                                              \
	};

// clang-format off
template<> struct JNISignature<void> { static consteval auto Value() { return FixedString("V"); } };
template<> struct JNISignature<jboolean> { static consteval auto Value() { return FixedString("Z"); } };
template<> struct JNISignature<jbyte> { static consteval auto Value() { return FixedString("B"); } };
template<> struct JNISignature<jchar> { static consteval auto Value() { return FixedString("C"); } };
template<> struct JNISignature<jshort> { static consteval auto Value() { return FixedString("S"); } };
template<> struct JNISignature<jint> { static consteval auto Value() { return FixedString("I"); } };
template<> struct JNISignature<jlong> { static consteval auto Value() { return FixedString("J"); } };
template<> struct JNISignature<jfloat> { static consteval auto Value() { return FixedString("F"); } };
template<> struct JNISignature<jdouble> { static consteval auto Value() { return FixedString("D"); } };
template<> struct JNISignature<jobject> { static consteval auto Value() { return FixedString("Ljava/lang/Object;"); } };
template<> struct JNISignature<jstring> { static consteval auto Value() { return FixedString("Ljava/lang/String;"); } };
template<> struct JNISignature<jclass> { static consteval auto Value() { return FixedString("Ljava/lang/Class;"); } };
template<> struct JNISignature<jthrowable> { static consteval auto Value() { return FixedString("Ljava/lang/Throwable;"); } };
// clang-format on

template <typename T>
concept IsJNIPrimitive =
	std::is_same_v<T, jboolean> || std::is_same_v<T, jbyte> || std::is_same_v<T, jchar> || std::is_same_v<T, jshort> ||
	std::is_same_v<T, jint> || std::is_same_v<T, jlong> || std::is_same_v<T, jfloat> || std::is_same_v<T, jdouble>;

} // namespace EasyJni