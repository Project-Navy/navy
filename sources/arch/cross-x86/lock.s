; Copyright (C) 2021 Keyboard Slayer
; 
; This file is part of navy.
; 
; navy is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; navy is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with navy.  If not, see <http://www.gnu.org/licenses/>.

[ GLOBAL arch_acquire ]
arch_acquire:
    cli
    lock bts dword [rdi], 0
    jc .spin 
    ret 

.spin:
    pause 
    test dword [rdi], 1
    jnz .spin 
    jmp arch_acquire

[ GLOBAL arch_release ]
arch_release:
    mov dword [rdi], 0
    sti
    ret
