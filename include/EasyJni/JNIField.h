#pragma once
#include <jni.h>
#include <type_traits>
#include "AlwaysFalse.h"

namespace EasyJni
{

template <typename T>
class JNIField
{
public:
	JNIField() = default;
	JNIField(jfieldID id) : fid(id)
	{
	}

	explicit operator bool() const
	{
		return fid != nullptr;
	}

	jfieldID getId() const
	{
		return fid;
	}

	T get(JNIEnv *env, jobject obj)
	{
		if constexpr (std::is_same_v<T, jboolean>) return env->GetBooleanField(obj, fid);
		else if constexpr (std::is_same_v<T, jbyte>) return env->GetByteField(obj, fid);
		else if constexpr (std::is_same_v<T, jchar>) return env->GetCharField(obj, fid);
		else if constexpr (std::is_same_v<T, jshort>) return env->GetShortField(obj, fid);
		else if constexpr (std::is_same_v<T, jint>) return env->GetIntField(obj, fid);
		else if constexpr (std::is_same_v<T, jlong>) return env->GetLongField(obj, fid);
		else if constexpr (std::is_same_v<T, jfloat>) return env->GetFloatField(obj, fid);
		else if constexpr (std::is_same_v<T, jdouble>) return env->GetDoubleField(obj, fid);
		else if constexpr (std::is_convertible_v<T, jobject>) return static_cast<T>(env->GetObjectField(obj, fid));
		else static_assert(always_false<T>, "Unsupported type");
	}

	void set(JNIEnv *env, jobject obj, T value)
	{
		if constexpr (std::is_same_v<T, jboolean>) return env->SetBooleanField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jbyte>) return env->SetByteField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jchar>) return env->SetCharField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jshort>) return env->SetShortField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jint>) return env->SetIntField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jlong>) return env->SetLongField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jfloat>) return env->SetFloatField(obj, fid, value);
		else if constexpr (std::is_same_v<T, jdouble>) return env->SetDoubleField(obj, fid, value);
		else if constexpr (std::is_convertible_v<T, jobject>) return env->SetObjectField(obj, fid, value);
		else static_assert(always_false<T>, "Unsupported type");
	}

private:
	jfieldID fid = nullptr;
};

template <typename T>
class JNIStaticField
{
public:
	JNIStaticField() = default;
	JNIStaticField(jfieldID id) : fid(id)
	{
	}

	explicit operator bool() const
	{
		return fid != nullptr;
	}

	jfieldID getId() const
	{
		return fid;
	}

	T get(JNIEnv *env, jclass cls)
	{
		if constexpr (std::is_same_v<T, jboolean>) return env->GetStaticBooleanField(cls, fid);
		else if constexpr (std::is_same_v<T, jbyte>) return env->GetStaticByteField(cls, fid);
		else if constexpr (std::is_same_v<T, jchar>) return env->GetStaticCharField(cls, fid);
		else if constexpr (std::is_same_v<T, jshort>) return env->GetStaticShortField(cls, fid);
		else if constexpr (std::is_same_v<T, jint>) return env->GetStaticIntField(cls, fid);
		else if constexpr (std::is_same_v<T, jlong>) return env->GetStaticLongField(cls, fid);
		else if constexpr (std::is_same_v<T, jfloat>) return env->GetStaticFloatField(cls, fid);
		else if constexpr (std::is_same_v<T, jdouble>) return env->GetStaticDoubleField(cls, fid);
		else if constexpr (std::is_convertible_v<T, jobject>)
			return static_cast<T>(env->GetStaticObjectField(cls, fid));
		else static_assert(always_false<T>, "Unsupported type");
	}

	void set(JNIEnv *env, jclass cls, T value)
	{
		if constexpr (std::is_same_v<T, jboolean>) env->SetStaticBooleanField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jbyte>) env->SetStaticByteField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jchar>) env->SetStaticCharField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jshort>) env->SetStaticShortField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jint>) env->SetStaticIntField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jlong>) env->SetStaticLongField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jfloat>) env->SetStaticFloatField(cls, fid, value);
		else if constexpr (std::is_same_v<T, jdouble>) env->SetStaticDoubleField(cls, fid, value);
		else if constexpr (std::is_convertible_v<T, jobject>) env->SetStaticObjectField(cls, fid, value);
		else static_assert(always_false<T>, "Unsupported type");
	}

private:
	jfieldID fid = nullptr;
};

} // namespace EasyJni