<p align="center"><img src="assets/logo.svg" alt="EasyJni logo" width="200"></p>
<h1 align="center">EasyJni</h1>
<p align="center">Header-only C++20 library for type-safe, declarative JNI. Mirror Java classes in C++</p>

---
Describe a Java class once as a C++ **mirror**, then use it like a normal object.

```cpp
class Point : public JNIClassMirror<"com/example/Point", Constructors<jint, jint>>
{
public:
	using JNIClassMirror::JNIClassMirror;

	JNIFieldMirror<"x", jint> x{this};
	JNIMethodMirror<"distanceTo", jdouble, Point> distanceTo{this};
};

Point a(env, jint{0}, jint{0});
Point b(env, jint{3}, jint{4});
jdouble d = a.distanceTo(b); // 5.0
```

No `GetMethodID`, no hand-written `"(Lcom/example/Point;)D"` strings, no `ExceptionCheck` after every call.

## Features

- **Header-only**: just add the include directory.
- **Compile-time signatures**: JNI signatures are generated from the C++ types.
- **Mirrors** for methods, fields, constructors, static members, inheritance and interfaces.
- **Cached IDs**: classes, method IDs and field IDs are looked up once per mirror type.
- **RAII references**: local and global references are released automatically.
- **Exceptions**: Java exceptions are rethrown as `EasyJni::JNIException`.
- **Strings and arrays**: UTF-8/UTF-16 access, primitive and object arrays, arrays of mirrors.

## Installation

### Requirements

- C++20 compiler
- CMake 3.14+
- JDK 8+

### CMake

EasyJni is an `INTERFACE` CMake target that also links JNI.

With `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    EasyJni
    GIT_REPOSITORY https://github.com/Dtuna2830/EasyJni.git
    GIT_TAG main
)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(EasyJni)

target_link_libraries(target PRIVATE EasyJni)
```

With `add_subdirectory`:

```cmake
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/EasyJni)
target_link_libraries(target PRIVATE EasyJni)
```

## Mirrors

```java
class Animal { String getName() { ... } }
interface Greeter { String greet(String other); }

class Dog extends Animal implements Greeter {
    static int count;
    int age;
    Dog(String name, int age) { ... } // throws if age < 0
    static Dog[] litter(int size) { ... }
}
```

```cpp
class Animal : public JNIClassMirror<"com/example/Animal">
{
public:
	using JNIClassMirror::JNIClassMirror;
	JNIMethodMirror<"getName", JNIString> getName{this};
};

class Greeter : public JNIClassMirror<"com/example/Greeter">
{
public:
	using JNIClassMirror::JNIClassMirror;
	JNIMethodMirror<"greet", JNIString, JNIString> greet{this};
};

class Dog : public JNIClassMirror<"com/example/Dog", Extends<Animal>, Implements<Greeter>, Constructors<JNIString, jint>>
{
public:
	using JNIClassMirror::JNIClassMirror;
	JNIStaticFieldMirror<"count", jint> count{this};
	JNIFieldMirror<"age", jint> age{this};
	JNIStaticMethodMirror<"litter", JNIArray<Dog>, jint> litter{this};
};
```

```cpp
Dog rex(env, JNIString::NewUtf8(env, "Rex"), jint{3});
JNIString name = rex.getName();                             // from Animal
JNIString text = rex.greet(JNIString::NewUtf8(env, "you")); // from Greeter
rex.age = jint{4};
jint dogs = rex.count;                                      // static, through an instance
Dog first = rex.litter(jint{2}).getElement(env, 0);
```

**Descriptors**

- `Extends<Parent>`: the superclass mirror (at most one).
- `Implements<Interfaces...>`: interface mirrors (at most one list).
- `Constructors<Args...>`: enables `Mirror(env, args...)` for a Java constructor. Repeat for each one you need. Any
  mirror can also wrap an existing object with `Mirror(env, jobject)`.

**Members** are initialized with `{this}`: `JNIMethodMirror`, `JNIStaticMethodMirror`, `JNIFieldMirror` and
`JNIStaticFieldMirror`, each taking the Java name followed by the types.

## Strings, arrays and exceptions

```cpp
JNIString s = JNIString::NewUtf8(env, "hello");
s.charsUtf8(env, [](const char *chars, jsize len) { /* ... */ });

auto numbers = JNIArray<jint>::New(env, 3);
numbers.setElement(env, 0, jint{42});
numbers.elements(env, [](jint *data, jsize len) { /* direct access, written back on return */ });

try
{
	Dog invalid(env, JNIString::NewUtf8(env, "Ghost"), jint{-1});
}
catch (const JNIException &e)
{
	e.what(); // "age must be positive"
	e.get();  // the Java throwable
}
```

## Low-level API

Mirrors are built on thin typed wrappers that you can also use directly:

```cpp
JNIClass cls = JNIClass::Find<"com/example/Dog">(env);
auto greet = cls.getMethod<jstring, jstring>(env, "greet"); // "(Ljava/lang/String;)Ljava/lang/String;"
JNIString result(env, greet.call(env, obj, other.get()));
JNIException::CheckAndThrow(env);

JNILocalRef<jobject> local(env, obj);
JNIGlobalRef<jobject> global(env, std::move(local));
```

## Details

**Passing `env` means taking ownership.** Wrappers built with an `env` delete their local reference when destroyed.
Built without one, they are only views.

| Owns the reference                                 | Only a view                              |
|----------------------------------------------------|------------------------------------------|
| `JNILocalRef<T>(env, ref)`                         | `JNILocalRef<T>(nullptr, ref)`           |
| `JNIString(env, str)`, `JNIString::From(env, str)` | `JNIString(str)`, `JNIString::From(str)` |
| `JNIArray<T>(env, array)`                          | `JNIArray<T>(array)`                     |
| `JNIException(env, throwable)`                     | `JNIException(throwable)`                |

Wrap references you don't own, such as native method parameters, as views:

```cpp
extern "C" JNIEXPORT void JNICALL Java_com_example_Native_print(JNIEnv *env, jclass, jstring text)
{
	JNIString view(text); // won't delete the caller's reference
	view.charsUtf8(env, [](const char *chars, jsize len) { /* ... */ });
}
```

**Low-level calls don't check for exceptions.** A failed `getMethod` or `getField` returns an empty object and leaves
the Java error pending. `JNIClass` doesn't own its `jclass` either, so release the result of `Find` yourself:

```cpp
JNIClass cls = JNIClass::Find<"com/example/Dog">(env);
JNILocalRef<jclass> owner(env, cls.get()); // deletes the class reference

auto bark = cls.getMethod<void>(env, "bark");
if (!bark)
	JNIException::CheckAndThrow(env); // NoSuchMethodError
```

## License

EasyJni is distributed under the terms described in [LICENSE](LICENSE).