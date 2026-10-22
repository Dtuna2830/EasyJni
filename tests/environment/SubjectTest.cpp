#include "SubjectTest.h"
#include "JvmEnvironment.h"

using namespace EasyJni;

void SubjectTest::SetUp()
{
	JNIEnv *env = jvmEnvironment->getEnv();
	cls = JNILocalRef<jclass>(env, JNIClass::Find<"Subject">(env).get());
	ASSERT_TRUE(cls);
	obj = JNILocalRef<jobject>(env, env->AllocObject(cls.get()));
	ASSERT_TRUE(obj);
}

JNIClass SubjectTest::subject() const
{
	return JNIClass::From(cls.get());
}