#pragma once
#include <exception>
#include <string>
#include "JNIClass.h"
#include "JNIString.h"

namespace EasyJni
{

class JNIException : public std::exception
{
public:
	JNIException() = default;
	explicit JNIException(std::string &&message) : msg(std::move(message)), exception(true)
	{
	}
	explicit JNIException(const std::string &message) : msg(message), exception(true)
	{
	}
	explicit JNIException(jthrowable throwable) : jthr(nullptr, throwable), exception(throwable != nullptr)
	{
	}
	explicit JNIException(JNIEnv *env) : JNIException(env, take(env))
	{
	}
	JNIException(JNIEnv *env, jthrowable throwable)
		: msg(message(env, throwable)), jthr(env, throwable), exception(throwable != nullptr)
	{
	}
	
	JNIException(const JNIException &o) : std::exception(o), msg(o.msg), jthr(o.jthr.clone()), exception(o.exception)
	{
	}
	JNIException(JNIException &&) noexcept = default;
	JNIException &operator=(const JNIException &o)
	{
		if (this != &o)
		{
			std::exception::operator=(o);
			msg = o.msg;
			jthr = o.jthr.clone();
			exception = o.exception;
		}
		return *this;
	}
	JNIException &operator=(JNIException &&) noexcept = default;

	explicit operator bool() const
	{
		return exception;
	}

	jthrowable get() const
	{
		return jthr.get();
	}

	jthrowable release()
	{
		return jthr.release();
	}

	const char *what() const noexcept override
	{
		return msg.c_str();
	}

	static JNIException Check(JNIEnv *env)
	{
		if (!env->ExceptionCheck()) return JNIException();
		jthrowable jt = env->ExceptionOccurred();
		env->ExceptionClear();
		return JNIException(env, jt);
	}

	static void CheckAndThrow(JNIEnv *env)
	{
		JNIException e = Check(env);
		if (e) throw e;
	}

	static jint ThrowJava(JNIEnv *env, jthrowable throwable)
	{
		return env->Throw(throwable);
	}

	jint throwNewJava(JNIEnv *env, jclass cls)
	{
		return env->ThrowNew(cls, msg.c_str());
	}

	template <FixedString ClassName>
	static jint ThrowNewJava(JNIEnv *env, const char *msg)
	{
		JNILocalRef<jclass> cls(env, JNIClass::Find<ClassName>(env).get());
		if (!cls) return -1;
		return env->ThrowNew(cls.get(), msg);
	}

	static void FatalError(JNIEnv *env, const char *msg)
	{
		env->FatalError(msg);
	}

protected:
	std::string msg;
	JNILocalRef<jthrowable> jthr;
	bool exception = false;

private:
	static jthrowable take(JNIEnv *env)
	{
		if (!env->ExceptionCheck()) return nullptr;
		jthrowable jt = env->ExceptionOccurred();
		env->ExceptionClear();
		return jt;
	}

	static std::string message(JNIEnv *env, jthrowable throwable)
	{
		if (throwable == nullptr) return "";
		auto throwableClass = JNIClass::Find<"java/lang/Throwable">(env);
		JNILocalRef<jclass> throwableClassR(env, throwableClass.get());
		if (!throwableClass) return "Unknown JNI exception";
		auto getMessageMethod = throwableClass.getMethod<jstring>(env, "getMessage");
		if (!getMessageMethod) return "Unknown JNI exception";
		jstring msg = getMessageMethod.call(env, throwable);
		if (env->ExceptionCheck())
		{
			env->ExceptionClear();
			return "Unknown JNI exception (java.lang.Throwable.getMessage() threw)";
		}
		std::string msgStr = "Unknown JNI exception";
		JNIString(env, msg).charsUtf8(env, [&](const char *chars, jsize len)
		{
			msgStr.assign(chars, len);
		});
		return msgStr;
	}
};

template <>
struct JNISignature<JNIException>
{
	static consteval auto Value()
	{
		return JNISignature<jthrowable>::Value();
	}
};

template <typename T>
concept IsJNIException = std::is_same_v<T, JNIException>;

} // namespace EasyJni