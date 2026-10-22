# Generates src/census_stubs.S: signature-agnostic counting stubs (log-only diagnostics).
N_CEN = 256   # GameInput vtable slots (4 tables x 64)
N_AK = 16     # Wwise PostEvent overloads
o = ['.text']
for i in range(N_CEN):
    o += [f'.globl cen_stub_{i}', f'cen_stub_{i}:',
          f'  movq %rdx, g_cen_rdx+{8*i}(%rip)',
          f'  lock incl g_cen_cnt+{4*i}(%rip)',
          f'  jmp *g_cen_orig+{8*i}(%rip)']
for i in range(N_AK):
    o += [f'.globl ak_stub_{i}', f'ak_stub_{i}:',
          '  push %rcx', '  push %rdx', '  push %r8', '  push %r9', '  sub $0x28, %rsp',
          '  mov %rdx, %r8', '  mov %rcx, %rdx', f'  mov ${i}, %ecx', '  call ak_log',
          '  add $0x28, %rsp', '  pop %r9', '  pop %r8', '  pop %rdx', '  pop %rcx',
          f'  jmp *g_ak_orig+{8*i}(%rip)']
o += ['.data', '.globl cen_stub_table', '.p2align 3', 'cen_stub_table:'] + [f'  .quad cen_stub_{i}' for i in range(N_CEN)]
o += ['.globl ak_stub_table', 'ak_stub_table:'] + [f'  .quad ak_stub_{i}' for i in range(N_AK)]
open('src/census_stubs.S', 'w').write('\n'.join(o) + '\n')
