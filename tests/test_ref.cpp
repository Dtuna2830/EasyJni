#include "environment/JvmEnvironment.h"
#include "EasyJni/JNIRef.h"

using namespace EasyJni;

// JNILocalRef

TEST(JNILocalRefTest, Empty)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNILocalRef<jobject>());
	EXPECT_FALSE(JNILocalRef<jobject>::New(env, nullptr));
}

TEST(JNILocalRefTest, New)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> original(env, env->NewStringUTF("New local ref"));
	JNILocalRef<jobject> ref = JNILocalRef<jobject>::New(env, original.get());
	ASSERT_TRUE(ref);
	EXPECT_NE(ref.get(), original.get());
	EXPECT_TRUE(env->IsSameObject(ref.get(), original.get()));
	EXPECT_EQ(env->GetObjectRefType(ref.get()), JNILocalRefType);
}

TEST(JNILocalRefTest, Move)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> a(env, env->NewStringUTF("Moved local ref"));
	jobject raw = a.get();

	JNILocalRef<jobject> b(std::move(a));
	EXPECT_FALSE(a);
	EXPECT_EQ(b.get(), raw);

	a = std::move(b);
	EXPECT_FALSE(b);
	EXPECT_EQ(a.get(), raw);
}

TEST(JNILocalRefTest, Clone)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> owner(env, env->NewStringUTF("Cloned local ref"));
	JNILocalRef<jobject> copy = owner.clone();
	EXPECT_NE(copy.get(), owner.get());
	EXPECT_TRUE(env->IsSameObject(copy.get(), owner.get()));

	JNILocalRef<jobject> view(nullptr, owner.get());
	EXPECT_EQ(view.clone().get(), owner.get());
}

TEST(JNILocalRefTest, Release)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> ref(env, env->NewStringUTF("Released local ref"));
	jobject raw = ref.release();
	EXPECT_FALSE(ref);
	EXPECT_EQ(env->GetObjectRefType(raw), JNILocalRefType);
	env->DeleteLocalRef(raw);
}

// JNIGlobalRef

TEST(JNIGlobalRefTest, Empty)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	EXPECT_FALSE(JNIGlobalRef<jobject>());
	EXPECT_FALSE(JNIGlobalRef<jobject>::New(env, nullptr));
}

TEST(JNIGlobalRefTest, New)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> local(env, env->NewStringUTF("New global ref"));
	JNIGlobalRef<jobject> ref = JNIGlobalRef<jobject>::New(env, local.get());
	ASSERT_TRUE(ref);
	EXPECT_TRUE(env->IsSameObject(ref.get(), local.get()));
	EXPECT_EQ(env->GetObjectRefType(ref.get()), JNIGlobalRefType);
}

TEST(JNIGlobalRefTest, FromLocal)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> local(env, env->NewStringUTF("Promoted global ref"));
	JNIGlobalRef<jobject> ref(env, std::move(local));
	EXPECT_FALSE(local);
	ASSERT_TRUE(ref);
	EXPECT_EQ(env->GetObjectRefType(ref.get()), JNIGlobalRefType);
}

TEST(JNIGlobalRefTest, Move)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> local(env, env->NewStringUTF("Moved global ref"));
	JNIGlobalRef<jobject> a = JNIGlobalRef<jobject>::New(env, local.get());
	jobject raw = a.get();

	JNIGlobalRef<jobject> b(std::move(a));
	EXPECT_FALSE(a);
	EXPECT_EQ(b.get(), raw);

	a = std::move(b);
	EXPECT_FALSE(b);
	EXPECT_EQ(a.get(), raw);
}

TEST(JNIGlobalRefTest, Release)
{
	JNIEnv *env = jvmEnvironment->getEnv();
	JNILocalRef<jobject> local(env, env->NewStringUTF("Released global ref"));
	JNIGlobalRef<jobject> ref = JNIGlobalRef<jobject>::New(env, local.get());
	jobject raw = ref.release();
	EXPECT_FALSE(ref);
	EXPECT_EQ(env->GetObjectRefType(raw), JNIGlobalRefType);
	env->DeleteGlobalRef(raw);
}