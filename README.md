# mvec.h

This is a single-header library implementing the core set of functions to work
with monolithic vectors. You can use it for multi-purpose dynamic arrays.

## Implementation details

Monolithic vector (from now on also called mvector, vector or mvec) is a
variable-size data structure providing the functionality of a dynamic array.
It's called monolithic because both the data and the vector's characteristics
are stored in the same chunk of memory.

The vector stores information about its current capacity, length and size of a
single element. Capacity is a maximum amount of elements that the vector can
contain. Length is a logical size of the array, i.e. amount of elements already
stored in the vector. All of these fields are of compile-time known
constant-sized `size_t` type and form a so-called vector's header.

When custom allocators are used, MvecHeader also stores information about
allocator that was used to allocate given vector.

```
                    | (relative address) |     (value)     |
                    |____________________|_________________|
                    |          0         |    MvecHeader   |
mvec_t* mvec -----> |sizeof(MvecHeader)+1|   T first_elem  |
                    |     (T*)mvec+1     |   T secnd_elem  |
                    |     (T*)mvec+2     |   T third_elem  |
                    |         ...        |       ...       |
```

When the vector gets created, a single chunk of memory is allocated. The first
`sizeof(MvecHeader)` bytes get reserved for the header. The entire subsequent
section of given chunk is where the actual data (elements of the array) is
stored.

Even though the vector stores the size of a single element, you cannot store
any other type info in it. Anyways, since technically `mvec_t` is a type alias
for `void`, it is a common and useful practice to assign a pointer to a newly
allocated mvec to a properly typed pointer:

```c
mvdef int* iv = mvalloc(12, sizeof(int));
```

Let's decompose it.

The token `mvdef` technically gets replaced with nothing and thus does nothing
but leaving it at the type declarations provides information that the given
pointer is supposed to be not just a pointer to elements but a pointer to mvec.

`int*` is valid because `mvalloc` returns the pointer to the first element.
With that said, you can iterate over it like over a normal pointer to the
multiple amount of elements, e.g. `iv[2]`.

`mvalloc`'s return type is `mvec_t*`, which is technically a `void*`, so the
implicit type conversion occurs.

The library provides only the core functionality. There are several reasons:
- A relatively small storage space footprint.
- Some functions could require element type information to be implemented. It
could lead either to a small subset of functions with ugly and
counter-intuitive application methods or a bunch of function factory macros
which are a headache on their own.
- Most of the functions can be easily implemented on the fly. See the `tests`
and `examples` for examples.

## Safety

The library depends on standard C library and/or the libraries that custom
allocator and/or memory copying/moving functions taken from. With that said,
it inherits the conditions under which the behavior is undefined. The library
**DOES NOT PROVIDE SAFETY CHECKS** in any way except:
- Return values of some functions
- Undefined behavior conditions in function descriptions

The conditions mentioned above can though be used to build on top of the
library proper safety checks that cover most of the UB conditions.

Monolithic vectors are initialized in dynamically allocated memory chunks. To
avoid memory leak, don't forget to call `mvfree()` to release the memory. Avoid
double frees and other memory management mistakes.

## Usage

1. Copy the `mvec.h` to the place you usually resolve headers from. This is the
only file you need.
2. `#include "mvec.h"` as usual header where you need.
3. `#define MVEC_IMPLEMENTATION` **before** `#include`ing `"mvec.h"` in the
translation unit in which you want the library implementation to be added. You
can also add a separate file (let's call it `mvec.c`) and leave there only two
lines:

```c
#define MVEC_IMPLEMENTATION
#include "mvec.h"
```

4. Enjoy. See the `tests` and `examples` for example usage in source code.

If you need custom allocators support, `#define MVEC_CUSTOM_ALLOCATORS` before
`#include`ing the header. If you have a ton of sources, it might be better
to define this macro using your compiler/build system functionality (e.g.
CMake's `add_compile_definitions(MVEC_CUSTOM_ALLOCATORS)`).
Set the library's current allocator via `mvec_setAllocator` before calling any
(re)allocating/freeing functions. Note: mvectors remember the allocator they
were allocated with. It also affects on mvec header size.

If you need custom `memcpy()` and `memmove()` support, consider defining
`MVEC_CUSTOM_MEMFUNCS` the same way you would do that with
`MVEC_CUSTOM_ALLOCATORS`. Set the library's current `memcpy()` and `memmove()`
implementations via `mvec_setMemcpy()` and `mvec_setMemmove()` respectively
before calling any functions that use these ones.

## Development

To build the project, `cd` to the root of the repository and perform these
commands:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

It will produce all tests and examples. To run tests, run `ctest` or use the
generated pseudo-target `test` in the build system. E.g. for `make`, simply
`make test`.

Try the examples built into the `examples` directory. Also try `test_bench`
which is disabled for taking too long.

Of course, when copying the header to your project, it will obey the rules
you've defined there.

## Legal stuff

Made by **nz**, aka **Nick Zaber** aka **nunzayin**

Licensed under MIT license
