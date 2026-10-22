#pragma once
#include "EasyJni/JNIMethodMirror.h"
#include "EasyJni/JNIFieldMirror.h"

class SubjectParentInterface : public EasyJni::JNIClassMirror<"SubjectParentInterface">
{
public:
	using JNIClassMirror::JNIClassMirror;
	
	EasyJni::JNIStaticFieldMirror<"PARENT_INTERFACE_NAME", EasyJni::JNIString> PARENT_INTERFACE_NAME{this};
	EasyJni::JNIMethodMirror<"sum", jint, EasyJni::JNIArray<jint>> sum{this};
};