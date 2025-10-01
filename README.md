# mvec.h

This is a single-header library implementing the core set of functions to work
with monolithic vectors.

## Implementation details

Monolithic vector (from now on also called mvector, vector or mvec) is a
variable-size data structure providing the functionality of a dynamic array.
It's called monolithic because both the data and the vector's characteristics
are stored in the same chunk of memory.

The vector stores information about its current capacity and length. Capacity
is a maximum amount of elements that the vector can contain. Length is a
logical size of the array, i.e. amount of elements already stored in the
vector. Both of these fields are of compile-time known constant-sized `size_t`
type and form a so-called vector's header. The size of a vector's header in
bytes is also a compile-time known constant available in the library.

```
                    |(relative address)|     (value)     |
                    |__________________|_________________|
mvec_t* ----------> |        0         | size_t capacity |
                    |  sizeof(size_t)  |  size_t length  |
                    |2 * sizeof(size_t)|      data       |
                    |       ...        |       ...       |
```

When the vector gets created, a single chunk of memory is allocated. The first
`2 * sizeof(size_t)` bytes get reserved for the header. The entire subsequent
section of given chunk is where the actual data (elements of the array) is
stored.

The vector is type-agnostic: it does not store information about type of the
elements stored in it. Because of that you can use already allocated vectors
for new purposes (that require a new vector of elements of different type) by
the cost of passing the size of a single element every time you call most of
the functions of this library.
