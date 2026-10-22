#pragma once
#include "JNISignature.h"
#include "JNIMethod.h"
#include "JNIField.h"

namespace EasyJni
{

class JNIClass
{
public:
	JNIClass() = default;
	explicit JNIClass(jclass cls) : jcls(cls)
	{
	}

	explicit operator bool() const
	{
		return jcls != nullptr;
	}

	template <FixedString Name>
	static JNIClass Find(JNIEnv *env)
	{
		jclass cls = env->FindClass(Name.data);
		if (!cls) return JNIClass();
		return JNIClass(cls);
	}

	static JNIClass From(jclass cls)
	{
		return JNIClass(cls);
	}

	jclass get() const
	{
		return jcls;
	}

	template <typename Ret, typename... Args>
	JNIMethod<Ret, Args...> getMethod(JNIEnv *env, const char *name)
	{
		if (!jcls) return JNIMethod<Ret, Args...>();
		constexpr auto sig = JNIMethodSignature<Ret, Args...>::Value();
		jmethodID mid = env->GetMethodID(jcls, name, sig.data);
		if (!mid) return JNIMethod<Ret, Args...>();
		return JNIMethod<Ret, Args...>(mid);
	}

	template <typename Ret, typename... Args>
	JNIStaticMethod<Ret, Args...> getStaticMethod(JNIEnv *env, const char *name)
	{
		if (!jcls) return JNIStaticMethod<Ret, Args...>();
		constexpr auto sig = JNIMethodSignature<Ret, Args...>::Value();
		jmethodID mid = env->GetStaticMethodID(jcls, name, sig.data);
		if (!mid) return JNIStaticMethod<Ret, Args...>();
		return JNIStaticMethod<Ret, Args...>(mid);
	}

	template <typename T>
	JNIField<T> getField(JNIEnv *env, const char *name)
	{
		if (!jcls) return JNIField<T>();
		constexpr auto sig = JNISignature<T>::Value();
		jfieldID fid = env->GetFieldID(jcls, name, sig.data);
		if (!fid) return JNIField<T>();
		return JNIField<T>(fid);
	}

	template <typename T>
	JNIStaticField<T> getStaticField(JNIEnv *env, const char *name)
	{
		if (!jcls) return JNIStaticField<T>();
		constexpr auto sig = JNISignature<T>::Value();
		jfieldID fid = env->GetStaticFieldID(jcls, name, sig.data);
		if (!fid) return JNIStaticField<T>();
		return JNIStaticField<T>(fid);
	}

private:
	jclass jcls = nullptr;
};

} // namespace EasyJni