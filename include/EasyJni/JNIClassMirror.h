#pragma once
#include <atomic>
#include "JNIObjectMirror.h"

namespace EasyJni
{

template <typename T>
struct Extends
{
};

template <typename... T>
struct Implements
{
};

template <typename... T>
struct Constructors
{
};

template <typename T>
struct ExtendsTrait : std::false_type
{
};

template <typename T>
struct ExtendsTrait<Extends<T>> : std::true_type
{
};

template <typename T>
struct ImplementsTrait : std::false_type
{
};

template <typename... T>
struct ImplementsTrait<Implements<T...>> : std::true_type
{
};

template <typename T>
struct ConstructorsTrait : std::false_type
{
};

template <typename... T>
struct ConstructorsTrait<Constructors<T...>> : std::true_type
{
};

template <typename T>
concept IsExtends = ExtendsTrait<T>::value;

template <typename T>
concept IsImplements = ImplementsTrait<T>::value;

template <typename T>
concept IsConstructors = ConstructorsTrait<T>::value;

template <typename... T>
struct IsUnique : std::true_type
{
};

template <typename T, typename... Ts>
struct IsUnique<T, Ts...> : std::bool_constant<(!std::is_same_v<T, Ts> && ...) && IsUnique<Ts...>::value>
{
};

template <typename... D>
concept ValidDescriptors = ((IsExtends<D> || IsImplements<D> || IsConstructors<D>) && ...) && IsUnique<D...>::value &&
						   ((IsExtends<D> + ... + 0) <= 1) && ((IsImplements<D> + ... + 0) <= 1);

template <typename C, typename... D>
concept ConstructorMatch = (std::is_same_v<C, D> || ...);

template <FixedString, typename, typename...>
class JNIMethodMirror;

template <FixedString, typename, typename...>
class JNIStaticMethodMirror;

template <FixedString, typename>
class JNIFieldMirror;

template <FixedString, typename>
class JNIStaticFieldMirror;

class JNIClassMirrorBase : public JNIObjectMirror
{
	template <FixedString, typename... D>
		requires ValidDescriptors<D...>
	friend class JNIClassMirror;

	template <FixedString, typename, typename...>
	friend class JNIMethodMirror;

	template <FixedString, typename, typename...>
	friend class JNIStaticMethodMirror;

	template <FixedString, typename>
	friend class JNIFieldMirror;

	template <FixedString, typename>
	friend class JNIStaticFieldMirror;

	template <FixedString>
	friend class JNIClassMirrorLink;

private:
	JNIClassMirrorBase(JNIEnv *env, jobject obj) : JNIObjectMirror(env, obj)
	{
	}

	jclass jclsr = nullptr;
};

template <FixedString Name>
class JNIClassMirrorLink : public JNIClassMirrorBase
{
protected:
	JNIClassMirrorLink(JNIEnv *env, jobject obj) : JNIClassMirrorBase(env, obj)
	{
	}
};

template <FixedString Name, typename D>
class JNIMirrorDescriptorBase
{
protected:
	JNIMirrorDescriptorBase(JNIEnv *, jobject)
	{
	}
};

template <FixedString Name, typename T>
class JNIMirrorDescriptorBase<Name, Extends<T>> : public T
{
protected:
	JNIMirrorDescriptorBase(JNIEnv *env, jobject obj) : T(env, obj)
	{
	}
};

template <FixedString Name, typename... T>
class JNIMirrorDescriptorBase<Name, Implements<T...>> : public T...
{
protected:
	JNIMirrorDescriptorBase(JNIEnv *env, jobject obj) : T(env, obj)...
	{
	}
};

template <typename T>
concept IsJNIClassMirror = std::is_base_of_v<JNIClassMirrorBase, T>;

template <typename T>
JNIMirrorType<T> JNIMirrorTypeV(const T &value)
{
	if constexpr (IsJNIClassMirror<T>)
	{
		return value.object();
	}
	else if constexpr (IsJNIObjectMirror<T>)
	{
		return value.jobj.get();
	}
	else if constexpr (IsJNIArray<T> || IsJNIString<T> || IsJNIException<T>)
	{
		return value.get();
	}
	else
	{
		return value;
	}
}

template <FixedString Name, typename... D>
	requires ValidDescriptors<D...>
class JNIClassMirror : public JNIClassMirrorLink<Name>, public JNIMirrorDescriptorBase<Name, D>...
{
	template <typename>
	friend struct JNISignature;

	template <typename>
	friend class JNIArray;

	template <typename T>
	friend JNIMirrorType<T> JNIMirrorTypeV(const T &);

	template <FixedString, typename, typename...>
	friend class JNIMethodMirror;

	template <FixedString, typename, typename...>
	friend class JNIStaticMethodMirror;

	template <FixedString, typename>
	friend class JNIFieldMirror;

	template <FixedString, typename>
	friend class JNIStaticFieldMirror;

public:
	JNIClassMirror(JNIEnv *env, jobject obj)
		: JNIClassMirrorLink<Name>(env, obj), JNIMirrorDescriptorBase<Name, D>(env, obj)...
	{
		JNIClassMirrorLink<Name>::jclsr = Class(env);
	}

	template <typename... A>
		requires ConstructorMatch<Constructors<A...>, D...>
	JNIClassMirror(JNIEnv *env, const A &...args) : JNIClassMirror(env, NewObject(env, args...).get())
	{
	}

	JNIClassMirror(const JNIClassMirror &) = delete;
	JNIClassMirror &operator=(const JNIClassMirror &) = delete;

private:
	jobject object() const
	{
		return JNIClassMirrorLink<Name>::jobj.get();
	}

	static consteval auto ClassName()
	{
		return Name;
	}

	static jclass Class(JNIEnv *env)
	{
		jclass cached = ajcls.load(std::memory_order_acquire);
		if (cached) return cached;
		JNILocalRef<jclass> local(env, JNIClass::Find<Name>(env).get());
		JNIException::CheckAndThrow(env);
		JNIGlobalRef<jclass> global(env, std::move(local));
		JNIException::CheckAndThrow(env);
		if (!global) throw JNIException("NewGlobalRef failed");
		jclass cls = global.get();
		jclass expected = nullptr;
		if (!ajcls.compare_exchange_strong(expected, cls, std::memory_order_acq_rel, std::memory_order_acquire))
			return expected;
		jcls = std::move(global);
		return cls;
	}

	template <typename... A>
	static JNILocalRef<jobject> NewObject(JNIEnv *env, const A &...args)
	{
		jclass cls = Class(env);
		JNIClass jniClass(cls);
		JNIMethod<void, A...> jniMethod = jniClass.getMethod<void, A...>(env, "<init>");
		JNIException::CheckAndThrow(env);
		JNILocalRef<jobject> jobj(env, env->NewObject(cls, jniMethod.getId(), JNIMirrorTypeV(args)...));
		JNIException::CheckAndThrow(env);
		return jobj;
	}

	inline static std::atomic<jclass> ajcls{nullptr};
	inline static JNIGlobalRef<jclass> jcls;
};

/*
template <FixedString Name, typename... D>
	requires ValidDescriptors<D...>
struct JNISignature<JNIClassMirror<Name, D...>>
{
	static consteval auto Value()
	{
		return FixedString("L") + Name + FixedString(";");
	}
};
*/

template <typename T>
	requires IsJNIClassMirror<T>
struct JNISignature<T>
{
	static consteval auto Value()
	{
		return FixedString("L") + T::ClassName() + FixedString(";");
	}
};

} // namespace EasyJni