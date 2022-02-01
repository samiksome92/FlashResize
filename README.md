# Flash Resize

A program which starts a standalone flashplayer instance and resizes it to fill the screen. Relies on `flashplayer.exe` being in `%PATH%`. At the very least the screen resolution and the OS need to be specified using arguments. Arguments are also available for custom setups where window borders and display are may vary from the default configurations.

## Compilation
Compile using `cl` along with `user32.lib` as follows:

    cl /O2 /FoFlashResize.obj /FeFlashResize.exe FlashResize.cpp user32.lib

## Usage
The program supports the following arguments.

    /R<width>x<height>    Sets screen resolution. For example `/R1920x1080`

    /WIN10                Sets dimensions for default Windows 10
    /WIN11                Sets dimensions for default Windows 11
    
    /WL<num>              Window left border
    /WR<num>              Window right border
    /WT<num>              Window top border
    /WB<num>              Window bottom border
    /DL<num>              Display left
    /DR<num>              Display right
    /DT<num>              Display top
    /DB<num>              Display bottom
    
    <path>                 File path

The resolution and OS arguments set the default window borders and display area. For example `/R1920x1080 /WIN11` sets window borders as 8, 8, 51, 8 (left, right, top and bottom), since those are the default sizes of the border around the flash content window in Windows 11. For left, right and bottom it is 1 pixel visible window border and 7 pixels invisible resize border. For top, it is 1 pixel border and 50 pixels titlebar and menubar. The display area is set as 0, 1920, 0, 1032 (left, right, top, bottom). Bottom is 1032 instead of 1080 to account for the 48 pixel taskbar.

If non-default configuration is used (custom themes, different taskbar height/position) then the values need to be set manually using `/W` and `/D` options. `/WL`, `/WR`, `/WT`, `/WB` specify the window borders around the actual flash content. `/DL`, `/DR`, `/DT`, `/DB` specify the display area to which the window should be fit. Thus, if someone is running Windows 10 with taskbar at top, they would set display values as 0, 1920, 40, 1080 (Windows 10 has 40 pixel taskbar).

At the very least it has to be called with a resolution and a OS argument. Thus for default Windows 10 at 2560x1440 resolution, one would use:

    FlashResize.exe /R2560x1440 /WIN10 <path>