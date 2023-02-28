import creopyson as cps
import cadquery as cq

client = cps.Client()
client.connect()

if not client.is_creo_running():
    print("CREO is not running!")
    exit()

myassembly = '2bars.asm'

print("FEATURE LIST ASM")
print(client.feature_list(myassembly))
mypart = 'bar.prt'
mypart2 = 'barlonger.prt'
print("MASS PROP PART")
print(client.file_massprops(file_=mypart))
print("FEATURE LIST PRT")
print(client.feature_list(mypart))

print("PRINT ONLY AXIS")
print(client.feature_list(mypart,type_="DATUM AXIS"))
print("GET TRANSFORM")
print(client.file_get_transform(path=[59])) # 59 is the id of BAR
#edges = client.get_edges()
#cps.interface.export_file(client, file_type="STEP")
client.interface_export_file(file_=mypart,file_type="STEP")
client.interface_export_file(file_=mypart2,file_type="STEP")
afile = cq.importers.importStep(client.creo_pwd()+'barlonger.stp')
cq.exporters.export(afile,'barlonger.stl')
afile = cq.importers.importStep(client.creo_pwd()+'bar.stp')
cq.exporters.export(afile,'bar.stl')

