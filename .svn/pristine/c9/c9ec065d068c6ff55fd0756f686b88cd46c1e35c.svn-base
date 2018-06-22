#include "i8042.h"
.global _g_bufferData
.global _getBuffer

.data
_g_bufferData:
	.byte 0

.text
_getBuffer:
	inb $KBD_OUT_BUF
	movb %al, _g_bufferData
	ret
