配合CE的自动汇编使用

- send hook 

```
alloc(newmem,2048,"EasyAntiCheat_launcher.exe"+1C5060) 
label(returnhere)
label(originalcode)
label(exit)

newmem: //this is allocated memory, you have read,write,execute access
//place your code here
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

push r8           //declen
push rdx          //decbuf
push rcx          //socket
mov rax,GetCurrentThreadId
call rax
push rax          //tid
mov rax,GetCurrentProcessId
call rax
mov r9,rsp        //pointer
mov r8,eax        //pid
mov rdx,0x1400    //400+1000
mov rcx,0x4ed20d8  //hwnd
sub rsp,0x20
mov rax,SendMessageA
call rax
add rsp,0x20
add rsp,0x20

pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
originalcode:
mov [rsp+08],rbx

exit:
jmp returnhere

"EasyAntiCheat_launcher.exe"+1C5060:
jmp newmem
returnhere:

```

- recv hook

```
alloc(newmem,2048,"EasyAntiCheat_launcher.exe"+1C4B59) 
label(returnhere)
label(originalcode)
label(exit)
newmem: //this is allocated memory, you have read,write,execute access
//place your code here
test rbx,rbx
je originalcode
cmp dword ptr ds:[rbx+0x70],0
je originalcode
push rax
push rbx          //decrypto
push rcx
push rdx
push rsi
push rdi
push rbp          //socket
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

mov eax,[ebx+0x70]
push rax          //len
mov rax,[ebx+0x28]
push rax          //buf
push ebp          //socket
mov rax,GetCurrentThreadId
call rax
push rax          //tid
mov rax,GetCurrentProcessId
call rax
mov r9,rsp        //pointer
mov r8,eax        //pid
mov rdx,0x1401    //400+1000
mov rcx,0x4ed20d8  //hwnd
sub rsp,0x20
mov rax,SendMessageA
call rax
add rsp,0x20
add rsp,0x20

pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
originalcode:
cmp byte ptr [rsp+000005D8],00

exit:
jmp returnhere

"EasyAntiCheat_launcher.exe"+1C4B59:
jmp newmem
nop 3
returnhere:


```

