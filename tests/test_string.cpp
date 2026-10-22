#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIException.h"

using namespace EasyJni;

TEST(JNIStringTest, Empty)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIString str;
	EXPECT_FALSE(str);
	EXPECT_EQ(str.length(env), 0);
	EXPECT_EQ(str.lengthUtf8(env), 0);
}

TEST(JNIStringTest, Utf8)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIString str = JNIString::NewUtf8(env, "Utf8 string");
	ASSERT_TRUE(str);
	EXPECT_EQ(str.lengthUtf8(env), 11);

	std::string value;
	str.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "Utf8 string");
}

TEST(JNIStringTest, Utf16)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	const jchar text[] = {'U', 't', 'f', '1', '6'};
	JNIString str = JNIString::New(env, text, 5);
	ASSERT_TRUE(str);
	EXPECT_EQ(str.length(env), 5);

	std::u16string value;
	str.chars(env, [&](const jchar *chars, jsize len)
	{
		value.assign(chars, chars + len);
	});
	EXPECT_EQ(value, u"Utf16");
}

TEST(JNIStringTest, Region)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIString str = JNIString::NewUtf8(env, "Region string");

	jchar buf[6] = {};
	str.region(env, 7, 6, buf);
	EXPECT_EQ(std::u16string(buf, buf + 6), u"string");

	char bufUtf8[7] = {};
	str.regionUtf8(env, 0, 6, bufUtf8);
	EXPECT_EQ(std::string(bufUtf8, 6), "Region");
}

TEST(JNIStringTest, OutOfBounds)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIString str = JNIString::NewUtf8(env, "Short");

	jchar buf[10] = {};
	str.region(env, 0, 10, buf);
	EXPECT_TRUE(JNIException::Check(env));
}

TEST(JNIStringTest, NonOwning)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNIString owner = JNIString::NewUtf8(env, "Non-owning string");
	{
		JNIString view = JNIString::From(owner.get());
		EXPECT_TRUE(view);
		EXPECT_EQ(view.get(), owner.get());
	}
	EXPECT_EQ(env->GetObjectRefType(owner.get()), JNILocalRefType);
}