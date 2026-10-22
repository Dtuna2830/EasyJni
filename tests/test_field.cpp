#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIException.h"
#include "environment/SubjectTest.h"

using namespace EasyJni;

using JNIFieldTest = SubjectTest;

TEST_F(JNIFieldTest, NotFound)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNIField<jint>());
	EXPECT_FALSE(subject().getField<jint>(env, "value"));
	EXPECT_TRUE(JNIException::Check(env));
}

TEST_F(JNIFieldTest, Primitive)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto value = subject().getField<jdouble>(env, "value");
	ASSERT_TRUE(value);

	value.set(env, obj.get(), 2.5);
	EXPECT_EQ(value.get(env, obj.get()), 2.5);
}

TEST_F(JNIFieldTest, Object)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto name = subject().getField<jstring>(env, "name");
	ASSERT_TRUE(name);

	JNIString text = JNIString::NewUtf8(env, "ab");
	name.set(env, obj.get(), text.get());
	JNIString result(env, name.get(env, obj.get()));
	EXPECT_TRUE(env->IsSameObject(result.get(), text.get()));
}

TEST_F(JNIFieldTest, Static)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto count = subject().getStaticField<jint>(env, "count");
	ASSERT_TRUE(count);

	count.set(env, cls.get(), 3);
	EXPECT_EQ(count.get(env, cls.get()), 3);
}