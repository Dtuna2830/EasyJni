#pragma once
#include "EasyJni/JNIMethodMirror.h"
#include "EasyJni/JNIFieldMirror.h"
#include "SubjectParent.h"
#include "SubjectInterface.h"

class Subject
	: public EasyJni::JNIClassMirror<"Subject", EasyJni::Extends<SubjectParent>, EasyJni::Implements<SubjectInterface>,
									 EasyJni::Constructors<>, EasyJni::Constructors<jdouble, EasyJni::JNIString>,
									 EasyJni::Constructors<SubjectParent>>
{
public:
	using JNIClassMirror::JNIClassMirror;

	EasyJni::JNIStaticFieldMirror<"count", jint> count{this};
	EasyJni::JNIFieldMirror<"value", jdouble> value{this};
	EasyJni::JNIFieldMirror<"name", EasyJni::JNIString> name{this};

	EasyJni::JNIMethodMirror<"setValue", void, jdouble> setValue{this};
	EasyJni::JNIMethodMirror<"getValue", jdouble> getValue{this};
	EasyJni::JNIMethodMirror<"max", Subject, Subject> max{this};
	EasyJni::JNIMethodMirror<"repeat", EasyJni::JNIString, EasyJni::JNIString, jint> repeat{this};
	EasyJni::JNIMethodMirror<"fail", void, EasyJni::JNIString> fail{this};

	EasyJni::JNIStaticMethodMirror<"round", jlong, jdouble> round{this};
	EasyJni::JNIStaticMethodMirror<"range", EasyJni::JNIArray<jint>, jint> range{this};
	EasyJni::JNIStaticMethodMirror<"failStatic", void, EasyJni::JNIString> failStatic{this};
	EasyJni::JNIStaticMethodMirror<"error", EasyJni::JNIException, EasyJni::JNIString> error{this};
};