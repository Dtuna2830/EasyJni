#pragma once
#include "JNISignature.h"
#include "JNIRef.h"

namespace EasyJni
{

class JNIString
{
public:
	JNIString() = default;
	explicit JNIString(jstring str) : jstr(nullptr, str)
	{
	}
	JNIString(JNIEnv *env, jstring str) : jstr(env, str)
	{
	}

	explicit operator bool() const
	{
		return static_cast<bool>(jstr);
	}

	jstring get() const
	{
		return jstr.get();
	}

	jstring release()
	{
		return jstr.release();
	}

	jsize length(JNIEnv *env) const // utf16
	{
		if (!jstr) return 0;
		return env->GetStringLength(jstr.get());
	}

	jsize lengthUtf8(JNIEnv *env) const
	{
		if (!jstr) return 0;
		return env->GetStringUTFLength(jstr.get());
	}

	static JNIString From(jstring str)
	{
		return JNIString(str);
	}

	static JNIString From(JNIEnv *env, jstring str)
	{
		return JNIString(env, str);
	}

	static JNIString New(JNIEnv *env, const jchar *unicodeChars, jsize len) // utf16
	{
		return JNIString(env, env->NewString(unicodeChars, len));
	}

	static JNIString NewUtf8(JNIEnv *env, const char *bytes)
	{
		return JNIString(env, env->NewStringUTF(bytes));
	}

	template <typename Func>
	void chars(JNIEnv *env, Func &&func)
	{
		if (!jstr) return;
		const jchar *ptr = env->GetStringChars(jstr.get(), nullptr); // dont care
		if (ptr == nullptr) return;
		try
		{
			func(ptr, env->GetStringLength(jstr.get()));
		}
		catch (...)
		{
			env->ReleaseStringChars(jstr.get(), ptr);
			throw;
		}
		env->ReleaseStringChars(jstr.get(), ptr);
	}

	template <typename Func>
	void charsUtf8(JNIEnv *env, Func &&func)
	{
		if (!jstr) return;
		const char *ptr = env->GetStringUTFChars(jstr.get(), nullptr);
		if (ptr == nullptr) return;
		try
		{
			func(ptr, env->GetStringUTFLength(jstr.get()));
		}
		catch (...)
		{
			env->ReleaseStringUTFChars(jstr.get(), ptr);
			throw;
		}
		env->ReleaseStringUTFChars(jstr.get(), ptr);
	}

	void region(JNIEnv *env, jsize start, jsize len, jchar *buf)
	{
		if (!jstr) return;
		env->GetStringRegion(jstr.get(), start, len, buf);
	}

	void regionUtf8(JNIEnv *env, jsize start, jsize len, char *buf)
	{
		if (!jstr) return;
		env->GetStringUTFRegion(jstr.get(), start, len, buf);
	}

private:
	JNILocalRef<jstring> jstr;
};

template <>
struct JNISignature<JNIString>
{
	static consteval auto Value()
	{
		return JNISignature<jstring>::Value();
	}
};

template <typename T>
concept IsJNIString = std::is_same_v<T, JNIString>;

} // namespace EasyJni