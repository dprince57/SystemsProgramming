; copy (the cp command in Unix/Linux Systems)
; here, we will impliment the "cp" command
; syntax:
; ./copy src dest
; Compile with: nasm -f elf copy.asm
; Link with (64 bit systems require elf_i386 option): ld -m elf_i386 copy.o -o copy
; or use the makefile by running "make"
;

; include external files
%include 'functions.asm' ; using the functions file from the tutorial

; section for define statements
SECTION .data
usage db 'Usage: ./copy src dst', 0Ah ; storing a usage statement
                                      ; note that 0Ah is 10 in decimal
                                      ; 10 is the ASCII number for newline character
; section for variables
; resb means "reserve number of bytes"
SECTION .bss
src     resb 8              ; buffer for file descriptor of source file
dst     resb 8              ; buffer for file descriptor of destination file
sz      resb 16             ; reserving 16 bytes to store integer for file size
buffer  resb 10000          ; reserving 10000 bytes to store the contents of a file

; our main program is below
SECTION .text
global _start

; main function
_start:


; deal with command line arguments here
; argc is stored in the ecx register
; argv is stored as a stack in the eax register
; recall that cp takes two arguments, so argc will be 3,
args:
    pop ecx         ; pop ecx to get argc off of the stack
    pop eax         ; pop eax to get argv[0] (the filename), off of the stack
    cmp ecx, 3h     ; checking if argc is 3 (number of args needed)
    jne printUsage  ; if above is false, go to printUsage, else continue below
                    ; "jne" - jump if not equal


    
; open the source file for reading using the "open" system call
open_src:
    pop eax         ; pop eax to get argv[1] off the stack, this has our filename
    mov ebx, eax    ; open expects the filename to be where?
    mov eax, 5      ; open expects the opcode for open to be where?,
    mov ecx, 0      ; open expects the mode (read only is 0) to be where?
    int 0x80    ; call the kernel to invoke the system call

    mov [src], eax  ; open returns a file descriptor in eax
                    ; move it to the src variable we declared above

; read the source file into a buffer using the "read" system call
read_src:
    mov eax, 3   ; move opcode for read into where?
    mov ebx, [src]   ; file descriptor for source goes where?
    mov ecx, buffer   ; buffer goes where?
    mov edx, 10000   ; max number of bytes to be read goes where?
    int 0x80   ; now call the kernel

                     ; read returns number of bytes read,
    mov [sz], eax    ; store this value in sz, so we can use this later


; create the destination file using the "creat" system call
; creat expects 2 arguments, file permissions in ecx, and filename in ebx
create_dst:
    pop eax ; now pop eax again to get the destination file

    mov ecx, 0644o ; set file permissions to 0777o 
    mov ebx, eax   ; move filename currently in eax to where?
    mov eax, 8   ; 32 bit opcode for "creat" goes where
    int 0x80   ; call the kernel

    mov [dst], eax ; creat returns a file descriptor in eax
                   ; store that in dst, so we can use it to write the buffer to the new file

; write the buffer into the destination file using the "write" system call
write_dst:
    mov eax, 4      ; opcode for write goes where?
    mov ebx, [dst]  ; file descriptor for destination goes where?
    mov ecx, buffer ; buffer goes where?
    mov edx, [sz]  ; number of bytes read earlier (stored in sz), goes where?
    int 0x80        ; then call the kernel

; we will now invoke the "close" system call twice, to close both file descriptors
;
; close the source file
close_src:
   mov eax, 6       ; opcode for close goes where?
   mov ebx, [src]   ; file descriptor for src goes where?
   int 0x80         ; now call the kernel

; close the destination file
closedst:
   mov eax, 6       ; opcode for close goes in eax
   mov ebx, [dst]   ; file descriptor for dst goes where?
   int 0x80         ; now call the kernel

   jmp done
   
; print a usage statement
printUsage:
    mov     eax, usage  ; move usage statement to eax
    call    sprint      ; call sprint (external function in functions.asm)
    jmp     done        ; go to section labeled "done"

; exit the program
done:
    call quit

