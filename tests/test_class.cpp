#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIException.h"

using namespace EasyJni;

TEST(JNIClassTest, Find)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jclass> cls(env, JNIClass::Find<"Subject">(env).get());
	EXPECT_TRUE(cls);
}

TEST(JNIClassTest, NotFound)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNIClass::Find<"Missing">(env));
	EXPECT_TRUE(JNIException::Check(env));
}

TEST(JNIClassTest, Empty)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIClass empty;
	EXPECT_FALSE(empty);
	EXPECT_FALSE(empty.getMethod<void>(env, "getValue"));
	EXPECT_FALSE(empty.getStaticMethod<void>(env, "round"));
	EXPECT_FALSE(empty.getField<jint>(env, "value"));
	EXPECT_FALSE(empty.getStaticField<jint>(env, "count"));
	EXPECT_FALSE(JNIException::Check(env));
}