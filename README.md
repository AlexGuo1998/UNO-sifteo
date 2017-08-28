# UNO-sifteo

A multiplayer card game [UNO](https://en.wikipedia.org/wiki/Uno_(card_game)) for [Sifteo Cubes](https://github.com/sifteo/thundercracker).

Chinese README can be found [HERE](#chinese)  
中文说明在[这里](#chinese)

Now developing.

## How to build

1.  Download Sifteo-SDK binary from [https://github.com/sifteo/thundercracker#getting-started](https://github.com/sifteo/thundercracker#getting-started)  
    OR  
    clone the [thundercracker repository](https://github.com/sifteo/thundercracker) and build the SDK by your own.
2.  ****VERY IMPORTANT:**** If you choose to download the binary, please make sure `include/sifteo/video/bg1.h` LINE 459 is
    ```
    unsigned word = _SYS_vbuf_peek(&sys.vbuf, offsetof(_SYSVideoRAM, bg1_x) / 2);
    ```
    If it is `bg0_x`, edit it to `bg1_x`, otherwise it can cause errors.
3.  Execute `sifteo-sdk-shell.cmd` (or `sifteo-sdk-shell.sh` on linux), `cd` to the project directory (`uno/`), and type  
    `make` if you want to build the debug version, or  
    `make RELEASE=1` if you want to build the release version.

## How to play

Use a `siftulator` to play it in emulator, or connect the Base and type `swiss install uno.elf` to install it to your Sifteo Base.

The rule for UNO can be found on [Wikipedia](https://en.wikipedia.org/wiki/Uno_(card_game)), while in the game:

*   Tilt to select a card
*   Touch to play the selected card (if the play is legal)
*   Shake to draw a card / cancel playing

## TODOs

*   Cubes add / remove in game
*   `StoredObject` suppport
*   Better animation ("UNO!", skip, reverse)
*   Music / voice

<a name=chinese></a>
# UNO-sifteo (Chinese)

多人桌游[UNO](https://en.wikipedia.org/wiki/Uno_(card_game))的[Sifteo Cube](https://github.com/sifteo/thundercracker)移植版

现在正在开发中。

## 如何编译

1.  下载二进制的 Sifteo-SDK： [https://github.com/sifteo/thundercracker#getting-started](https://github.com/sifteo/thundercracker#getting-started)  
    或者  
    克隆 [thundercracker](https://github.com/sifteo/thundercracker)，然后自己编译SDK。
2.  **非常重要：** 如果你选择下载二进制的SDK，请确认 `include/sifteo/video/bg1.h` 的第459行如下所示：
    ```
    unsigned word = _SYS_vbuf_peek(&sys.vbuf, offsetof(_SYSVideoRAM, bg1_x) / 2);
    ```
    如果是 `bg0_x`，请将其改为 `bg1_x`，否则会出现错误。
3.  运行 `sifteo-sdk-shell.cmd` （如果是linux，运行 `sifteo-sdk-shell.sh`），`cd` 到源代码的目录（`uno\`），输入  
    `make` 来生成debug（调试）版本，或者输入  
    `make RELEASE=1` 来生成release（发行）版本。

## 如何玩

使用 `siftulator` 在模拟器中调试，或者连接 Base （基地）并输入 `swiss install uno.elf`，将其安装到 Sifteo Base 中。

UNO游戏的规则可以在维基百科上找到（[英文版](https://en.wikipedia.org/wiki/Uno_(card_game))）（[中文版](https://zh.wikipedia.org/wiki/UNO)）。在游戏中：

*   倾斜选择卡片
*   触摸屏幕打出选中的卡片
*   晃动来抽一张卡，或不打出（视情况而定）

## TODOs

*   游戏中的 Cube （方块）增加/减少
*   `StoredObject` 支持（用于游戏保存）
*   更好的动画效果（"UNO!"，跳过，反转）
*   音乐/语音
