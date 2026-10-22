#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIArray.h"
#include "EasyJni/JNIException.h"

using namespace EasyJni;

TEST(JNIArrayTest, Empty)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIArray<jint> array;
	EXPECT_FALSE(array);
	EXPECT_EQ(array.size(env), 0);
}

TEST(JNIArrayTest, Primitive)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto array = JNIArray<jint>::New(env, 3);
	ASSERT_TRUE(array);
	EXPECT_EQ(array.size(env), 3);

	array.setElement(env, 1, 42);
	EXPECT_EQ(array.getElement(env, 1), 42);
}

TEST(JNIArrayTest, Elements)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto array = JNIArray<jint>::New(env, 3);
	ASSERT_TRUE(array);

	array.elements(env, [](jint *elems, jsize len)
	{
		for (jsize i = 0; i < len; i++)
			elems[i] = i * 2;
	});
	EXPECT_EQ(array.getElement(env, 0), 0);
	EXPECT_EQ(array.getElement(env, 1), 2);
	EXPECT_EQ(array.getElement(env, 2), 4);
}

TEST(JNIArrayTest, Object)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto array = JNIArray<JNIString>::New(env, 2);
	ASSERT_TRUE(array);
	EXPECT_EQ(array.size(env), 2);

	array.setElement(env, 0, JNIString::NewUtf8(env, "Array element"));
	JNIString result = array.getElement(env, 0);
	std::string value;
	result.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "Array element");
	EXPECT_FALSE(array.getElement(env, 1));
}

TEST(JNIArrayTest, Multidimensional)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto array = JNIArray<JNIArray<jint>>::New(env, 2);
	ASSERT_TRUE(array);

	auto inner = JNIArray<jint>::New(env, 1);
	inner.setElement(env, 0, 7);
	array.setElement(env, 1, inner);
	EXPECT_EQ(array.getElement(env, 1).getElement(env, 0), 7);
}

TEST(JNIArrayTest, OutOfBounds)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	auto array = JNIArray<jint>::New(env, 3);
	ASSERT_TRUE(array);

	array.getElement(env, 3);
	EXPECT_TRUE(JNIException::Check(env));
}