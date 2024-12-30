### Running the code

```bash
$ clang++ -std=c++11 main.cpp
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

#### Example 2

```
(let ((x 0)) (if (< x 3) 3 4))
(let ((x 0)) (let ((temp_0 (< x 3))) (if temp_0 3 4)))
        .globl main
main:
        pushq %rbp
        movq %rsp, %rbp
        subq $16, %rsp
        movq $0, -8(%rbp)
        cmpq 3, -8(%rbp)
        setl %al
        movzbq %al, %rsi
        cmpq $1, %rsi
        je block_16
        jmp block_17

block_16:

        movq $3, %rdi
        callq print_int
        jmp conclusion

block_17:

        movq $4, %rdi
        callq print_int
        jmp conclusion

conclusion:

        addq $16, %rsp
        popq %rbp
        retq
```

