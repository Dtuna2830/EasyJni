#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIException.h"
#include "environment/SubjectTest.h"

using namespace EasyJni;

using JNIMethodTest = SubjectTest;

TEST_F(JNIMethodTest, NotFound)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNIMethod<void>());
	EXPECT_FALSE((subject().getMethod<void, jint>(env, "setValue")));
	EXPECT_TRUE(JNIException::Check(env));
}

TEST_F(JNIMethodTest, Primitive)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto setValue = subject().getMethod<void, jdouble>(env, "setValue");
	auto getValue = subject().getMethod<jdouble>(env, "getValue");
	ASSERT_TRUE(setValue);
	ASSERT_TRUE(getValue);

	setValue.call(env, obj.get(), 2.5);
	EXPECT_EQ(getValue.call(env, obj.get()), 2.5);
}

TEST_F(JNIMethodTest, Object)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto repeat = subject().getMethod<jstring, jstring, jint>(env, "repeat");
	ASSERT_TRUE(repeat);

	JNIString text = JNIString::NewUtf8(env, "ab");
	JNIString result(env, repeat.call(env, obj.get(), text.get(), 3));
	std::string value;
	result.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "ababab");
}

TEST_F(JNIMethodTest, Static)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto round = subject().getStaticMethod<jlong, jdouble>(env, "round");
	ASSERT_TRUE(round);

	EXPECT_EQ(round.call(env, cls.get(), 2.6), 3);
}