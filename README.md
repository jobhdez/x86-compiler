### x86 compiler
this compiler is based on chapers 2, 4, 5 in [Dr. Siek's book](https://github.com/IUCompilerCourse/Essentials-of-Compilation).

### running the code
```bash
$ make main
$ make test
```

#### Example 1
```
(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin (set sum (+ sum 3)) (set i (+ i 1)))) sum)))

(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin (set sum (+ sum 3)) (set i (+ i 1)))) sum)))

        .globl main
main:
        pushq %rbp
        movq %rsp, %rbp
        subq $16, %rsp
        movq $0, -8(%rbp)
        movq $0, -16(%rbp)
loop_16:
        movq $3, %rax
        addq %rax, -8(%rbp)
        movq $1, %rax
        addq %rax, -16(%rbp)
        cmpq $5, -16(%rbp)
        jl loop_16
        movq -8(%rbp) %rdi
        callq print_int
conclusion:
        addq $16, %rsp
        popq %rbp
        retq
```




        addq $16, %rsp
        popq %rbp
        retq
```

