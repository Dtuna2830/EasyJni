#pragma once
#include <jni.h>

namespace EasyJni
{

template <typename T>
class JNILocalRef
{
public:
	JNILocalRef() = default;
	JNILocalRef(JNIEnv *jenv, T jref) : env(jenv), ref(jref)
	{
	}
	~JNILocalRef()
	{
		reset();
	}

	JNILocalRef(const JNILocalRef &) = delete;
	JNILocalRef &operator=(const JNILocalRef &) = delete;

	JNILocalRef(JNILocalRef &&o) noexcept : env(o.env), ref(o.ref)
	{
		o.env = nullptr;
		o.ref = nullptr;
	}
	JNILocalRef &operator=(JNILocalRef &&o) noexcept
	{
		if (this != &o)
		{
			if (env && ref) env->DeleteLocalRef(ref);
			env = o.env;
			ref = o.ref;
			o.env = nullptr;
			o.ref = nullptr;
		}
		return *this;
	}
	explicit operator bool() const
	{
		return ref != nullptr;
	}

	static JNILocalRef New(JNIEnv *jenv, T jref)
	{
		if (!jenv || !jref) return JNILocalRef();
		return JNILocalRef(jenv, static_cast<T>(jenv->NewLocalRef(jref)));
	}

	JNILocalRef clone() const
	{
		if (!env) return JNILocalRef(nullptr, ref);
		return New(env, ref);
	}

	T get() const
	{
		return ref;
	}
	void reset()
	{
		if (env && ref) env->DeleteLocalRef(ref);
		env = nullptr;
		ref = nullptr;
	}
	T release()
	{
		T tpm = ref;
		env = nullptr;
		ref = nullptr;
		return tpm;
	}

private:
	JNIEnv *env = nullptr;
	T ref = nullptr;
};

template <typename T>
class JNIGlobalRef
{
public:
	JNIGlobalRef() = default;
	JNIGlobalRef(JNIEnv *jenv, T jref, jint jniVersion = JNI_VERSION_1_6) : version(jniVersion), ref(jref)
	{
		if (jenv) jenv->GetJavaVM(&vm);
	}
	JNIGlobalRef(JNIEnv *jenv, JNILocalRef<T> &&lref, jint jniVersion = JNI_VERSION_1_6)
		: JNIGlobalRef(New(jenv, lref.get(), jniVersion))
	{
		lref.reset();
	}
	~JNIGlobalRef()
	{
		reset();
	}

	JNIGlobalRef(const JNIGlobalRef &) = delete;
	JNIGlobalRef &operator=(const JNIGlobalRef &) = delete;

	JNIGlobalRef(JNIGlobalRef &&o) noexcept : vm(o.vm), version(o.version), ref(o.ref)
	{
		o.vm = nullptr;
		o.version = JNI_VERSION_1_6;
		o.ref = nullptr;
	}
	JNIGlobalRef &operator=(JNIGlobalRef &&o) noexcept
	{
		if (this != &o)
		{
			reset();
			vm = o.vm;
			version = o.version;
			ref = o.ref;
			o.vm = nullptr;
			o.version = JNI_VERSION_1_6;
			o.ref = nullptr;
		}
		return *this;
	}
	explicit operator bool() const
	{
		return ref != nullptr;
	}

	static JNIGlobalRef New(JNIEnv *jenv, T jref, jint jniVersion = JNI_VERSION_1_6)
	{
		if (!jenv || !jref) return JNIGlobalRef();
		return JNIGlobalRef(jenv, static_cast<T>(jenv->NewGlobalRef(jref)), jniVersion);
	}

	T get() const
	{
		return ref;
	}
	void reset()
	{
		if (vm && ref)
		{
			JNIEnv *env = nullptr;
			jint result = vm->GetEnv(reinterpret_cast<void **>(&env), version);
			if (result == JNI_OK) env->DeleteGlobalRef(ref);
		}
		vm = nullptr;
		version = JNI_VERSION_1_6;
		ref = nullptr;
	}
	T release()
	{
		T tpm = ref;
		vm = nullptr;
		version = JNI_VERSION_1_6;
		ref = nullptr;
		return tpm;
	}

private:
	JavaVM *vm = nullptr;
	jint version = JNI_VERSION_1_6;
	T ref = nullptr;
};

} // namespace EasyJni