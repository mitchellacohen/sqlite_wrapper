# sqlite_wrapper
Wrapper to simplify queries with [SQLite](https://www.sqlite.org) [Version 3](https://www.sqlite.org/version3.html).

The goal is to provide a simple interface to execute queries and loop through results of rows.

[Example code usage](./src/lib/sqlite_sample_main.cc)

Some details:
* The included [BUILD file](./src/lib/BUILD) is written for [bazel](http://bazel.build).
* Functions return an [abseil status code](https://github.com/abseil/abseil-cpp/tree/master/absl/status).
* Coding style follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) including class, function and variable naming.
