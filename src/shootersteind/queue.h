#ifndef __SHOOTERSTEIN_QUEUE_H__
#define __SHOOTERSTEIN_QUEUE_H__

#include <vector>
#include <mutex>
#include <cstring>
#include <cstdlib>
#include <new>
#include <thread>

namespace shooterstein {

    template<class T>
    class Queue {
        T* _items;
        volatile std::size_t _start;
        volatile std::size_t _size;
        volatile std::size_t _cap;
        std::mutex _mutex;

    public:
        typedef typename std::add_rvalue_reference<T>::type rvalue_reference;
        //typedef typename std::add_const<typename std::add_lvalue_reference<T>::type>::type const_reference;
        typedef typename std::add_lvalue_reference<typename std::add_const<T>::type>::type const_reference;

        Queue() {
            _items = NULL;
            _start = 0;
            _cap = 0;
            _size = 0;
        }

        ~Queue() {
            if (_cap > 0) {
                for (auto i = 0; i < _size; i++) {
                    (&_at(i))->~T();
                }

                std::free(_items);
            }
        }

        Queue(Queue<T> const& x) {
            std::lock_guard<std::mutex> guard(x._mutex);

            _items = (T*)malloc(x._size * sizeof(T));
            x._copy_to(_items);
            _size = x._size;
            _cap = x._size;
            _start = 0;
        }

        Queue(Queue<T>&& x) {
            std::lock_guard<std::mutex> guard(x._mutex);

            _items = x._items;
            _size = x._size;
            _cap = x._cap;
            _start = x._start;
            x._items = NULL;
            x._size = 0;
            x._cap = 0;
            x._start = 0;
        }

        Queue<T>& operator = (Queue<T> const& x) {
            new (this) T(x);
            return *this;
        }

        Queue<T>& operator = (Queue<T>&& x) {
            new (this) T(std::move(x));
            return *this;
        }

        void enqueue_with_maxsize(const_reference item, size_t max_size) {
            _enqueue(item, max_size);
        }

        void enqueue_with_maxsize(rvalue_reference item, size_t max_size) {
            _enqueue(::std::move(item), max_size);
        }

        void enqueue(const_reference item) {
            _enqueue(item, 0);
        }

        void enqueue(rvalue_reference item) {
            _enqueue(::std::move(item), 0);
        }

    private:

        template <class F>
        void _enqueue(F&& item, size_t max_size) {
            std::lock_guard<std::mutex> guard(_mutex);

            if (max_size > 0) {
                while (_size + 1 > max_size) {
                    _dequeue_nolock();
                }
            }

            if (_size == _cap) {
                _expand();
            }

            new (&_at(_size)) T(std::forward<F>(item));
            _size = _size + 1;
        }

    public:

        T dequeue() {
            while (true) {
                _mutex.lock();
                if (_size == 0) {
                    _mutex.unlock();
                    std::this_thread::yield();
                } else {
                    break;
                }
            }

            auto result = _dequeue_nolock();

            _mutex.unlock();
            return result;
        }

    private:
        T _dequeue_nolock() {
            T result(std::move(_at(0)));

            (&_at(0))->~T();
            _start = (_start + 1) % _cap;
            _size = _size - 1;

            if (_size < _cap / 4) {
                _contract();
            }

            return result;
        }

    public:

        size_t size() {
            std::lock_guard<std::mutex> guard(_mutex);

            return _size;
        }

        bool empty() {
            std::lock_guard<std::mutex> guard(_mutex);

            return _size == 0;
        }

    private:
        void _contract() {
            _resize(_cap / 2);
        }

        void _expand() {
            auto new_cap = _cap * 2;
            if (new_cap == 0) {
                new_cap = 1;
            }
            _resize(new_cap);
        }

        void _resize(std::size_t newcap) {
            if (newcap == 0) {
                if (_cap > 0) {
                    std::free(_items);
                }

                _items = NULL;
                _size = 0;
                _cap = 0;
                _start = 0;

                return;
            }

            auto new_items = (T*)std::malloc(newcap * sizeof(T));

            _copy_to(new_items);
            if (_cap > 0) {
                std::free(_items);
            }

            _items = new_items;
            _start = 0;
            _cap = newcap;
        }

        void _copy_to(T* arr) {
            if (_start + _size > _cap) {
                auto to_copy = _cap - _start;

                for (size_t i = 0; i < to_copy; i++) {
                    new (arr + i) T(std::move(_items[_start + i]));
                    (_items + _start + i)->~T();
                }

                for (size_t i = to_copy; i < _size; i++) {
                    new (arr + i) T(std::move(_items[i - to_copy]));
                    (_items + i - to_copy)->~T();
                }

            } else if (_size > 0) {
                for (size_t i = 0; i < _size; i++) {
                    new (arr + i) T(std::move(_items[_start + i]));
                    (_items + _start + i)->~T();
                }
            }
        }

        T& _at(std::size_t i) {
            return _items[(_start + i) % _cap];
        }
    };
}

#endif // __SHOOTERSTEIN_QUEUE_H__
