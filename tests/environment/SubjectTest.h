#pragma once
#include <gtest/gtest.h>
#include "EasyJni/JNIClass.h"
#include "EasyJni/JNIRef.h"

class SubjectTest : public ::testing::Test
{
protected:
	void SetUp() override;

	EasyJni::JNIClass subject() const;
	
	EasyJni::JNILocalRef<jclass> cls;
	EasyJni::JNILocalRef<jobject> obj;
};