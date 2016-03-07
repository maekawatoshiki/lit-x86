
out:     ファイル形式 binary


セクション .data の逆アセンブル:


jmp    0x108
push   ebp
mov    ebp,esp
sub    esp,0x20
mov    eax,DWORD PTR [ebp-0x4]
push   eax
mov    eax,0x2
mov    ebx,eax
pop    eax
cmp    eax,ebx
sete   al
movzx  eax,al
cmp    eax,0x0
jne    0x2c
jmp    0x3b
mov    eax,0x1
jmp    0x100
jmp    0x3b
mov    eax,DWORD PTR [ebp-0x4]
push   eax
mov    eax,0x2
mov    ebx,eax
pop    eax
mov    edx,0x0
idiv   ebx
mov    eax,edx
push   eax
mov    eax,0x0
mov    ebx,eax
pop    eax
cmp    eax,ebx
sete   al
movzx  eax,al
cmp    eax,0x0
jne    0x6b
jmp    0x7a
mov    eax,0x0
jmp    0x100
jmp    0x7a
mov    eax,0x3
mov    DWORD PTR [ebp-0x8],eax
nop
mov    eax,DWORD PTR [ebp-0x8]
push   eax
mov    eax,DWORD PTR [ebp-0x8]
mov    ebx,eax
pop    eax
imul   ebx
push   eax
mov    eax,DWORD PTR [ebp-0x4]
mov    ebx,eax
pop    eax
cmp    eax,ebx
setle  al
movzx  eax,al
cmp    eax,0x0
jne    0xa8
jmp    0xf9
mov    eax,DWORD PTR [ebp-0x4]
push   eax
mov    eax,DWORD PTR [ebp-0x8]
mov    ebx,eax
pop    eax
mov    edx,0x0
idiv   ebx
mov    eax,edx
push   eax
mov    eax,0x0
mov    ebx,eax
pop    eax
cmp    eax,ebx
sete   al
movzx  eax,al
cmp    eax,0x0
jne    0xd6
jmp    0xe5
mov    eax,0x0
jmp    0x100
jmp    0xe5
mov    eax,0x2
push   eax
mov    eax,DWORD PTR [ebp-0x8]
pop    ebx
add    eax,ebx
mov    DWORD PTR [ebp-0x8],eax
jmp    0x82
nop
nop
mov    eax,0x1
add    esp,0x20
leave  
ret    
push   ebp
mov    ebp,esp
sub    esp,0x1c
mov    esi,DWORD PTR [ebp+0xc]
mov    eax,0x2
mov    DWORD PTR [ebp-0x4],eax
nop
mov    eax,DWORD PTR [ebp-0x4]
push   eax
mov    eax,0x64
mov    ebx,eax
pop    eax
cmp    eax,ebx
setl   al
movzx  eax,al
cmp    eax,0x0
jne    0x13b
jmp    0x180
mov    eax,DWORD PTR [ebp-0x4]
mov    DWORD PTR [esp-0xc],eax
call   0x5
cmp    eax,0x0
jne    0x151
jmp    0x16c
mov    eax,DWORD PTR [ebp-0x4]
mov    DWORD PTR [esp],eax
call   DWORD PTR [esi]
mov    eax,0x8eefa18
mov    DWORD PTR [esp],eax
call   DWORD PTR [esi+0x4]
call   DWORD PTR [esi+0x8]
jmp    0x16c
mov    eax,0x1
push   eax
mov    eax,DWORD PTR [ebp-0x4]
pop    ebx
add    eax,ebx
mov    DWORD PTR [ebp-0x4],eax
jmp    0x11c
nop
nop
add    esp,0x1c
leave  
ret    
