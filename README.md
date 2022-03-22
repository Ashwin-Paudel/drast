<div align="center">

# Drast Programming Language

![issues](https://img.shields.io/github/issues/Malvion/drast?style=flat-square)
![license](https://img.shields.io/github/license/Malvion/drast?style=flat-square)

A general-purpose language, that is meant to be a more modernized version of C, while still maintaining its simplicity.

[Docs](docs/docs.md) | [Changes](Changes.md) | [TODO](TODO.md)

</div>

> **NOTE:** This is still in its development phase, so things may be changed

## Features

- Super Simple and Easy to learn syntax ( basically the same thing as C, except optional semicolons and functions are
  defined differently )
- More are yet to come...

## Example

```c
import io

int :: main(int argc, string[] argv) {
    int variable = 40
    print("Hello World! $(variable)\n")
    
    return 0
}
```

## Building

Drast is written in cpp20

Just run the `CMakeLists.txt`