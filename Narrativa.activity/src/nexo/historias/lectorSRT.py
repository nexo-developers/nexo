from pysrt import SubRipFile, SubRipTime

# hay que ver todavia como usar SubRipTime que nos va a solucionar la lectura

subs = SubRipFile.open('14Blades.srt', encoding='iso-8859-1')

print("Hay" ,subs.__len__()," subtitulos")

linea= subs[0]
print( linea.text)
print("inicio" ,linea.start.seconds," segundos.")
print("fin" ,linea.end.seconds," segundos.")


linea= subs[1]
print( linea.text)
print("inicio" ,linea.start.seconds," segundos.")
print("fin" ,linea.end.seconds," segundos.")

linea= subs[14]
print( linea.text)
print("inicio" ,linea.start.minutes,"minutos con", linea.start.seconds,"segundos.")
print("fin",linea.start.minutes,"minutos con", linea.end.seconds,"segundos.")

#equivalent
#part = subs.slice(ends_after=SubRipTime(0, 0, 40))
#part = subs.slice(ends_after=(0, 0, 40))
#part = subs.slice(ends_after={'seconds': 40})

#part.shift(seconds=-2)
#subs.save('other/path.srt', 'utf-8');