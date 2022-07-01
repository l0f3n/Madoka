
;; ===== print.asm =====

section .bss
	printArea resb 100			; Where we store our converted string
	printAreaIndex resb 8		; Points to current index into printArea

section .text

__print_integer:
	;; Before calling this function, store the value that you want to
	;; print in rax

_prepare_conversion:
	mov rcx, printArea 			; Load beginning of printArea

	mov rbx, 10					; Load newline
	mov [rcx], rbx				; Store newline at beginning of printArea

_conv_digits_to_chars:
	;; Move to next index
	inc rcx						; Move to next index
	mov [printAreaIndex], rcx	; Store new index

	;; Perform division, value stored in rax, remainder in rdx
	mov rdx, 0					; Clear rdx, used implicitly in division
	mov rbx, 10					; Divide by 10
	div rbx						; Divide rax by rbx

	;; Perform conversion and store char
	add rdx, 48					; Convert the number to a char
	mov [rcx], dl				; Store char (dl lowest 8 bits from rdx)

	cmp rax, 0					; Do we have any more numbers to convert?
	jne _conv_digits_to_chars	; If we do, repeat this loop

_print_chars:
	mov  rcx, [printAreaIndex]	; Load character

	;;  Write syscall
	mov rax, 0x1
	mov rdi, 0x1
	mov rsi, rcx
	mov rdx, 1
	syscall

	mov rcx, [printAreaIndex]	; Load character
	dec rcx						; Move backward one character
	mov [printAreaIndex], rcx	; Store new position

	cmp rcx, printArea			; Are we at beginning of string?
	jge _print_chars 			; If not, print another character

	ret

;; ===== end print.asm =====
