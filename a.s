section .text
            global _start       ;リンカのための宣言

_start:                         ;リンカに開始ポイントを伝える
        mov     edx,len         ;メッセージの長さ
        mov     ecx,msg         ;メッセージ
        mov eax, 0
        mov     ebx,1           ;標準出力を指定
        mov     eax,4           ;システムコール番号 (sys_write)
        int     0x80            ;システムコール

        mov     eax,1           ;システムコール番号 (sys_exit)
        int     0x80            ;システムコール
  call [esi+4]
        section .data

        msg     db      'Hello, world!',0xa       ;メッセージ文字列
        len     equ     $ - msg                  ;文字列の長さ
