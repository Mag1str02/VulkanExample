#pragma once

#define NO_COPY_CONSTRUCTORS(class_name)                     \
    class_name(const class_name& other)            = delete; \
    class_name& operator=(const class_name& other) = delete;

#define NO_MOVE_CONSTRUCTORS(class_name)                \
    class_name(class_name&& other)            = delete; \
    class_name& operator=(class_name&& other) = delete;

#define COPY_CONSTRUCTORS(class_name)    \
    class_name(const class_name& other); \
    class_name& operator=(const class_name& other);

#define MOVE_CONSTRUCTORS(class_name) \
    class_name(class_name&& other);   \
    class_name& operator=(class_name&& other);

#define DEFAULT_COPY_CONSTRUCTORS(class_name)                 \
    class_name(const class_name& other)            = default; \
    class_name& operator=(const class_name& other) = default;

#define DEFAULT_MOVE_CONSTRUCTORS(class_name)            \
    class_name(class_name&& other)            = default; \
    class_name& operator=(class_name&& other) = default;

namespace Engine {
    struct NonCopyable {
        NonCopyable() = default;
        NO_COPY_CONSTRUCTORS(NonCopyable);
    };
    struct NonMovable {
        NonMovable() = default;
        NO_MOVE_CONSTRUCTORS(NonMovable);
    };
    struct NonCopyMoveable : NonCopyable, NonMovable {
        NonCopyMoveable() = default;
    };
}  // namespace Engine