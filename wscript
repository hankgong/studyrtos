VERSION='0.0.1'
APPNAME='simplec'

from waflib.Tools.compiler_c import c_compiler

top = '.'
out = 'build'

c_compiler['linux'] = ['avr-gcc']

def options(opt):
	opt.load('compiler_c')

def configure(conf):
	conf.load('compiler_c')
	conf.env.append_value('CFLAGS', ['-g','-o2',  '-mmcu=atmega328p', '-DF_CPU=16000000UL'])
	
def build(bld):
	bld.program(source='gccregs/regs.c', target='regs.elf')
	bld(rule='avr-objdump -h -S ${SRC} > ${TGT}', source='regs.elf', target='regs.lst')
	bld.program(source='gccregs/regvars.c', target='regvars.elf')
	bld(rule='avr-objdump -h -S ${SRC} > ${TGT}', source='regvars.elf', target='regvars.lst')
	bld.program(source='kernelwithdelay/delaykernel.c', target='delaykernel.elf')
