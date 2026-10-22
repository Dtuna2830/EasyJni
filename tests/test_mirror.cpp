#include "environment/JvmEnvironment.h"
#include "mirror/Subject.h"

using namespace EasyJni;

// Constructors

TEST(JNIMirrorTest, Constructor)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject empty(env);
	EXPECT_EQ(empty.getValue(), 0.0);

	JNIString name = JNIString::NewUtf8(env, "Constructed subject");
	Subject subject(env, jdouble{2.5}, name);
	JNIString result = subject.name;
	EXPECT_EQ(subject.getValue(), 2.5);
	EXPECT_TRUE(env->IsSameObject(result.get(), name.get()));
}

TEST(JNIMirrorTest, MirrorConstructor)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	SubjectParent parent(env, jint{7});
	Subject subject(env, parent);
	EXPECT_EQ(subject.getParentValue(), 7);
}

// Methods

TEST(JNIMirrorTest, Method)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	subject.setValue(jdouble{2.5});
	EXPECT_EQ(subject.getValue(), 2.5);

	JNIString result = subject.repeat(JNIString::NewUtf8(env, "ab"), jint{3});
	std::string value;
	result.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "ababab");
}

TEST(JNIMirrorTest, StaticMethod)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	EXPECT_EQ(subject.round(jdouble{2.6}), 3);
}

TEST(JNIMirrorTest, MirrorMethod)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject low(env);
	Subject high(env);
	low.setValue(jdouble{1.0});
	high.setValue(jdouble{2.0});

	Subject result = low.max(high);
	EXPECT_EQ(result.getValue(), 2.0);
}

TEST(JNIMirrorTest, ArrayMethod)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	JNIArray<jint> values = subject.range(jint{4});
	EXPECT_EQ(values.size(env), 4);
	EXPECT_EQ(subject.sum(values), 6);
}

TEST(JNIMirrorTest, ObjectArrayMethod)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	auto parts = JNIArray<JNIString>::New(env, 2);
	parts.setElement(env, 0, JNIString::NewUtf8(env, "a"));
	parts.setElement(env, 1, JNIString::NewUtf8(env, "b"));

	JNIString joined = subject.join(parts);
	std::string value;
	joined.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "a,b");

	JNIArray<SubjectParent> parents = subject.parents(jint{2});
	EXPECT_EQ(parents.size(env), 2);
	EXPECT_EQ(parents.getElement(env, 1).getParentValue(), 1);
}

TEST(JNIMirrorTest, ExceptionMethod)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	JNIException error = subject.error(JNIString::NewUtf8(env, "Returned exception"));
	EXPECT_STREQ(error.what(), "Returned exception");

	JNIString message = subject.message(error);
	std::string value;
	message.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "Returned exception");
}

TEST(JNIMirrorTest, Throw)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	EXPECT_THROW(subject.fail(JNIString::NewUtf8(env, "Thrown exception")), JNIException);
	EXPECT_THROW(subject.failStatic(JNIString::NewUtf8(env, "Thrown static exception")), JNIException);
	EXPECT_FALSE(JNIException::Check(env));
}

// Fields

TEST(JNIMirrorTest, Field)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	JNIString name = JNIString::NewUtf8(env, "Field name");
	subject.value = 4.0;
	subject.name = name;

	jdouble value = subject.value;
	JNIString result = subject.name;
	EXPECT_EQ(value, 4.0);
	EXPECT_TRUE(env->IsSameObject(result.get(), name.get()));
}

TEST(JNIMirrorTest, StaticField)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	subject.count = 3;
	jint count = subject.count;
	EXPECT_EQ(count, 3);

	JNIString name = subject.PARENT_INTERFACE_NAME;
	std::string value;
	name.charsUtf8(env, [&](const char *chars, jsize len)
	{
		value.assign(chars, len);
	});
	EXPECT_EQ(value, "SubjectParentInterface");
}

TEST(JNIMirrorTest, ArrayField)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	auto values = JNIArray<jint>::New(env, 2);
	values.setElement(env, 1, 5);
	subject.parentValues = values;

	JNIArray<jint> result = subject.parentValues;
	EXPECT_EQ(result.getElement(env, 1), 5);
}

TEST(JNIMirrorTest, MirrorField)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	Subject subject(env);
	SubjectParent parent(env, jint{7});
	subject.next = parent;

	SubjectParent next = subject.next;
	EXPECT_EQ(next.getParentValue(), 7);
}