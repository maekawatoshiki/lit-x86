** Lit has stoped development **

## Language Wiki
- Please look at Wiki.

## Try it on Docker
```sh
$ docker build -t="lit-lang" .
$ docker run -i -t "lit-lang"
```

## Build
- Lit generates native codes with LLVM.
- When you build, need LLVM and Clang.

- LLVM Core (== 3.5) (detail in Dockerfile)
- make

```
$ make test
$ ./lit -h # -h option shows helps
```

## License
- Unlicense(Pubilc Domain!)
