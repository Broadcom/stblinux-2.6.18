#source: x86-64-io.s
#objdump: -dwMintel
#name: x86-64 rex64 in/out (Intel disassembly)

.*: +file format .*

Disassembly of section .text:

0+000 <_in>:
   0:	48 ed                	rex64 in     eax,dx
   2:	66                   	data16
   3:	48 ed                	rex64 in     eax,dx

0+005 <_out>:
   5:	48 ef                	rex64 out    dx,eax
   7:	66                   	data16
   8:	48 ef                	rex64 out    dx,eax

0+00a <_ins>:
   a:	48 6d                	rex64 ins    DWORD PTR es:\[rdi\],dx
   c:	66                   	data16
   d:	48 6d                	rex64 ins    DWORD PTR es:\[rdi\],dx

0+00f <_outs>:
   f:	48 6f                	rex64 outs   dx,DWORD PTR ds:\[rsi\]
  11:	66                   	data16
  12:	48 6f                	rex64 outs   dx,DWORD PTR ds:\[rsi\]
#pass
