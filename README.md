# cgrep - conditional grep

This is just another variation of grep (because we needed more of those) that allows you to require or reject files before the normal grep process based upon patterns. For example, you could reject all patterns that include `version :latest` before matching the `version` pattern in a bunch of package manifest files.

## Requires
This was built using `C++17` and it uses the POSIX header file `glob.h` which I assume is only available on `macOS` and `Linux`. I tested it on `macOS`.

## Building
```bash
make release
./cgrep
```

## Examples
#### By default cgrep asks you to enter patterns at the command line.
```
$ cgrep src/*.cpp src/*.hpp --count
# Enter grep patterns starting with...
#   '+' for required patterns
#   '-' for rejected patterns
#   '=' for matched patterns
# Enter a blank line to submit.
> + <map> 
> + <vector>
> = #include
> 

src/grep_factory.hpp:4
src/output_strategy.hpp:5
src/parse_input.hpp:3
```
---
#### Those patterns can also be sourced from a file.
```
$ cgrep src/*.cpp src/*.hpp --count --pattern-file=patterns.txt
src/grep_factory.hpp:4
src/output_strategy.hpp:5
src/parse_input.hpp:3
```
`patterns.txt`
```
+ <map> 
+ <vector>
= #include
```

## Help Page

```
[cgrep]
-------
Conditionally grep files using multiple
required(+), rejected(-) and matched(=) patterns.
Required and rejected patterns are resolved
before returning matches.
   
[usage]
-------
cgrep [-acEhilLn] [-p=pattern-file] [-o=output-file] [file ...]
   
[options]
---------
-a / --again
   Rerun previous query again.
-c / --count
   Count the number of matched lines in each file.
-E / --extended-regexp
   Sets pattern matching to egrep.
-h / --help
   Shows this page.
-i / --ignore-case
   Sets grep to ignore case when matching.
-l / --files-with-matches
   Lists all files with matched lines.
-L / --files-without-matches
   Lists all files without matched lines in search set.
-n / --line-number
   Each output line is preceded by its relative line number.
-o= / --output-file=
   Writes results to a file.
-p= / --pattern-file=
   Reads grep patterns from a file.
```