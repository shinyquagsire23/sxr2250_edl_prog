# sxr2250_edl_prog

## Building
Building uses [qtestsign](https://github.com/shinyquagsire23/qtestsign/tree/feat-mbnv7-readmbn) and `aarch64-none-elf`.

```
make
python3 ../qtestsign/qtestsign.py -v 7 -o ../sxr2250_edl_prog/sxr2250_edl_prog_resign.elf prog ../sxr2250_edl_prog/sxr2250_edl_prog.elf
```

## Useful reference material

- https://github.com/facebookincubator/oculus-linux-kernel/blob/oculus-quest3-kernel-master/arch/arm64/boot/dts/vendor/qcom/anorak.dtsi#L756