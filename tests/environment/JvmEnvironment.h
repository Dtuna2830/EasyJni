#pragma once
#include <gtest/gtest.h>
#include <jni.h>

class JvmEnvironment : public ::testing::Environment
{
public:
	void SetUp() override;
	void TearDown() override;

	JavaVM *getJvm() const;
	JNIEnv *getEnv() const;

private:
	JavaVM *jvm = nullptr;
};

extern JvmEnvironment *jvmEnvironment;