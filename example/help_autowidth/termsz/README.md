# termsz.h

`termsz.h`はターミナルのサイズを取得するライブラリです。

ヘッダ内にすべてのソースコードを含めているため、`termsz.h`をプロジェクトにコピーしてインクルードするだけで使用できます。

## Env

たぶんWindows/Mac/Linuxどれでも行けると思う。
ダメなケースあったらIssue投げてください。

## Usage

1. Include `termsz.h`
```c
#include "termsz.h"
```

2. Call `getTerminalSize()`

```c
int row, col;
int ret;
ret = getTerminalSize(&row, &col);
```

3. All done !
```
 <------------------------------------------ col ------------------------------------------>
+-------------------------------------------------------------------------------------------+
|grape@GrapeCenter:~$ gcc --version                                                         |  ^
|gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0                                                  |  |
|Copyright (C) 2021 Free Software Foundation, Inc.                                          |  |
|This is free software; see the source for copying conditions.  There is NO                 |  |
|warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                | row
|                                                                                           |  |
|grape@GrapeCenter:~$                                                                       |  |
|grape@GrapeCenter:~$                                                                       |  |
|grape@GrapeCenter:~$                                                                       |  V
+-------------------------------------------------------------------------------------------+
```

- Return value ... 0 : success / 1 : failed

