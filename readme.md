## Preface
C is the mother of all programming languages. A programming language that influences almost all other programming languages e.g. C++, Golang, JavaScript, PHP, through its syntax, way of thinking, and internal design. 

In this day and age of higher level programming paradigms e.g. Node.js, Ruby, Swift, Python, PHP, the benefit of learning a language built in the 1970s isn't clear in the first glance. However in truth, C is inevitable to: 
- our operating systems' core e.g. [Linux kernel](https://github.com/torvalds/linux), Windows NT kernel, XNU (Darwin kernel), Plan 9 (Unix kernel), [FreeBSD kernel](https://github.com/freebsd/freebsd-src)
- our operating systems' utilities e.g. [coreutils](https://github.com/coreutils/coreutils), [busybox](https://github.com/mirror/busybox)
- operating systems e.g. [Android](https://android.googlesource.com/), [GNU/Linux](https://www.gnu.org/home.en.html)
- the most pertinent opensource libraries for web e.g. [libssl](https://github.com/openssl/openssl) that enables https on our websites, [libbrotli](https://github.com/google/brotli) for lossless data compression, [libpng](https://github.com/glennrp/libpng) for portable network graphics
- the most used utilities over the internet e.g. [openssh](https://github.com/openssh/openssh-portable), [ftp](https://github.com/aria2/aria2), [curl](https://github.com/curl/curl)
- other programming languages that developers use to build a better world e.g. [JavaScript](https://github.com/v8/v8), [Golang](https://github.com/golang/go)
- a recommended stepping stone for learning to code in [C++](https://libcxx.llvm.org/)

### Some of my playful ideas
I am still learning, I will always be. However the main aim of the wiki or other materials is to be able to *write more code*. More better code! For this reason, sometimes I work on naive ideas or pet projects just to stay warmed up in writing C code. Once again, these are naive fun playful ideas 🧸

- **activeforks** gets you forks of a repository, sorted by number of stars.
- **autofree** automatically frees/deallocates all heap memory allocated. No more `free()` or memory leak. For production code, use [Boehm collector](http://hboehm.info/gc/) or `libgc-dev`.
- **commitcount** counts the total number of commits in a GitHub project.
- **catfacts** gets you facts about cats 🐱
- **lib** contains interesting concise functions.
- **main** contains usage examples of all the above functions.
- [**chatnet-cli**](https://github.com/midnqp/chatnet-cli) is an experimental chat application for the terminal, that leverages a modern asynchronous multiplexing approach to stdin and stdout. (Promoted to a new repository!)
