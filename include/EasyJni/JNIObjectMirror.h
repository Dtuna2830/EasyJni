#pragma once
#include "JNIArray.h"

namespace EasyJni
{

class JNIObjectMirror;

template <typename T>
concept IsJNIObjectMirror = std::is_base_of_v<JNIObjectMirror, T>;

template <typename T>
using JNIMirrorType =
	std::conditional_t<IsJNIObjectMirror<T>, jobject,
					   std::conditional_t<IsJNIArray<T>, jarray,
										  std::conditional_t<IsJNIString<T>, jstring,
															 std::conditional_t<IsJNIException<T>, jthrowable, T>>>>;

class JNIObjectMirror
{
	template <typename T>
	friend JNIMirrorType<T> JNIMirrorTypeV(const T &);

protected:
	JNIObjectMirror(JNIEnv *env, jobject obj) : jenv(env), jobj(JNILocalRef<jobject>::New(env, obj))
	{
	}

	JNIEnv *jenv = nullptr;
	JNILocalRef<jobject> jobj;
};

} // namespace EasyJni