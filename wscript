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
	conf.env.append_value('CFLAGS', ['-mmcu=atmega2560', '-DF_CPU=16000000'])
	conf.env.append_value('CFLAGS', ['-std=gnu99', '-Wstrict-prototypes'])
	conf.env.append_value('CFLAGS', ['-gstabs', '-Os' ])
	
	conf.env.append_value('LINKFLAGS', ['-mmcu=atmega2560', '-DF_CPU=16000000'])

	#conf.env.append_value('CFLAGS', ['-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums'])

def build(bld):
	#bld.program(source='gccregs/regs.c', target='regs.elf')
	#bld(rule='avr-objdump -h -S ${SRC} > ${TGT}', source='regs.elf', target='regs.lst')
	#bld.program(source='gccregs/regvars.c', target='regvars.elf')
	#bld(rule='avr-objdump -h -S ${SRC} > ${TGT}', source='regvars.elf', target='regvars.lst')
	#bld.program(source='kernelwithdelay/delaykernel.c', target='delaykernel.elf')
	bld.program(source='kernelwithdelay/fish.c', target='fish.elf', use='test', includes=['.'])
	bld.objects(source='kernelwithdelay/hserial.c', target='test')

	#bld.program(source='kernelwithdelay/icptest.c', target='icptest.elf')

	#bld(rule='cp *.elf ~/Mappings/fileserver/4Proteus/studyrtos', always=True, after='cprogram')
	bld(rule='cp *.elf ~/.gvfs/d\ on\ wenjingsun-pc/4Proteus', always=True, after='cprogram')
