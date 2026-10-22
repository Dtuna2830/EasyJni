#pragma once
#include "JNIClassMirror.h"

namespace EasyJni
{

template <FixedString Name, typename T>
class JNIFieldMirror
{
public:
	template <typename Owner>
		requires IsJNIClassMirror<Owner>
	JNIFieldMirror(Owner *o)
		: owner(static_cast<JNIClassMirrorLink<Owner::ClassName()> *>(o)),
		  fid(fidCache<Owner::ClassName()>.load(std::memory_order_acquire))
	{
		if (!owner || fid) return;
		JNIClass jniClass(owner->jclsr);
		jfieldID id = jniClass.getField<T>(owner->jenv, Name.data).getId();
		JNIException::CheckAndThrow(owner->jenv);
		fid = id;
		fidCache<Owner::ClassName()>.store(id, std::memory_order_release);
	}

	JNIFieldMirror(const JNIFieldMirror &) = delete;

	operator T() const
	{
		JNIField<JNIMirrorType<T>> field(fid);
		JNIMirrorType<T> jtype = field.get(owner->jenv, owner->jobj.get());
		if constexpr (IsJNIObjectMirror<T>)
		{
			JNILocalRef<jobject> local(owner->jenv, jtype);
			JNIException::CheckAndThrow(owner->jenv);
			return T(owner->jenv, local.get());
		}
		else if constexpr (IsJNIArray<T> || IsJNIString<T> || IsJNIException<T>)
		{
			T jtypev(owner->jenv, jtype);
			JNIException::CheckAndThrow(owner->jenv);
			return jtypev;
		}
		else
		{
			JNIException::CheckAndThrow(owner->jenv);
			return jtype;
		}
	}

	JNIFieldMirror &operator=(const T &value)
	{
		JNIField<JNIMirrorType<T>> field(fid);
		field.set(owner->jenv, owner->jobj.get(), JNIMirrorTypeV(value));
		return *this;
	}

	JNIFieldMirror &operator=(const JNIFieldMirror &other)
	{
		return *this = other.operator T();
	}

private:
	JNIClassMirrorBase *owner = nullptr;
	jfieldID fid = nullptr;
	template <FixedString ClassName>
	inline static std::atomic<jfieldID> fidCache{nullptr};
};

template <FixedString Name, typename T>
class JNIStaticFieldMirror
{
public:
	template <typename Owner>
		requires IsJNIClassMirror<Owner>
	JNIStaticFieldMirror(Owner *o)
		: owner(static_cast<JNIClassMirrorLink<Owner::ClassName()> *>(o)),
		  fid(fidCache<Owner::ClassName()>.load(std::memory_order_acquire))
	{
		if (!owner || fid) return;
		JNIClass jniClass(owner->jclsr);
		jfieldID id = jniClass.getStaticField<T>(owner->jenv, Name.data).getId();
		JNIException::CheckAndThrow(owner->jenv);
		fid = id;
		fidCache<Owner::ClassName()>.store(id, std::memory_order_release);
	}

	JNIStaticFieldMirror(const JNIStaticFieldMirror &) = delete;

	operator T() const
	{
		JNIStaticField<JNIMirrorType<T>> field(fid);
		JNIMirrorType<T> jtype = field.get(owner->jenv, owner->jclsr);
		if constexpr (IsJNIObjectMirror<T>)
		{
			JNILocalRef<jobject> local(owner->jenv, jtype);
			JNIException::CheckAndThrow(owner->jenv);
			return T(owner->jenv, local.get());
		}
		else if constexpr (IsJNIArray<T> || IsJNIString<T> || IsJNIException<T>)
		{
			T jtypev(owner->jenv, jtype);
			JNIException::CheckAndThrow(owner->jenv);
			return jtypev;
		}
		else
		{
			JNIException::CheckAndThrow(owner->jenv);
			return jtype;
		}
	}

	JNIStaticFieldMirror &operator=(const T &value)
	{
		JNIStaticField<JNIMirrorType<T>> field(fid);
		field.set(owner->jenv, owner->jclsr, JNIMirrorTypeV(value));
		return *this;
	}

	JNIStaticFieldMirror &operator=(const JNIStaticFieldMirror &other)
	{
		return *this = other.operator T();
	}

private:
	JNIClassMirrorBase *owner = nullptr;
	jfieldID fid = nullptr;
	template <FixedString ClassName>
	inline static std::atomic<jfieldID> fidCache{nullptr};
};

} // namespace EasyJni