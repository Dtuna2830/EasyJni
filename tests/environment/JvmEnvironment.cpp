#include "JvmEnvironment.h"

JvmEnvironment *jvmEnvironment = new JvmEnvironment();

void JvmEnvironment::SetUp()
{
	JavaVMOption option;
	option.optionString = const_cast<char *>("-Djava.class.path=" EASYJNI_TESTCLASSPATH);
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_6;
	vm_args.nOptions = 1;
	vm_args.options = &option;
	vm_args.ignoreUnrecognized = JNI_FALSE;

	JNIEnv *trash = nullptr;
	jint res = JNI_CreateJavaVM(&jvm, reinterpret_cast<void **>(&trash), &vm_args);
	if (res != JNI_OK)
	{
		throw std::runtime_error("Failed to create JVM environment.");
	}
}

void JvmEnvironment::TearDown()
{
	if (jvm)
	{
		jvm->DestroyJavaVM();
		jvm = nullptr;
	}
}

JavaVM *JvmEnvironment::getJvm() const
{
	return jvm;
}

JNIEnv *JvmEnvironment::getEnv() const
{
	JNIEnv *env = nullptr;
	jint res = jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
	if (res == JNI_OK) return env;
	if (res != JNI_EDETACHED) return nullptr;

	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6;
	args.name = nullptr;
	args.group = nullptr;
	res = jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), &args);
	if (res != JNI_OK) return nullptr;

	struct DetachMe
	{
		JavaVM *jvm;
		DetachMe(JavaVM *jvm) : jvm(jvm)
		{
		}
		~DetachMe()
		{
			jvm->DetachCurrentThread();
		}
	};
	thread_local DetachMe detachMe(jvm);
	return env;
}