#include <memory>
#include <vector>

namespace Array
{
/// @brief Fixed size vector
/// @tparam T
/// @tparam Allocator
template <class T, class Allocator = std::allocator<T>> class FixedSizeVector
{
  public:
#pragma region construction and destruction
    explicit FixedSizeVector(size_t size) : _data(size)
    {
    }

    FixedSizeVector(const FixedSizeVector &other) : _data(other._data)
    {
    }

    FixedSizeVector(FixedSizeVector &&other) noexcept : _data(std::move(other._data))
    {
    }

    ~FixedSizeVector() = default;

    FixedSizeVector &operator=(const FixedSizeVector &other)
    {
        _data = other._data;
        return *this;
    }
#pragma endregion

#pragma region Iterators
    /// @brief Get the iterator to the beginning of the vector
    /// @return The iterator to the beginning of the vector
    std::vector<T>::iterator begin()
    {
        return _data.begin();
    }

    /// @brief Get the iterator to the end of the vector
    /// @return The iterator to the end of the vector
    std::vector<T>::iterator end()
    {
        return _data.end();
    }

    /// @brief Get the const iterator to the beginning of the vector
    /// @return The const iterator to the beginning of the vector
    std::vector<T>::const_iterator cbegin() const
    {
        return _data.cbegin();
    }

    /// @brief Get the const iterator to the end of the vector
    /// @return The const iterator to the end of the vector
    std::vector<T>::reverse_iterator rbegin()
    {
        return _data.rbegin();
    }

    /// @brief Get the const iterator to the end of the vector
    /// @return The const iterator to the end of the vector
    std::vector<T>::reverse_iterator rend()
    {
        return _data.rend();
    }

    /// @brief Get the const iterator to the beginning of the vector
    /// @return The const iterator to the beginning of the vector
    std::vector<T>::const_reverse_iterator crbegin() const
    {
        return _data.crbegin();
    }

    /// @brief Get the const iterator to the end of the vector
    /// @return The const iterator to the end of the vector
    std::vector<T>::const_reverse_iterator crend() const
    {
        return _data.crend();
    }
#pragma endregion

#pragma region Capacity
    /// @brief Get the size of the vector
    /// @return The size of the vector
    size_t size() const
    {
        return _data.size();
    }

    /// @brief Get the capacity of the vector
    /// @return The capacity of the vector
    size_t capacity() const
    {
        return _data.capacity();
    }

    /// @brief Check if the vector is empty
    /// @return If the vector is empty, return true.
    bool empty() const
    {
        return _data.empty();
    }

    // Below functions may concern the size of the vector, therefore, they are not implemented.
    // size_t max_size() const;
    // void resize(size_t count);
    // void resize(size_t count, const T &value);
    // void reserve(size_t new_cap);
    // void shrink_to_fit();
#pragma endregion

#pragma region Modifiers
    /// @brief Access the element at the index
    /// @param index The index of the element
    /// @return The reference to the element
    T &operator[](size_t index)
    {
        return _data.at(index);
    }

    /// @brief Access the element at the index
    /// @param index The index of the element
    /// @return The reference to the element
    const T &operator[](size_t index) const
    {
        return _data.at(index);
    }

    /// @brief Access the element at the index
    /// @param index The index of the element
    /// @return The reference to the element
    T &at(size_t index)
    {
        return _data.at(index);
    }

    /// @brief Access the element at the index
    /// @param index The index of the element
    /// @return The reference to the element
    const T &at(size_t index) const
    {
        return _data.at(index);
    }

    /// @brief Access the element at the front
    /// @return The reference to the element
    T &front()
    {
        return _data.front();
    }

    /// @brief Access the element at the front
    /// @return The reference to the element
    const T &front() const
    {
        return _data.front();
    }

    /// @brief Access the element at the back
    /// @return The reference to the element
    T &back()
    {
        return _data.back();
    }

    /// @brief Access the element at the back
    /// @return The reference to the element
    const T &back() const
    {
        return _data.back();
    }

    /// @brief Access the data
    /// @return The pointer to the data
    T *data()
    {
        return _data.data();
    }

    const T *data() const
    {
        return _data.data();
    }
#pragma endregion

#pragma region Modifiers
    // This section will length of the vector, therefore, it is not implemented.
#pragma endregion

  private:
    std::vector<T> _data;
};
} // namespace Array
