#pragma once
#include "EasyJni/JNIMethodMirror.h"
#include "SubjectParent.h"

class SubjectInterface : public EasyJni::JNIClassMirror<"SubjectInterface">
{
public:
	using JNIClassMirror::JNIClassMirror;

	EasyJni::JNIMethodMirror<"join", EasyJni::JNIString, EasyJni::JNIArray<EasyJni::JNIString>> join{this};
	EasyJni::JNIMethodMirror<"parents", EasyJni::JNIArray<SubjectParent>, jint> parents{this};
};