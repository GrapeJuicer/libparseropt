# libparseropt
optargの後継となる引数解析ライブラリです。読み方は「パーサロプト」です。

# note

-n, --no        ... NO_ARGUMENT
-r, --require   ... REQUIRE_ARGUMENT
-o, --optional  ... OPTIONAL_ARGUMENT
-x              ... other argument
{ANY UPPERCASE} ... Value (not option)

```sh
$ your_exe -r           # error
$ your_exe -r XXX       # works: optarg = XXX
$ your_exe -rXXX        # works: optarg = XXX
$ your_exe -r -n        # works: optarg = -n
                        #   note: In this case, -n does not mean option. It is value.
```
