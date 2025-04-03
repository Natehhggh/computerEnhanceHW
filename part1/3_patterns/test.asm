add bx, [bx + si] 
add bx, [bp + 0] 
add si, 2 
add bp, 2 
add cx, 8 
add bx, [bp + 0] 
add cx, [bx + 2] 
add bh, [bp + si + 4] 
add di, [bp + di + 6] 
add [bx + si], bx 
add [bp + 0], bx 
add [bp + 0], bx 
add [bx + 2], cx 
add [bp + si + 4], bh 
add [bp + di + 6], di 
add [bx], 34 
add [bp + si + 65512], 29 
add ax, [bp + 0] 
add al, [bx + si] 
add ax, bx 
add al, ah 
add ax, 65512 
add al, 65506 
add al, 9 
sub bx, [bx + si] 
sub bx, [bp + 0] 
sub si, 2 
sub bp, 2 
sub cx, 8 
sub bx, [bp + 0] 
sub cx, [bx + 2] 
sub bh, [bp + si + 4] 
sub di, [bp + di + 6] 
sub [bx + si], bx 
sub [bp + 0], bx 
sub [bp + 0], bx 
sub [bx + 2], cx 
sub [bp + si + 4], bh 
sub [bp + di + 6], di 
sub [bx], 34 
sub [bx + di], 29 
sub ax, [bp + 0] 
sub al, [bx + si] 
sub ax, bx 
sub al, ah 
sub ax, 65512 
sub al, 65506 
sub al, 9 
cmp bx, [bx + si] 
cmp bx, [bp + 0] 
cmp si, 2 
cmp bp, 2 
cmp cx, 8 
cmp bx, [bp + 0] 
cmp cx, [bx + 2] 
cmp bh, [bp + si + 4] 
cmp di, [bp + di + 6] 
cmp [bx + si], bx 
cmp [bp + 0], bx 
cmp [bp + 0], bx 
cmp [bx + 2], cx 
cmp [bp + si + 4], bh 
cmp [bp + di + 6], di 
cmp [bx], 34 
cmp [65506], 29 
cmp ax, [bp + 0] 
cmp al, [bx + si] 
cmp ax, bx 
cmp al, ah 
cmp ax, 65512 
cmp al, 65506 
cmp al, 9 
JNE 2
JNE -4
JNE -6
JNE -4
JE -2
JL -4
JLE -6
JB -8
JBE -10
JP -12
J0 -14
JS -16
JNE -18
JNL -20
JNLE -22
JNB -24
JNBE -26
JNP -28
JN0 -30
JNS -32
LOOP -34
LOOPZ -36
LOOPNZ -38
JCXZ -40
