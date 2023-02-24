import creopyson as cps

c = cps.Client()
c.connect()

if not c.is_creo_running():
    print("CREO is not running!")
    exit()

myassembly = '2bars.asm'

print("FEATURE LIST ASM")
print(c.feature_list(myassembly))
mypart = 'bar.prt'
print("MASS PROP PART")
print(cps.file.massprops(c, mypart ))
print("FEATURE LIST PRT")
print(c.feature_list(mypart))
