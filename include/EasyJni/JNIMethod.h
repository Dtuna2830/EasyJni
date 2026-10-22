#pragma once
#include <jni.h>
#include <type_traits>
#include "AlwaysFalse.h"

namespace EasyJni
{

template <typename Ret, typename... Args>
class JNIMethod
{
public:
	JNIMethod() = default;
	JNIMethod(jmethodID id) : mid(id)
	{
	}

	explicit operator bool() const
	{
		return mid != nullptr;
	}

	jmethodID getId() const
	{
		return mid;
	}

	Ret call(JNIEnv *env, jobject obj, Args... args)
	{
		if constexpr (std::is_same_v<Ret, void>) return env->CallVoidMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jboolean>) return env->CallBooleanMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jbyte>) return env->CallByteMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jchar>) return env->CallCharMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jshort>) return env->CallShortMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jint>) return env->CallIntMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jlong>) return env->CallLongMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jfloat>) return env->CallFloatMethod(obj, mid, args...);
		else if constexpr (std::is_same_v<Ret, jdouble>) return env->CallDoubleMethod(obj, mid, args...);
		else if constexpr (std::is_convertible_v<Ret, jobject>)
			return static_cast<Ret>(env->CallObjectMethod(obj, mid, args...));
		else static_assert(always_false<Ret>, "Unsupported type");
	}

private:
	jmethodID mid = nullptr;
};

template <typename Ret, typename... Args>
class JNIStaticMethod
{
public:
	JNIStaticMethod() = default;
	JNIStaticMethod(jmethodID id) : mid(id)
	{
	}

	explicit operator bool() const
	{
		return mid != nullptr;
	}

	jmethodID getId() const
	{
		return mid;
	}

	Ret call(JNIEnv *env, jclass cls, Args... args)
	{
		if constexpr (std::is_same_v<Ret, void>) return env->CallStaticVoidMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jboolean>) return env->CallStaticBooleanMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jbyte>) return env->CallStaticByteMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jchar>) return env->CallStaticCharMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jshort>) return env->CallStaticShortMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jint>) return env->CallStaticIntMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jlong>) return env->CallStaticLongMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jfloat>) return env->CallStaticFloatMethod(cls, mid, args...);
		else if constexpr (std::is_same_v<Ret, jdouble>) return env->CallStaticDoubleMethod(cls, mid, args...);
		else if constexpr (std::is_convertible_v<Ret, jobject>)
			return static_cast<Ret>(env->CallStaticObjectMethod(cls, mid, args...));
		else static_assert(always_false<Ret>, "Unsupported type");
	}

private:
	jmethodID mid = nullptr;
};

} // namespace EasyJni