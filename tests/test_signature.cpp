#include <gtest/gtest.h>
#include "EasyJni/JNIException.h"
#include "EasyJni/JNIClassMirror.h"
#include "mirror/Subject.h"

using namespace EasyJni;

// FixedString

TEST(FixedStringTest, Substr)
{
	constexpr auto str = FixedString("Ljava/lang/String;");
	EXPECT_EQ((str.substr<1, 16>().view()), "java/lang/String");
	EXPECT_EQ((str.substr<0, 0>().view()), "");
}

// JNISignature

TEST(JNISignatureTest, Primitive)
{
	EXPECT_EQ(JNISignature<void>::Value().view(), "V");
	EXPECT_EQ(JNISignature<jboolean>::Value().view(), "Z");
	EXPECT_EQ(JNISignature<jbyte>::Value().view(), "B");
	EXPECT_EQ(JNISignature<jchar>::Value().view(), "C");
	EXPECT_EQ(JNISignature<jshort>::Value().view(), "S");
	EXPECT_EQ(JNISignature<jint>::Value().view(), "I");
	EXPECT_EQ(JNISignature<jlong>::Value().view(), "J");
	EXPECT_EQ(JNISignature<jfloat>::Value().view(), "F");
	EXPECT_EQ(JNISignature<jdouble>::Value().view(), "D");
}

TEST(JNISignatureTest, Object)
{
	EXPECT_EQ(JNISignature<jobject>::Value().view(), "Ljava/lang/Object;");
	EXPECT_EQ(JNISignature<jstring>::Value().view(), "Ljava/lang/String;");
	EXPECT_EQ(JNISignature<jclass>::Value().view(), "Ljava/lang/Class;");
	EXPECT_EQ(JNISignature<jthrowable>::Value().view(), "Ljava/lang/Throwable;");
	EXPECT_EQ(JNISignature<JNIString>::Value().view(), "Ljava/lang/String;");
	EXPECT_EQ(JNISignature<JNIException>::Value().view(), "Ljava/lang/Throwable;");
}

struct Custom;
REGISTER_JNI_OBJECT_SIGNATURE(Custom, "com/example/Custom")
TEST(JNISignatureTest, CustomObject)
{
	EXPECT_EQ(JNISignature<Custom>::Value().view(), "Lcom/example/Custom;");
}

TEST(JNISignatureTest, ClassMirror)
{
	EXPECT_EQ(JNISignature<SubjectParentInterface>::Value().view(), "LSubjectParentInterface;");
	EXPECT_EQ(JNISignature<Subject>::Value().view(), "LSubject;");
}

// JNIArraySignature and JNISignature<JNIArray<T>>

TEST(JNIArraySignatureTest, Primitive)
{
	EXPECT_EQ(JNIArraySignature<jint>::Value().view(), "[I");
	EXPECT_EQ(JNISignature<JNIArray<jboolean>>::Value().view(), "[Z");
}

TEST(JNIArraySignatureTest, Multidimensional)
{
	EXPECT_EQ(JNIArraySignature<JNIArray<jint>>::Value().view(), "[[I");
	EXPECT_EQ(JNISignature<JNIArray<JNIArray<JNIArray<jdouble>>>>::Value().view(), "[[[D");
}

// JNIMethodSignature

TEST(JNIMethodSignatureTest, NoArguments)
{
	EXPECT_EQ(JNIMethodSignature<void>::Value().view(), "()V");
	EXPECT_EQ(JNIMethodSignature<jint>::Value().view(), "()I");
}

TEST(JNIMethodSignatureTest, SingleArgument)
{
	EXPECT_EQ((JNIMethodSignature<void, jint>::Value().view()), "(I)V");
}

TEST(JNIMethodSignatureTest, MultipleArguments)
{
	EXPECT_EQ((JNIMethodSignature<jboolean, jint, jdouble>::Value().view()), "(ID)Z");
}

TEST(JNIMethodSignatureTest, ClassMirror)
{
	EXPECT_EQ((JNIMethodSignature<Subject, JNIArray<SubjectParent>>::Value().view()), "([LSubjectParent;)LSubject;");
}