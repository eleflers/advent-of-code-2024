# About

Record vim keystrokes

`vim input -W script`, careful `-W` overwrites, `-w` appends

Doesn't play back nicely, missing `-` in the `C` command?

Otherwise it does get us to the sorted stage somehow, investigate

To run back, `vim input -s script`, or `:source! script`

# Day 1

- Produce intermediate sorted arrays `sorted` with `script-sorted`
- TODO: part1 and part2 scripts
- Part2 needed a macro that uses `:read !grep 'num2$' intermediate | wc -l` after first saving a intermediate file (or using input)

- Try again in plain vim, `\vim`, neovim may be affecting things

