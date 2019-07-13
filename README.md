# FuckMake

FuckMake is a much simpler thing than make and it wont just suddenly take a dump in your face.

## Example

Example compiling [PowerSupply](https://github.com/JeppeSRC/PowerSupply)

```
!FuckMake

CFiles = GetFiles(src/, *.c,)
ASMFiles = GetFiles(src/, *.asm *.s,)

IncludeDirs = src/

OutDir = bin/%(config)/%(platform)/
ObjDir = %(OutDir)obj/

CC = arm-none-eabi-gcc
LD = %(CC)
AS = arm-none-eabi-as

AFLAGS= -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS= %(AFLAGS) -O3 -Wno-unknown-pragmas -Wno-nonnull-compare -Wall
LFLAGS= -nostartfiles -nodefaultlibs -nostdlib -Wl,--gc-sections

Compile {
    Msg(Compiling %Input)
    %(CC) %(CFLAGS) -c -o %Output %Input
}

Assemble {
    Msg(Assembling %Input)
    %(AS) %(CFLAGS) -c -o %Ouput %Input
}

Link {
    Msg(Linking)
    %(LD) %(LFLAGS) -o MicroCode.elf %Input
}

Build: 
    ExecuteList(Compile, %(CFiles), %(OutDir))
    ExecuteList(Assemble, %(ASMFiles), %(OutDir))
    Execute(Link,GetFiles(%(ObjDir)),)

Clean:
    DeleteFiles(GetFiles(%(OutDir)))

```

### Some form of explanation

##### Beginning of file
`!FuckMake` Marks the start of the FuckFile (it's just there to say FUCK YOU MAKE one more time).

##### Variables
`Name = <something>` Defines a variable.

`%(Name)` Is used to get the value of a variable.

##### Actions

```
Name {
    ....
}
```

Defines an action which can be executed with [Execute](#execute). Actions define how to compile, assemble, link etc.

`%Input` and `%Output` are built in variables that will be different depending on how the action is executed.

##### Execute

There are two types of execute functions, [ExecuteList](#executelist) and [Execute](#executesingle)

#### Targets

`Name:` Defines a target. All targets must be at the end of the file.

#### Functions

-   [`GetFiles`](#getfiles)
-   [`DeleteFiles`](#deletefiles)
-   [`Msg`](#msg)
-   [`ExecuteList`](#executelist)
-   [`Execute`](#executesingle)

##### GetFiles

`GetFiles(Directory, Wildcards, Exclusions)` 

-   `Directory` The root directory to start grabbing files in.
-   `Wildcards` You can use this to filter the files included. Example `*.c` will only include files that end with a `.c`. Multiple wildcards can be specified if seperated by a space
-   `Exclusions` Used to exclute files and subdirectories that gets included. Example `print.c` will exclude all files named `print.c` even if it gets included by the wildcard parameter.

All parameters are optional, all files in the current directory will be included if all are left blank.

##### DeleteFiles

`DeleteFiles(Files)`

-   `Files` A list of files to be deleted.

##### Msg

`Msg(Message)`

-   `Message` A message that will be printed when executed.

##### ExecuteList

`ExecuteList(Action, Files, OutDir)`

-   `Action` Specifies the actions to be executed.
-   `Files` A List of files separeted by commas to be used as input files. This will be the contents of `%Input`.
-   `OutDir` Is a path to where the files shall be written. This is the directory that will be in `%Output`.

`Files` and `OutDir` are optional and may be left blank, if left blank, all files in the curret directory and all sub-directories will be used as input files and the current directory will be used as output directory.

Executes the action once for every file in the `Files` list.

##### Execute

`Execute(Action, Files, OutDir)`

-   `Action` Same as [ExecuteList](#executelist)
-   `Files` Same as [ExecuteList](#executelist)
-   `OutDir` Same as [ExecuteList](#executelist)

`Files` and `OutDir` are optional and may be left blank, if left blank files in the curret directory will be used as input files and the current directory will be used as output directory.

Same as [ExecuteList](#executelist) except that it's only executed once and `%Input` will be a list of `Files` separated by spaces.
