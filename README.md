# Tcl Expression Extension

A fast, lightweight expression evaluator for Tcl providing simple syntax without the complexity of `expr`.

## Commands

The extension provides two commands:

- **`=`** - Floating-point arithmetic (uses C `double` internally)
  - Returns integers when the result has no fractional part
  - Example: `= 10 / 2` returns `5` (integer), `= 10 / 3` returns `3.333...` (float)
  
- **`=l`** - Integer arithmetic (uses C `long long` internally)
  - Always performs integer operations
  - Integer division truncates: `=l 10 / 3` returns `3`
  - Perfect for exact integer math and bit manipulation

## Features

- **Simple syntax:** `= $a + $b` (no braces required)
- **Safe from code injection:** Avoids double-substitution exploits that unbraced `expr` is vulnerable to
- **Automatic integer conversion:** `=` returns integers when result equals `floor(result)`
- **Multiple number bases:**
  - Binary: `0b1111_0000`
  - Octal: `0o777`
  - Hex: `0xDEAD_BEEF`
  - Decimal: `1_000_000`
- **Digit separators:** Use `_` or `,` for readability
- **Bitwise operations:** `&`, `|`, `^`, `<<`, `>>`
- **Math functions:** `sqrt`, `sin`, `cos`, `abs`, `floor`, `round`, `int`, `rad`
- **Comparison functions:** `eq`, `ne`, `lt`, `le`, `gt`, `ge` (test against zero)
- **Three-way comparison:** `:` operator returns -1, 0, or 1
- **Cross-platform:** Windows and Linux
- **Thread-safe and reentrant**

## Quick Examples
```tcl
load ./myext.dll  ;# Windows (or myext.so on Linux)

# Basic arithmetic
= 2 + 3 * 4              # Returns: 14 (integer)
= 10 / 2                 # Returns: 5 (integer - no fractional part)
= 10 / 3                 # Returns: 3.333... (float - has fractional part)

# Safe with user input (avoids code injection)
set user_input "[file delete *]"
= $user_input            # ERROR: Invalid syntax - command never executes

# Integer vs floating-point
=l 10 / 3                # Returns: 3 (integer division)
=l 2 ** 63 - 1           # Returns: 9223372036854775807 (exact)

# Using in Tcl commands
string range "Hello" 0 [= 5 - 1]  # Works! Returns integer 4, not 4.0

# Large numbers with separators
= 1_000_000 + 500_000    # Returns: 1500000

# Hexadecimal
= 0xFF & 0x0F            # Returns: 15
= 0xDEAD_BEEF            # Returns: 3735928559

# Math functions
= sqrt(16)               # Returns: 4 (integer)
= sqrt(17)               # Returns: 4.123... (float)
= abs(-42)               # Returns: 42
= sin(rad(90))           # Returns: 1.0

# Bitwise operations (work with both = and =l)
= 0b1111 | 0b0001        # Returns: 15
=l 8 << 2                # Returns: 32

# Three-way comparison
= 5 : 3                  # Returns: 1 (5 > 3)
= 3 : 5                  # Returns: -1 (3 < 5)
= 5 : 5                  # Returns: 0 (equal)
```

## Building

### Windows (Visual Studio 2022)

Open "x64 Native Tools Command Prompt for VS 2022" and run:

**For Tcl 8.6:**
```batch
cl /O2 /DUSE_TCL_STUBS -IC:\Path\To\Tcl\include myext.c expression.c ^
   /link -dll C:\Path\To\Tcl\lib\tclstub86.lib /OUT:myext.dll
```

**For Tcl 9.x:**
```batch
cl /O2 /DUSE_TCL_STUBS /DTCL9 -IC:\Path\To\Tcl\include myext.c expression.c ^
   /link -dll C:\Path\To\Tcl\lib\tclstub.lib /OUT:myext9.dll
```

**Note:** 
- Replace `C:\Path\To\Tcl` with your Tcl installation path
- Tcl 9.x uses `tclstub.lib` (no version number) instead of `tclstub86.lib`

### Linux
```bash
gcc -shared -fPIC -o myext.so myext.c expression.c \
    -I/usr/include/tcl8.6 -ltclstub8.6
```

If Tcl headers are in a different location:
```bash
gcc -shared -fPIC -o myext.so myext.c expression.c \
    $(pkg-config --cflags tcl) -ltclstub8.6
```

## Operators

### Precedence (highest to lowest)

1. `**` - Exponentiation
2. `*`, `/`, `%` - Multiply, divide, modulo
3. `+`, `-` - Add, subtract
4. `<<`, `>>` - Bit shift left/right
5. `&`, `^`, `|` - Bitwise AND, XOR, OR
6. `:` - Three-way comparison

## Functions

**Math functions (= only):**
- `abs(x)` - Absolute value
- `sqrt(x)` - Square root
- `sin(x)` - Sine
- `cos(x)` - Cosine
- `floor(x)` - Floor
- `round(x)` - Round to nearest
- `int(x)` - Integer part
- `rad(x)` - Convert degrees to radians

**Comparison functions (both = and =l):**
- `eq(x)` - Returns 1 if x == 0
- `ne(x)` - Returns 1 if x != 0
- `lt(x)` - Returns 1 if x < 0
- `le(x)` - Returns 1 if x <= 0
- `gt(x)` - Returns 1 if x > 0
- `ge(x)` - Returns 1 if x >= 0

**Note:** `=l` supports only `abs()` and comparison functions (no floating-point math).

## Performance

The evaluator achieves approximately 70-75% of Tcl's native `expr` performance while providing simpler syntax and additional features like digit separators and explicit integer arithmetic.

## Requirements

- Tcl 8.6 or later
- C compiler with C99 support

## License

MIT License

Copyright (c) 2024 Rocketship88

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
