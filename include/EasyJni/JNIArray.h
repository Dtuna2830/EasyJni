#pragma once
#include "JNIException.h"

namespace EasyJni
{

template <typename T>
class JNIArray;

class JNIClassMirrorBase;

template <typename T>
struct JNIArrayTrait : std::false_type
{
};

template <typename T>
struct JNIArrayTrait<JNIArray<T>> : std::true_type
{
};

template <typename T>
concept IsJNIArray = JNIArrayTrait<T>::value;

template <typename T>
class JNIArray
{
public:
	JNIArray() = default;
	explicit JNIArray(jarray array) : ary(nullptr, array)
	{
	}
	JNIArray(JNIEnv *env, jarray array) : ary(env, array)
	{
	}

	explicit operator bool() const
	{
		return static_cast<bool>(ary);
	}

	jarray get() const
	{
		return ary.get();
	}

	jarray release()
	{
		return ary.release();
	}

	jsize size(JNIEnv *env) const
	{
		if (!ary) return 0;
		return env->GetArrayLength(ary.get());
	}

	static JNIArray New(JNIEnv *env, jsize len)
	{
		// clang-format off
		if constexpr (std::is_same_v<T, jboolean>) return JNIArray(env, env->NewBooleanArray(len));
		else if constexpr (std::is_same_v<T, jbyte>) return JNIArray(env, env->NewByteArray(len));
		else if constexpr (std::is_same_v<T, jchar>) return JNIArray(env, env->NewCharArray(len));
		else if constexpr (std::is_same_v<T, jshort>) return JNIArray(env, env->NewShortArray(len));
		else if constexpr (std::is_same_v<T, jint>) return JNIArray(env, env->NewIntArray(len));
		else if constexpr (std::is_same_v<T, jlong>) return JNIArray(env, env->NewLongArray(len));
		else if constexpr (std::is_same_v<T, jfloat>) return JNIArray(env, env->NewFloatArray(len));
		else if constexpr (std::is_same_v<T, jdouble>) return JNIArray(env, env->NewDoubleArray(len));
		// clang-format on
		else
		{
			constexpr auto name = ElementClassName();
			JNILocalRef<jclass> cls(env, env->FindClass(name.data));
			if (!cls) return JNIArray();
			return JNIArray(env, env->NewObjectArray(len, cls.get(), nullptr));
		}
	}

	template <typename Func>
	void elements(JNIEnv *env, Func &&func)
	{
		static_assert(IsJNIPrimitive<T>, "elements() supports only primitive arrays");
		T *ptr = getElements(env);
		try
		{
			func(ptr, size(env));
		}
		catch (...)
		{
			releaseElements(env, ptr, JNI_ABORT);
			throw;
		}
		releaseElements(env, ptr, 0);
	}

