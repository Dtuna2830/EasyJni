#pragma once
#include "JNIClassMirror.h"

namespace EasyJni
{

template <FixedString Name, typename Ret, typename... Args>
class JNIMethodMirror
{
public:
	template <typename Owner>
		requires IsJNIClassMirror<Owner>
	JNIMethodMirror(Owner *o)
		: owner(static_cast<JNIClassMirrorLink<Owner::ClassName()> *>(o)),
		  mid(midCache<Owner::ClassName()>.load(std::memory_order_acquire))
	{
		if (!owner || mid) return;
		JNIClass jniClass(owner->jclsr);
		jmethodID id = jniClass.getMethod<Ret, Args...>(owner->jenv, Name.data).getId();
		JNIException::CheckAndThrow(owner->jenv);
		mid = id;
		midCache<Owner::ClassName()>.store(id, std::memory_order_release);
	}

	JNIMethodMirror(const JNIMethodMirror &) = delete;
	JNIMethodMirror &operator=(const JNIMethodMirror &) = delete;

	Ret operator()(const Args &...args)
	{
		JNIMethod<JNIMirrorType<Ret>, JNIMirrorType<Args>...> method(mid);
		if constexpr (std::is_void_v<Ret>)
		{
			method.call(owner->jenv, owner->jobj.get(), JNIMirrorTypeV(args)...);
			JNIException::CheckAndThrow(owner->jenv);
		}
		else
		{
			JNIMirrorType<Ret> ret = method.call(owner->jenv, owner->jobj.get(), JNIMirrorTypeV(args)...);
			if constexpr (IsJNIObjectMirror<Ret>)
			{
				JNILocalRef<jobject> local(owner->jenv, ret);
				JNIException::CheckAndThrow(owner->jenv);
				return Ret(owner->jenv, local.get());
			}
			else if constexpr (IsJNIArray<Ret> || IsJNIString<Ret> || IsJNIException<Ret>)
			{
				Ret retv(owner->jenv, ret);
				JNIException::CheckAndThrow(owner->jenv);
				return retv;
			}
			else
			{
				JNIException::CheckAndThrow(owner->jenv);
				return ret;
			}
		}
	}

private:
	JNIClassMirrorBase *owner = nullptr;
	jmethodID mid = nullptr;
	template <FixedString ClassName>
	inline static std::atomic<jmethodID> midCache{nullptr};
};

template <FixedString Name, typename Ret, typename... Args>
class JNIStaticMethodMirror
{
public:
	template <typename Owner>
		requires IsJNIClassMirror<Owner>
	JNIStaticMethodMirror(Owner *o)
		: owner(static_cast<JNIClassMirrorLink<Owner::ClassName()> *>(o)),
		  mid(midCache<Owner::ClassName()>.load(std::memory_order_acquire))
	{
		if (!owner || mid) return;
		JNIClass jniClass(owner->jclsr);
		jmethodID id = jniClass.getStaticMethod<Ret, Args...>(owner->jenv, Name.data).getId();
		JNIException::CheckAndThrow(owner->jenv);
		mid = id;
		midCache<Owner::ClassName()>.store(id, std::memory_order_release);
	}

	JNIStaticMethodMirror(const JNIStaticMethodMirror &) = delete;
	JNIStaticMethodMirror &operator=(const JNIStaticMethodMirror &) = delete;

	Ret operator()(const Args &...args)
	{
		JNIStaticMethod<JNIMirrorType<Ret>, JNIMirrorType<Args>...> method(mid);
		if constexpr (std::is_void_v<Ret>)
		{
			method.call(owner->jenv, owner->jclsr, JNIMirrorTypeV(args)...);
			JNIException::CheckAndThrow(owner->jenv);
		}
		else
		{
			JNIMirrorType<Ret> ret = method.call(owner->jenv, owner->jclsr, JNIMirrorTypeV(args)...);
			if constexpr (IsJNIObjectMirror<Ret>)
			{
				JNILocalRef<jobject> local(owner->jenv, ret);
				JNIException::CheckAndThrow(owner->jenv);
				return Ret(owner->jenv, local.get());
			}
			else if constexpr (IsJNIArray<Ret> || IsJNIString<Ret> || IsJNIException<Ret>)
			{
				Ret retv(owner->jenv, ret);
				JNIException::CheckAndThrow(owner->jenv);
				return retv;
			}
			else
			{
				JNIException::CheckAndThrow(owner->jenv);
				return ret;
			}
		}
	}

private:
	JNIClassMirrorBase *owner = nullptr;
	jmethodID mid = nullptr;
	template <FixedString ClassName>
	inline static std::atomic<jmethodID> midCache{nullptr};
};

} // namespace EasyJni