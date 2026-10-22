#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIException.h"
#include "environment/SubjectTest.h"

using namespace EasyJni;

using JNIExceptionTest = SubjectTest;

TEST_F(JNIExceptionTest, None)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNIException::Check(env));
	EXPECT_NO_THROW(JNIException::CheckAndThrow(env));
}

TEST_F(JNIExceptionTest, Message)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto fail = subject().getMethod<void, jstring>(env, "fail");
	ASSERT_TRUE(fail);

	JNIString message = JNIString::NewUtf8(env, "Instance method exception");
	fail.call(env, obj.get(), message.get());
	JNIException e = JNIException::Check(env);
	EXPECT_TRUE(e);
	EXPECT_TRUE(e.get());
	EXPECT_STREQ(e.what(), "Instance method exception");
	EXPECT_FALSE(JNIException::Check(env));
}

TEST_F(JNIExceptionTest, NullMessage)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto fail = subject().getMethod<void, jstring>(env, "fail");
	ASSERT_TRUE(fail);

	fail.call(env, obj.get(), nullptr);
	EXPECT_STREQ(JNIException::Check(env).what(), "Unknown JNI exception");
}

TEST_F(JNIExceptionTest, CheckAndThrow)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto failStatic = subject().getStaticMethod<void, jstring>(env, "failStatic");
	ASSERT_TRUE(failStatic);

	JNIString message = JNIString::NewUtf8(env, "Static method exception");
	failStatic.call(env, cls.get(), message.get());
	EXPECT_THROW(JNIException::CheckAndThrow(env), JNIException);
	EXPECT_FALSE(JNIException::Check(env));
}

TEST_F(JNIExceptionTest, ThrowJava)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto error = subject().getStaticMethod<jthrowable, jstring>(env, "error");
	ASSERT_TRUE(error);

	JNIString message = JNIString::NewUtf8(env, "Returned Java exception");
	JNIException returned(env, error.call(env, cls.get(), message.get()));
	ASSERT_TRUE(returned);
	EXPECT_STREQ(returned.what(), "Returned Java exception");
	ASSERT_EQ(JNIException::ThrowJava(env, returned.get()), JNI_OK);
	EXPECT_STREQ(JNIException::Check(env).what(), "Returned Java exception");
}

TEST_F(JNIExceptionTest, NonOwning)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto error = subject().getStaticMethod<jthrowable, jstring>(env, "error");
	ASSERT_TRUE(error);

	JNIString message = JNIString::NewUtf8(env, "Non-owning Java exception");
	JNIException owner(env, error.call(env, cls.get(), message.get()));
	JNIException view(owner.get());
	EXPECT_TRUE(view);
	EXPECT_EQ(view.get(), owner.get());
	EXPECT_STREQ(view.what(), "");
}

TEST_F(JNIExceptionTest, Copy)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto error = subject().getStaticMethod<jthrowable, jstring>(env, "error");
	ASSERT_TRUE(error);

	JNIString message = JNIString::NewUtf8(env, "Copied Java exception");
	JNIException original(env, error.call(env, cls.get(), message.get()));
	JNIException copy(original);
	original = JNIException();
	EXPECT_STREQ(copy.what(), "Copied Java exception");
	ASSERT_EQ(JNIException::ThrowJava(env, copy.get()), JNI_OK);
	EXPECT_STREQ(JNIException::Check(env).what(), "Copied Java exception");
}

TEST_F(JNIExceptionTest, ThrowNewJava)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	ASSERT_EQ(JNIException::ThrowNewJava<"java/lang/RuntimeException">(env, "New Java exception"), JNI_OK);
	EXPECT_STREQ(JNIException::Check(env).what(), "New Java exception");
}

TEST_F(JNIExceptionTest, ClassNotFound)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_EQ(JNIException::ThrowNewJava<"Missing">(env, "Missing class exception"), -1);
	EXPECT_TRUE(JNIException::Check(env));
}

TEST_F(JNIExceptionTest, Rethrow)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jclass> runtimeException(env, JNIClass::Find<"java/lang/RuntimeException">(env).get());
	ASSERT_TRUE(runtimeException);

	ASSERT_EQ(JNIException("Rethrown C++ exception").throwNewJava(env, runtimeException.get()), JNI_OK);
	EXPECT_STREQ(JNIException::Check(env).what(), "Rethrown C++ exception");
}