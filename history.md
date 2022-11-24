_Personal forword: in order to simplify for teaching, there is value in not mentioning too much technical terms, which may dazzle a student of knowledge._

### History
- Martin Richards developed BCPL in 1967.
- Ken Thompson improved this language and named "B".
  - one of the contraints was small computer memory
- Dennis Ritchie developed "C" at the Bell Telepohone Laboratories in 1972.
  - named C, as it copied many features from B
  - for use in Unix
  - many ideas borrowed from Algol 68
  - all cpu architectures supported
- Brian Kernighan authored "The C Programming Language" in 1978.
- A committee named "X3J11" produced standards e.g. C89, C99, C11, C17, C23.

### Spec
- A procedural [imperative](imperative-vs-declarative.md) language supporting structured programming, lexical variable scope, and recursion, with a static type system.
- Low-level access to memory.
- Outputs efficient binaries, with minimal runtime support.
- Portable. Encourages cross-platform.
- Can be compiled to many platforms(cpu arch) and operating systems.

### Operators and Data types
- 44 reserved keywords, however compilers implement further convenient identifiers. Such as `__declspec` in MSVC.
- Important operators:
  - arithmetic, assignment, augmented assignment
  - bitwise logic `~` `*` `[]`, bitwise shifts `<<` `>>`
  - boolean logic, conditional evaluation `? :`
  - order relations `< <= > >=`

- `char` is a single-byte data type.
- Static type system. Compilers ensure correctness of expressions. However programmers can override the checks through:
  1. explicit typecast
  2. using pointers or unions to reinterpret the underlying bits of a data object

### Pointers
- Pointers are a type of reference that records the address/location of data in memory.
- Pointers can be dereferenced:
  1. to **access** the data stored at the address pointed to
  2. to invoke a pointed-to function
- Pointers can be manipulated using assignment i.e. pointer arithmetic.
- Pointers can be type-checked in compile-time because, a pointer's type includes the type of the thing pointed to.
- Pointer arithmetic is **automatically scaled** by the size of the pointed-to data type.

- Common usage:
  - Text strings are often manipulated using pointers into arrays of characters.
  - Dynamic memory allocation is performed using pointers, result of `malloc`
  - Many data types e.g. trees, are commonly implemented as dynamically allocated `struct` objects linked together.
  - In multi-dimensional arrays and in `struct[]`, pointers to other pointers are used.
  - In higher-order functions - e.g. qsort, bsearch, dispatch tables, callbacks - pointers to functions are passed as arguments.


- A _null pointer_ value points to no valid location. Deferencing a null pointer value is undefined, often resulting in a _segmentation fault_. Null pointer values are useful to indicate special cases such as:
  - no "next" pointer in the final node of a _linked list_
  - an error (for functions that return a pointer)

- A _void pointer_ `void *` points to objects of unspecified type, and therefore can be used as "generic" data pointers.
- Void pointers cannot be dereferenced, and pointer arithmetic on them is **not allowed** - since the size and type of the pointed-to thing in not known.


- Careless use of pointers is potentially dangerous. 
  1. Undesirable effects i.e. exploits, can be caused using invalid pointer arithmetic, pointers can be made to point to unsafe places.
  2. *Dangling pointers* are those which are used after deallocation.
  3. *Wild pointers* are those which are used without having been initialized.
  4. A pointer may be assigned an unsafe value using a typecast, union, or through another corrupt pointer.
- In general, C is permissive in allowing manipulation and conversion between pointer types, although compilers provide options for various levels of checking. Such as: `-Wno-overflow` `-Wno-stringop-overflow`.


### Arrays
- Definition:
  - Traditionally fixed e.g. `int arr[] = {1, 2, 3};` or `int arr[3] = {1, 2, 3};`
  - C99 allows variable-length arrays e.g. `void vla(int len) { int arr[n]; }`
  - Possible to `malloc` at run-time, and treat it as an array.
- Usage:
  - In effect, arrays are accessed as pointers.
  - Array access is typically not checked against the underlying array size. However some compilers provide *bounds-checking* as an option.
  - Array bounds violations are possible, and can lead to:
    - *illegal memory access*
    - *data corruption*
    - [buffer overruns](buffer-overruns.md)
    - run-time exceptions
  - 
