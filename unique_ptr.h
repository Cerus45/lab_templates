#pragma once

#include <cstddef>
#include <memory>
#include <utility>

// -----  основная специализация  -----
template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    // ======================== Constructors ========================

    UniquePtr() noexcept : ptr_(nullptr), deleter_() {}

    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr), deleter_() {}

    UniquePtr(T* ptr, const Deleter& deleter) : ptr_(ptr), deleter_(deleter) {}

    UniquePtr(T* ptr, Deleter&& deleter) : ptr_(ptr), deleter_(std::move(deleter)) {}

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // =================== Destructor ==============================

    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================

    T* get() noexcept {
        return ptr_;
    }

    const T* get() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T& operator*() noexcept {
        return *ptr_;
    }

    const T& operator*() const noexcept {
        return *ptr_;
    }

    T* operator->() noexcept {
        return ptr_;
    }

    const T* operator->() const noexcept {
        return ptr_;
    }

    Deleter& get_deleter() noexcept {
        return deleter_;
    }

    const Deleter& get_deleter() const noexcept {
        return deleter_;
    }

    // =================== Modifiers ===============================

    T* release() noexcept {
        T* old = ptr_;
        ptr_ = nullptr;
        return old;
    }

    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            if (ptr_)
                deleter_(ptr_);
            ptr_ = ptr;
        }
    }

    void swap(UniquePtr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(deleter_, other.deleter_);
    }

private:
    T* ptr_;
    Deleter deleter_;
};

// -----  специализация для массивов  -----
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    // ======================== Constructors ========================

    UniquePtr() noexcept : ptr_(nullptr), deleter_() {}

    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr), deleter_() {}

    UniquePtr(T* ptr, const Deleter& deleter) : ptr_(ptr), deleter_(deleter) {}

    UniquePtr(T* ptr, Deleter&& deleter) : ptr_(ptr), deleter_(std::move(deleter)) {}

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // =================== Destructor ==============================

    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================

    T* get() noexcept {
        return ptr_;
    }

    const T* get() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T& operator[](size_t index) noexcept {
        return ptr_[index];
    }

    const T& operator[](size_t index) const noexcept {
        return ptr_[index];
    }

    Deleter& get_deleter() noexcept {
        return deleter_;
    }

    const Deleter& get_deleter() const noexcept {
        return deleter_;
    }

    // =================== Modifiers ===============================

    T* release() noexcept {
        T* old = ptr_;
        ptr_ = nullptr;
        return old;
    }

    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            if (ptr_)
                deleter_(ptr_);
            ptr_ = ptr;
        }
    }

    void swap(UniquePtr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(deleter_, other.deleter_);
    }

private:
    T* ptr_;
    Deleter deleter_;
};

// =================== make_unique ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
UniquePtr<T> make_unique(size_t size) {
    // для массивов
    return UniquePtr<T>(new std::remove_extent_t<T>[size]());
}
