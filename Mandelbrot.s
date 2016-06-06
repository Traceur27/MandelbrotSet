global Mandelbrot


section .data
    align 32 ;must be align - othervise there would be seg fault
    one: dd 1.0
    four: dd 4.0
    align 32
    incr: dd 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0

section .text

; dx - xmm0
; dy - xmm1
; x1 - xmm2
; y1 - xmm3
; width - rdi
; height - rsi
; maxIters - rdx
; pixels - rcx
; rounded width - r8

Mandelbrot:
    push rbp
    push r12
    push r13
    push r14
    push r15


    ;VCVTPD2PS xmm0, ymm0                                   ;change double to float - compiler pass float as double
    vshufps ymm0, ymm0, 0                                   ;put dx values in down half of ymm0
    VINSERTF128 ymm0, ymm0, xmm0, 1                         ;fill ymm0 with dx values

    ;VCVTPD2PS xmm1, ymm1
    vshufps ymm1, ymm1, 0
    VINSERTF128 ymm1, ymm1, xmm1, 1                         ;all dy in ymm1

    ;VCVTPD2PS xmm2, ymm2
    vshufps ymm2, ymm2, 0
    VINSERTF128 ymm2, ymm2, xmm2, 1                         ;all x1 in ymm2

    ;VCVTPD2PS xmm3, ymm3
    vshufps ymm3, ymm3, 0
    VINSERTF128 ymm3, ymm3, xmm3, 1                         ;all y1 in ymm3

    VBROADCASTSS ymm4, [one]                                ;all 1.0 in ymm4 - to increment iterations
    VBROADCASTSS ymm5, [four]                               ;all 4.0 in ymm5 - 2^2 to compare


    VXORPS ymm6, ymm6, ymm6                                 ;zero out j counter
    xor r9, r9                                              ;zero out j counter

    mainloop:
    cmp rsi, r9                                             ;j == height
    je endmaninloop

    vmovaps ymm7, [incr]

        xor r10, r10
        innerloop:
        cmp r8, r10                                         ;width == i
        je endinnerloop                                     ;moving by 8 in each iteration


        VMULPS ymm8, ymm7, ymm0                             ;x0 = incrValues*dx
        VADDPS ymm8, ymm8, ymm2                             ;x0 += x1
        VMULPS ymm9, ymm6, ymm1                             ;y0 = j*dy
        VADDPS ymm9, ymm9, ymm3                             ;y0 += y1

        VXORPS ymm10, ymm10, ymm10
        vmovaps ymm11, ymm10
        vmovaps ymm12, ymm10

        xor r11, r11                                        ;test value
        xor r12, r12                                        ;iteration counter

            iterationloop:

            VMULPS ymm13, ymm11, ymm11                      ;xi * xi
            VMULPS ymm14, ymm12, ymm12                      ;yi * yi
            VADDPS ymm15, ymm13, ymm14                      ;xi^2 + yi^2

            VCMPPS ymm15, ymm15, ymm5, 17                   ;checking if values are more then 4; 17 - less then

            VMOVMSKPS r11, ymm15                            ;store test value in register
            and r11, 255                                    ;lower 8 bits are comparisons

            VXORPS ymm5, ymm5, ymm5
            VEXTRACTF128 xmm5, ymm15, 1
            VPAND xmm5, xmm5, xmm4
            VPAND xmm15, xmm15, xmm4
            VINSERTF128 ymm15, ymm15, xmm5, 1

            VBROADCASTSS ymm5, [four]

            VADDPS ymm10, ymm10, ymm15

            VMULPS ymm15, ymm11, ymm12                      ;xi*yi
            VSUBPS ymm11, ymm13, ymm14                      ;xi*xi-yi*yi
            VADDPS ymm11, ymm11, ymm8                       ;xi <- xi*xi-yi*yi+x0 done!
            VADDPS ymm12, ymm15, ymm15                      ;2*xi*yi
            VADDPS ymm12, ymm12, ymm9                       ;yi <- 2*xi*yi+y0

            inc r12

            cmp r11, 0                                      ;untill all values reach more then 4
            je iterationloopend

            cmp r12, rdx                                    ;or maxIter condition is met
            jl iterationloop


            iterationloopend:

        VCVTPS2DQ ymm10, ymm10

        vMOVDQU [rcx], ymm10
        add rcx, 32


        VADDPS ymm7, ymm7, ymm5                             ;next i position - increment each slot by 8
        VADDPS ymm7, ymm7, ymm5

        add r10, 8
        jmp innerloop
        endinnerloop:

    VADDPS ymm6, ymm6, ymm4
    inc r9
    jmp mainloop

    endmaninloop:


    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret