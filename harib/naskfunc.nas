; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; �I�u�W�F�N�g�t�@�C������郂�[�h	
[INSTRSET "i486p"]				; �w��cpu?�^�C?����??ECX?�񑶊펧�s��??
[BITS 32]						; 32�r�b�g���[�h�p�̋@�B�����点��


; �I�u�W�F�N�g�t�@�C���̂��߂̏��

[FILE "naskfunc.nas"]			; �\�[�X�t�@�C�������

		GLOBAL	_io_hlt;,_write_mem8	; ���̃v���O�����Ɋ܂܂��֐���


; �ȉ��͎��ۂ̊֐�

[SECTION .text]		; �I�u�W�F�N�g�t�@�C���ł͂���������Ă���v���O����������

_io_hlt:	; void io_hlt(void);
		HLT
		RET

;_write_mem8:	; void write_mem8(int addr, int data);
;		MOV		ECX,[ESP+4]		; ESP+4�����I���n��
;		MOV		AL,[ESP+8]		; ESP+8�����I������
;		MOV		[ECX],AL
;		RET