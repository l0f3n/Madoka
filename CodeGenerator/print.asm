
;; ===== print.asm =====

section .bss
	printArea resb 100			; Where we store our converted string
	printAreaIndex resb 8		; Points to current index into printArea

section .text

__print_integer:
	;; Before calling this function, store the value that you want to
	;; print in rax

	push rax					; Save rax for later, to see if its negative

	test rax, rax
	jns _prepare_conversion 	; If not negative

	;; Convert negative number to positive
	not rax
	add rax, 1

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

	pop rax

	test rax, rax
	jns _print_chars			; If its not negative, just print chars

	;; If its negative, add a minus sign to the front
	inc rcx						; Move to next index
	mov [printAreaIndex], rcx	; Store new index

	mov rbx, 45					; Load hyphen '-'
	mov [rcx], rbx				; Store hyphen

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

__print_bool:
	;; Put the values 0 or 1 in rax to print them

	mov rcx, printArea 			; Load beginning of printArea

	cmp	rax, 0
	je _store_false

_store_true:

	mov [rcx], dword 'true'

	add rcx, 4					; Move forward as many bytes as we wrote
	mov rdx, 5					; We want to print 'true\n', length = 5

	jmp _print_bool_chars

_store_false:

	mov [rcx], dword 'fals'
	add rcx, 4					; Move forward as many bytes as we wrote

	mov [rcx], byte 'e'
	add rcx, 1					; Move forward as many bytes as we wrote

	mov rdx, 6					; We want to print 'false\n', length = 6

_print_bool_chars:

	mov rbx, 10					; Load newline
	mov [rcx], rbx				; Store newline at beginning of printArea

	inc rcx

	;;  Write syscall
	mov rax, 0x1
	mov rdi, 0x1
	mov rsi, printArea
	;; rdx is set previously, depending on true or false
	syscall

	ret


;; ===== end print.asm =====
