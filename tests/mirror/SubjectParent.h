#pragma once
#include "EasyJni/JNIMethodMirror.h"
#include "EasyJni/JNIFieldMirror.h"
#include "SubjectParentInterface.h"

class SubjectParent : public EasyJni::JNIClassMirror<"SubjectParent", EasyJni::Implements<SubjectParentInterface>,
													 EasyJni::Constructors<jint>>
{
public:
	using JNIClassMirror::JNIClassMirror;

	EasyJni::JNIFieldMirror<"parentValues", EasyJni::JNIArray<jint>> parentValues{this};
	EasyJni::JNIFieldMirror<"next", SubjectParent> next{this};
	EasyJni::JNIMethodMirror<"getParentValue", jint> getParentValue{this};
	EasyJni::JNIStaticMethodMirror<"message", EasyJni::JNIString, EasyJni::JNIException> message{this};
};