	T getElement(JNIEnv *env, jsize index)
	{
		if constexpr (IsJNIArray<T>)
			return T(env, static_cast<jarray>(env->GetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index)));
		else if constexpr (IsJNIString<T>)
			return T(env,
					 static_cast<jstring>(env->GetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index)));
		else if constexpr (IsJNIException<T>)
			return T(env,
					 static_cast<jthrowable>(env->GetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index)));
		else if constexpr (std::is_base_of_v<JNIClassMirrorBase, T>)
		{
			JNILocalRef<jobject> local(env, env->GetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index));
			return T(env, local.get());
		}
		else if constexpr (std::is_convertible_v<T, jobject>)
			return static_cast<T>(env->GetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index));
		else
		{
			T value{};
			// clang-format off
			if constexpr (std::is_same_v<T, jboolean>) env->GetBooleanArrayRegion(static_cast<jbooleanArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jbyte>) env->GetByteArrayRegion(static_cast<jbyteArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jchar>) env->GetCharArrayRegion(static_cast<jcharArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jshort>) env->GetShortArrayRegion(static_cast<jshortArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jint>) env->GetIntArrayRegion(static_cast<jintArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jlong>) env->GetLongArrayRegion(static_cast<jlongArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jfloat>) env->GetFloatArrayRegion(static_cast<jfloatArray>(ary.get()), index, 1, &value);
			else if constexpr (std::is_same_v<T, jdouble>) env->GetDoubleArrayRegion(static_cast<jdoubleArray>(ary.get()), index, 1, &value);
			else static_assert(always_false<T>, "Unsupported type");
			// clang-format on
			return value;
		}
	}

	void setElement(JNIEnv *env, jsize index, const T &value)
	{
		// clang-format off
		if constexpr (IsJNIArray<T> || IsJNIString<T> || IsJNIException<T>) env->SetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index, value.get());
		else if constexpr (std::is_base_of_v<JNIClassMirrorBase, T>) env->SetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index, value.object());
		else if constexpr (std::is_convertible_v<T, jobject>) env->SetObjectArrayElement(static_cast<jobjectArray>(ary.get()), index, value);
		else if constexpr (std::is_same_v<T, jboolean>) env->SetBooleanArrayRegion(static_cast<jbooleanArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jbyte>) env->SetByteArrayRegion(static_cast<jbyteArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jchar>) env->SetCharArrayRegion(static_cast<jcharArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jshort>) env->SetShortArrayRegion(static_cast<jshortArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jint>) env->SetIntArrayRegion(static_cast<jintArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jlong>) env->SetLongArrayRegion(static_cast<jlongArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jfloat>) env->SetFloatArrayRegion(static_cast<jfloatArray>(ary.get()), index, 1, &value);
		else if constexpr (std::is_same_v<T, jdouble>) env->SetDoubleArrayRegion(static_cast<jdoubleArray>(ary.get()), index, 1, &value);
		else static_assert(always_false<T>, "Unsupported type");
		// clang-format on
	}

	T getElementPtr(JNIEnv *env, jsize index)
	{
		if constexpr (!IsJNIPrimitive<T>) return getElement(env, index);
		else
		{
			T *elements = getElements(env);
			T value = elements[index];
			releaseElements(env, elements, JNI_ABORT);
			return value;
		}
	}

	void setElementPtr(JNIEnv *env, jsize index, const T &value)
	{
		if constexpr (!IsJNIPrimitive<T>) setElement(env, index, value);
		else
		{
			T *elements = getElements(env);
			elements[index] = value;
			releaseElements(env, elements, 0);
		}
	}

	T *getElements(JNIEnv *env)
	{
		// clang-format off
		if constexpr (std::is_same_v<T, jboolean>) return env->GetBooleanArrayElements(static_cast<jbooleanArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jbyte>) return env->GetByteArrayElements(static_cast<jbyteArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jchar>) return env->GetCharArrayElements(static_cast<jcharArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jshort>) return env->GetShortArrayElements(static_cast<jshortArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jint>) return env->GetIntArrayElements(static_cast<jintArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jlong>) return env->GetLongArrayElements(static_cast<jlongArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jfloat>) return env->GetFloatArrayElements(static_cast<jfloatArray>(ary.get()), nullptr);
		else if constexpr (std::is_same_v<T, jdouble>) return env->GetDoubleArrayElements(static_cast<jdoubleArray>(ary.get()), nullptr);
		else static_assert(always_false<T>, "Unsupported type");
		// clang-format on
	}

	void releaseElements(JNIEnv *env, T *elems, jint mode = 0)
	{
		// clang-format off
		if constexpr (std::is_same_v<T, jboolean>) env->ReleaseBooleanArrayElements(static_cast<jbooleanArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jbyte>) env->ReleaseByteArrayElements(static_cast<jbyteArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jchar>) env->ReleaseCharArrayElements(static_cast<jcharArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jshort>) env->ReleaseShortArrayElements(static_cast<jshortArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jint>) env->ReleaseIntArrayElements(static_cast<jintArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jlong>) env->ReleaseLongArrayElements(static_cast<jlongArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jfloat>) env->ReleaseFloatArrayElements(static_cast<jfloatArray>(ary.get()), elems, mode);
		else if constexpr (std::is_same_v<T, jdouble>) env->ReleaseDoubleArrayElements(static_cast<jdoubleArray>(ary.get()), elems, mode);
		else static_assert(always_false<T>, "Unsupported type");
		// clang-format on
	}

private:
	static consteval auto ElementClassName()
	{
		constexpr auto sig = JNISignature<T>::Value();
		if constexpr (sig.data[0] == 'L') return sig.template substr<1, sizeof(sig.data) - 3>();
		else return sig;
	}

	JNILocalRef<jarray> ary;
};

template <typename T>
struct JNISignature<JNIArray<T>>
{
	static consteval auto Value()
	{
		return FixedString("[") + JNISignature<T>::Value();
	}
};

} // namespace EasyJni